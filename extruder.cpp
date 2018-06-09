/*
 * extruder.cpp
 * Author : Justin Doss
 *
 * This file implements the functionality of the temperature sensor and the dynamic
 * PWM for the extruder.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */
#include "defs.h"

#define E_CUTOFF 990

const float ROOM_TEMP = 30.0;
const unsigned char MAX_HL = 100;
const unsigned char PREHEAT_THRESHOLD = 30;
const unsigned char ACCEPT_RANGE = 5;

enum PID_state { PID_OFF, PREHEAT, STABLE, COOL, HEAT };
enum PWM_state { PWM_LOW, PWM_HIGH };

unsigned short desiredTemp = 0;
float prevTemp = 0.0;
float curTemp = 0.0;

unsigned char H = 0;
unsigned char L = 100;

unsigned char i = 0;
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

void Extruder::init()
{
    INITADC(T_SENSOR);
    INITPIN(EXTRUDER_PIN, OUTPUT, LOW);

    heakerTask.state = PWM_LOW;
    heakerTask.period = 100 / TICK_PERIOD;
    heakerTask.elapsedTime = 0;
    heakerTask.TickFct = &Extruder::onTickHeater;

    tempTask.state = PID_OFF;
    tempTask.period = 1000 / TICK_PERIOD; // 1 second
    tempTask.elapsedTime = 0;
    tempTask.TickFct = &Extruder::onTickSensor;

    addTask(&heakerTask);
    addTask(&tempTask);
}

void Extruder::setTemp(unsigned char temp)
{
    desiredTemp = temp;
}

float Extruder::getTemp()
{
    return curTemp;
}

void Extruder::checkTemp()
{
	static unsigned char count;

    unsigned short xADC = GETADC();

    if (xADC >= E_CUTOFF)
    {
		if (++count > 3) // catch glitches
        {
			setTemp(0);
			systemFailure("Temp Sensor");
		}
		return;
    }
	count = 0;
	unsigned char i;
	for (i = 1; i < NUMTEMPS; ++i)
	{
		if (temptable[i][0] > xADC)
		{
			short base_temp = temptable[i][1];
			short t_diff = temptable[i - 1][1] - base_temp;
			short range = temptable[i][0] - temptable[i - 1][0];
			if ((temptable[i][0] - xADC) != 0)
			{
				curTemp = (float)base_temp + ((float)t_diff / (float)range * (float)(temptable[i][0] - xADC));
			}
			else
				curTemp = base_temp;
			break;
		}
	}
	
	if (tempTask.state == PID_OFF)
	{
		prevTemp = curTemp;
	}
}

void Extruder::onTickHeater(_task *task)
{
    switch (task->state)
    {
        case PWM_LOW:
            if (H > 0 && ++i > L)
            {
                task->state = PWM_HIGH;
                i = 0;
                SETPIN(PB_3, HIGH);
            }
            break;
        case PWM_HIGH:
            if (L > 0 && ++i > H)
            {
                task->state = PWM_LOW;
                i = 0;
                SETPIN(PB_3, LOW);
            }
            break;
        default:
			systemFailure("PWM State");
            break;
    }

}

void Extruder::onTickSensor(_task *task)
{
    static unsigned char i;

    if (task->state != PID_OFF && desiredTemp <= ROOM_TEMP)
    {
        task->state = PID_OFF;
        L = MAX_HL;
        H = 0;
    }

    switch (task->state)
    {
        case PID_OFF:
            if (desiredTemp > ROOM_TEMP)
            {
                task->state = PREHEAT;
                H = MAX_HL;
                L = MAX_HL - H;
                i = 0;
            }
            break;
        case PREHEAT:
            if (desiredTemp - curTemp < 4 * (curTemp - prevTemp))
            {
                H = 0;
                L = MAX_HL;
            }
            if (desiredTemp - curTemp < 2 * ACCEPT_RANGE)
            {
                task->state = STABLE;
                H = MAX_HL / 3;
                L = MAX_HL - H;
            }
            if (prevTemp > curTemp && ++i > 3)
            {
                task->state = HEAT;
                H = MAX_HL / 3;
                L = MAX_HL - H;
            }
			if (prevTemp <= curTemp)
            {
                i = 0;
            }
            break;
        case STABLE:
            if (desiredTemp - curTemp > ACCEPT_RANGE)
            {
                task->state = HEAT;
                if (L > 0)
                {
                    ++H;
                    --L;
                }
                i = 0;
            }
            else if (curTemp - desiredTemp > ACCEPT_RANGE)// || (2 * curTemp - prevTemp > desiredTemp + ACCEPT_RANGE))
            {
                task->state = COOL;
                if (H > 0)
                {
                    --H;
                    ++L;
                }
				H = 0;
                i = 0;
            }
            if (prevTemp > curTemp && desiredTemp - 1.0 > curTemp && L > 0)
            {
                ++H;
                --L;
            }
            else if (prevTemp < curTemp && curTemp > desiredTemp + 1.0 && H > 0)
            {
                --H;
                ++L;
            }
            break;
        case COOL:
			H = 0;
            if (curTemp - desiredTemp <= ACCEPT_RANGE)
            {
                task->state = STABLE;
                H = MAX_HL - L;
            }
            else if (prevTemp < curTemp && ++i > 10)
            {
                if (L < MAX_HL)
                {
                    ++L;
                }
                i = 0;
            }
            break;
        case HEAT:
            if (desiredTemp - curTemp <= ACCEPT_RANGE)
            {
                task->state = STABLE;
                if (H > 0)
                {
                    --H;
                    ++L;
                }
            }
            else if (prevTemp > curTemp && ++i > 75)
            {
                if (L > 0)
                {
                    ++H;
                    --L;
                }
                i = 0;
            }
            break;
        default:
            task->state = PID_OFF;
			systemFailure("PID State");
            break;
    }
	
	prevTemp = curTemp;
}