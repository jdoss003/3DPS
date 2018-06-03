/*
 * extruder.cpp
 *
 * Created: 5/27/2018 3:34:21 PM
 *  Author: Justin
 */

#include "defs.h"

#define E_CUTOFF 990

const float ROOM_TEMP = 30.0;
const unsigned char MAX_HL = 100;
const unsigned char PREHEAT_THRESHOLD = 20;
const unsigned char ACCEPT_RANGE = 5;

enum PID_state { PID_OFF, PREHEAT, STABLE, COOL, HEAT };
enum PWM_state { PWM_LOW, PWM_HIGH };

static unsigned char desiredTemp = 0;
static float prevTemp = 0.0;
static float curTemp = 0.0;

static unsigned char H = 100;
static unsigned char L = 0;

static unsigned char i;
static _task heakerTask, tempTask;

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

// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency)
{
    static double current_frequency; // Keeps track of the currently set frequency
    // Will only update the registers when the frequency changes, otherwise allows
    // music to play uninterrupted.
    if (frequency != current_frequency)
    {
        if (!frequency)
        { TCCR0A &= 0x08; } //stops timer/counter
        else
        { TCCR0A |= 0x03; } // resumes/continues timer/counter

        // prevents OCR3A from overflowing, using prescaler 64
        // 0.954 is smallest frequency that will not result in overflow
        if (frequency < 0.954)
        { OCR0A = 0xFFFF; }

            // prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
        else if (frequency > 31250)
        { OCR0A = 0x0000; }

            // set OCR3A based on desired frequency
        else
        { OCR0A = (short) (8000000 / (128 * frequency)) - 1; }

        TCNT0 = 0; // resets counter
        current_frequency = frequency; // Updates the current frequency
    }
}

void PWM_on()
{
    TCCR0A = (1 << COM0A0);
    // COM0A0: Toggle PB3 on compare match between counter and OCR0A
    TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
    // WGM02: When counter (TCNT0) matches OCR0A, reset counter
    // CS01 & CS00: Set a prescaler of 64
    set_PWM(0);
}

void PWM_off()
{
    TCCR0A = 0x00;
    TCCR0B = 0x00;
}

void Extruder::init()
{
    INITADC(T_SENSOR);
    INITPIN(PB_3, OUTPUT, LOW);

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
    prevTemp = curTemp;

    unsigned short xADC = GETADC();

    if (xADC < E_CUTOFF)
    {
        unsigned char i;
        for (i = 1; i < NUMTEMPS; ++i)
        {
            if (temptable[i][0] > xADC)
            {
                float base_temp = temptable[i][1];
                float t_diff = temptable[i - 1][1] - base_temp;
                float range = temptable[i][0] - temptable[i - 1][0];
                curTemp = base_temp + (t_diff / range * (temptable[i][0] - xADC));
                return;
            }
        }
    }

    setTemp(0);
    // TODO fail
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
            break;
    }

}

void Extruder::onTickSensor(_task *task)
{
    Extruder::checkTemp();

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
                L = 0;
            }
            break;
        case PREHEAT:
            if (desiredTemp - curTemp < PREHEAT_THRESHOLD)
            {
                H = 0;
                L = MAX_HL;
            }
            if (desiredTemp - curTemp < ACCEPT_RANGE)
            {
                task->state = STABLE;
                H = MAX_HL / 5;
                L = MAX_HL - H;
            }
            if (prevTemp > curTemp)
            {
                task->state = HEAT;
                H = MAX_HL / 4;
                L = MAX_HL - H;
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
            else if (curTemp - desiredTemp > ACCEPT_RANGE)
            {
                task->state = COOL;
                if (H > 0)
                {
                    --H;
                    ++L;
                }
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
            if (curTemp - desiredTemp <= ACCEPT_RANGE)
            {
                task->state = STABLE;
                if (L > 0)
                {
                    ++H;
                    --L;
                }
            }
            else if (prevTemp < curTemp && ++i > 200)
            {
                if (H > 0)
                {
                    --H;
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
            else if (prevTemp > curTemp && ++i > 200)
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
            break;
    }
}