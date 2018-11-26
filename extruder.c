/*
 * extruder.c
 * Author : Justin Doss
 *
 * This file implements the functionality of the temperature sensor and the dynamic
 * PWM for the extruder.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#include "defs.h"

#define MAX_TEMP 230
#define ROOM_TEMP 30
//const unsigned char PREHEAT_THRESHOLD = 30;

#define READS_PER_SEC 20
#define TEMP_PERIOD 1000 / TICK_PERIOD_A / READS_PER_SEC

#define MAX_HL 100
#define ACCEPT_RANGE 5
#define NUM_PREV_ERRS 3

enum PID_state { PID_OFF, PREHEAT, STABLE, COOL, HEAT };
enum PWM_state { PWM_LOW, PWM_HIGH };

volatile unsigned short desiredTemp = 0;
volatile unsigned short prevDesiredTemp = 0;
volatile float prevTemp = 0.0;
volatile float curTemp = 0.0;

volatile unsigned char H = 0;
volatile unsigned char L = 100;

volatile float errors[NUM_PREV_ERRS];
volatile unsigned char prevIndex = 0;
volatile float curI = 0.0;

#define PID_I KI * 0.003 * TEMP_PERIOD
#define PID_D KD / (0.001 * TEMP_PERIOD)

_task heakerTask, tempTask;

/*
 * Look up table for ATC Semitec 104GT-2 100K thermistor
 * pull up R: 4.7K
 * beta: 4267K
 * max adc: 1023
 * https://www.reprap.org/wiki/Thermistor#ATC_Semitec_104GT-2
 */
#define NUMTEMPS 20
short temptable[NUMTEMPS][2] = {
        {1,    713},
        {54,   236},
        {107,  195},
        {160,  172},
        {213,  157},
        {266,  144},
        {319,  134},
        {372,  125},
        {425,  117},
        {478,  110},
        {531,  103},
        {584,  96},
        {637,  89},
        {690,  83},
        {743,  75},
        {796,  68},
        {849,  59},
        {902,  48},
        {955,  34},
        {1008, 3}
};

void Extruder_init()
{
    INITPIN(EXTRUDER_PIN, OUTPUT, LOW);

    heakerTask.state = PWM_LOW;
    heakerTask.period = 100 / TICK_PERIOD_A;
    heakerTask.elapsedTime = 0;
    heakerTask.TickFct = &Extruder_onTickHeater;

    tempTask.state = PID_OFF;
    tempTask.period = TEMP_PERIOD; // 1 second
    tempTask.elapsedTime = 0;
    tempTask.TickFct = &Extruder_onTickSensor;

    for (unsigned char j = 0; j < NUM_PREV_ERRS; ++j)
    {
        errors[j] = 0.0;
    }

    addTask(&heakerTask);
    addTask(&tempTask);
}

void Extruder_setTemp(unsigned char temp)
{
    prevDesiredTemp = desiredTemp;
    desiredTemp = temp;

    if (temp == 0)
    {
        SETPIN(EXTRUDER_PIN, LOW);
    }
}

void  Extruder_preHeat()
{
    Extruder_setTemp(190);
}

float Extruder_getTemp()
{
    return curTemp;
}

unsigned short Extruder_getDesiredTemp()
{
    return desiredTemp;
}

//void Extruder_checkTemp()
//{
//    static unsigned char count;
//
//    unsigned short xADC = GETADC(T_SENSOR);
//
//    if (xADC >= E_CUTOFF)
//    {
//        if (++count > 3) // catch glitches
//        {
//            Extruder_setTemp(0);
//            systemFailure("Temp Sensor");
//        }
//        return;
//    }
//    count = 0;
//    unsigned char i;
//    for (i = 1; i < NUMTEMPS; ++i)
//    {
//        if (temptable[i][0] >= xADC)
//        {
//            short base_temp = temptable[i][1];
//            if ((temptable[i][0] - xADC) != 0)
//            {
//                short t_diff = temptable[i - 1][1] - base_temp;
//                short range = temptable[i][0] - temptable[i - 1][0];
//                curTemp = (float)base_temp + ((float)t_diff / (float)range * (float)(temptable[i][0] - xADC));
//            }
//            else
//                curTemp = base_temp;
//            break;
//        }
//    }
//
//    if (tempTask.state == PID_OFF)
//    {
//        prevTemp = curTemp;
//    }
//}

void Extruder_checkTemp()
{
    static unsigned char count;
	
    unsigned short xADC = GETADC(T_SENSOR);
//     xADC = GETADC(T_SENSOR);
//     xADC = GETADC(T_SENSOR);
    //xADC /= 3;
	
    if (xADC >= E_CUTOFF)
    {
        if (++count > 3) // catch glitches
        {
            Extruder_setTemp(0);
            systemFailure("Temp Sensor");
        }
        return;
    }
    count = 0;

    prevTemp = curTemp;

    for (unsigned char i = 1; i < NUMTEMPS; ++i)
    {
        if (temptable[i][0] >= xADC)
        {
            short base_temp = temptable[i][1];
            if ((temptable[i][0] - xADC) != 0)
            {
                short t_diff = temptable[i - 1][1] - base_temp;
                short range = temptable[i][0] - temptable[i - 1][0];
                curTemp = (float)base_temp + ((float)t_diff / (float)range * (float)(temptable[i][0] - xADC));
            }
            else
                curTemp = base_temp;
            break;
        }
    }
}

