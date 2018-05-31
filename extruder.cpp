/*
 * extruder.cpp
 *
 * Created: 5/27/2018 3:34:21 PM
 *  Author: Justin
 */

#include "defs.h"

const float ROOM_TEMP = 28.0;
const unsigned char MAX_HL = 100;
const unsigned char PREHEAT_THRESHOLD = 5;
const unsigned char ACCEPT_RANGE = 2;

enum PID_state { PID_OFF, PREHEAT, STABLE, COOL, HEAT };
enum PWM_state { PWM_LOW, PWM_HIGH };

static unsigned char desiredTemp = 0;
static float prevTemp = 0.0;
static float curTemp = 0.0;

static unsigned char H = 100;
static unsigned char L = 0;

static unsigned char prevH = 0;
static unsigned char prevL = 0;

static unsigned char i;
static _task heakerTask, tempTask;

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
    heakerTask.period = 2;
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
    unsigned short xADCMAX = GETMAXADC();

    // do math

    // set new curTemp;

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

    if (task->state != PID_OFF && desiredTemp <= ROOM_TEMP)
    {
        task->state = PID_OFF;
        L = MAX_HL;
        H = 0;
        prevL = 0;
        prevH = 0;
    }

    switch (task->state)
    {
        case PID_OFF:
            if (desiredTemp > ROOM_TEMP)
            {
                task->state = PREHEAT;
            }
            break;
        case PREHEAT:
            if (desiredTemp - curTemp > PREHEAT_THRESHOLD)
            {
                task->state = HEAT;
                H = MAX_HL;
                L = 0;
            }
            break;
        case STABLE:
            if (desiredTemp - curTemp > ACCEPT_RANGE)
            {
                task->state = HEAT;
                prevH = H + 1;
                prevL = L - 1;
                H = MAX_HL;
                L = 0;
            }
            else if (curTemp - desiredTemp > ACCEPT_RANGE)
            {
                task->state = COOL;
                prevH = H - 1;
                prevL = L + 1;
                H = 0;
                L = MAX_HL;
            }
            break;
        case COOL:
            if (curTemp - desiredTemp <= ACCEPT_RANGE)
            {
                task->state = STABLE;
                H = prevH;
                L = prevL;
            }
            break;
        case HEAT:
            if (desiredTemp - curTemp <= ACCEPT_RANGE)
            {
                task->state = STABLE;
                H = prevH;
                L = prevL;
            }
            break;
        default:
            task->state = PID_OFF;
            break;
    }
}