void Extruder_onTickHeater(_task *task)
{
    static unsigned char i = 0;
    switch (task->state)
    {
        case PWM_LOW:
            if (H > 0 && ++i > L)
            {
                task->state = PWM_HIGH;
                i = 0;
                SETPIN(EXTRUDER_PIN, HIGH);
            }
            break;
        case PWM_HIGH:
            if (L > 0 && ++i > H)
            {
                task->state = PWM_LOW;
                i = 0;
                SETPIN(EXTRUDER_PIN, LOW);
            }
            break;
        default:
            systemFailure("Heater PWM State");
            break;
    }

}

void Extruder_onTickSensor(_task *task)
{
    if (task->state != PID_OFF)
    {
        if (desiredTemp <= ROOM_TEMP)
        {
            task->state = PID_OFF;
            L = MAX_HL;
            H = 0;
        }
        else
        {
            curI -= errors[prevIndex] * PID_I;
        }
    }

    Extruder_checkTemp();

    switch (task->state)
    {
        case PID_OFF:
            if (desiredTemp > ROOM_TEMP)
            {
                task->state = PREHEAT;
                H = MAX_HL;
                L = MAX_HL - H;
            }
            return;
        case PREHEAT:

            break;
        case STABLE:

            break;
        case COOL:

            break;
        case HEAT:

            break;
        default:
            task->state = PID_OFF;
            systemFailure("PID State");
            break;
    }

    if (task->state != PID_OFF && curTemp > MAX_TEMP)
    {
        systemFailure("Temp runaway");
    }

    float curError = (float)(desiredTemp) - curTemp;
    float curDiff = curTemp - prevTemp;
	errors[prevIndex] = curError;
	prevIndex = (prevIndex + 1) % NUM_PREV_ERRS;
    curI += curError * PID_I;

    short newH = KP * curError + curI - (curDiff * PID_D);

    if (newH > MAX_HL)
        newH = MAX_HL;
    else if (newH < 0)
        newH = 0;

    H = newH;
    L = MAX_HL - H;
}

//void Extruder_onTickSensor(_task *task)
//{
//    static unsigned char i;
//
//    if (task->state != PID_OFF && desiredTemp <= ROOM_TEMP)
//    {
//        task->state = PID_OFF;
//        L = MAX_HL;
//        H = 0;
//    }
//
//    switch (task->state)
//    {
//        case PID_OFF:
//            if (desiredTemp > ROOM_TEMP)
//            {
//                task->state = PREHEAT;
//                H = MAX_HL;
//                L = MAX_HL - H;
//                i = 0;
//            }
//            break;
//        case PREHEAT:
//            if (desiredTemp - curTemp < 4 * (curTemp - prevTemp))
//            {
//                H = 0;
//                L = MAX_HL;
//            }
//            if (desiredTemp - curTemp < 2 * ACCEPT_RANGE)
//            {
//                task->state = STABLE;
//                H = MAX_HL / 3;
//                L = MAX_HL - H;
//            }
//            if (prevTemp > curTemp && ++i > 3)
//            {
//                task->state = HEAT;
//                H = MAX_HL / 3;
//                L = MAX_HL - H;
//            }
//            if (prevTemp <= curTemp)
//            {
//                i = 0;
//            }
//            break;
//        case STABLE:
//            if (desiredTemp - curTemp > ACCEPT_RANGE)
//            {
//                task->state = HEAT;
//                if (L > 0)
//                {
//                    ++H;
//                    --L;
//                }
//                i = 0;
//            }
//            else if (curTemp - desiredTemp > ACCEPT_RANGE)// || (2 * curTemp - prevTemp > desiredTemp + ACCEPT_RANGE))
//            {
//                task->state = COOL;
//                if (H > 0)
//                {
//                    --H;
//                    ++L;
//                }
//                H = 0;
//                i = 0;
//            }
//            if (prevTemp > curTemp && desiredTemp - 1.0 > curTemp && L > 0)
//            {
//                ++H;
//                --L;
//            }
//            else if (prevTemp < curTemp && curTemp > desiredTemp + 1.0 && H > 0)
//            {
//                --H;
//                ++L;
//            }
//            break;
//        case COOL:
//            H = 0;
//            if (curTemp - desiredTemp <= ACCEPT_RANGE)
//            {
//                task->state = STABLE;
//                H = MAX_HL - L;
//            }
//            else if (prevTemp < curTemp && ++i > 10)
//            {
//                if (L < MAX_HL)
//                {
//                    ++L;
//                }
//                i = 0;
//            }
//            break;
//        case HEAT:
//            if (desiredTemp - curTemp <= ACCEPT_RANGE)
//            {
//                task->state = STABLE;
//                if (H > 0)
//                {
//                    --H;
//                    ++L;
//                }
//            }
//            else if (prevTemp > curTemp && ++i > 75)
//            {
//                if (L > 0)
//                {
//                    ++H;
//                    --L;
//                }
//                i = 0;
//            }
//            break;
//        default:
//            task->state = PID_OFF;
//            systemFailure("PID State");
//            break;
//    }
//
//    prevTemp = curTemp;
//}