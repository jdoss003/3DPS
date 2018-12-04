/*
 * File: fan.c
 * Author : Justin Doss
 *
 * This file is a driver for a DC fan
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#include "defs.h"

#define MAX_HL_FAN 255

enum PWM_state { PWM_LOW, PWM_HIGH };

volatile unsigned char H_fan = 0;
volatile unsigned char L_fan = MAX_HL_FAN;

_task fanTask;

void Fan_init()
{
    INITPIN(FAN_PIN, OUTPUT, LOW);

    fanTask.state = PWM_LOW;
    fanTask.period = 100 / TICK_PERIOD_A;
    fanTask.elapsedTime = 0;
    fanTask.TickFct = &Fan_onTickPWM;

    addTask(&fanTask);
}

void Fan_setPWM(unsigned char pwm)
{
    H_fan = pwm;
    L_fan = MAX_HL_FAN - H_fan;
}

void Fan_off()
{
    Fan_setPWM(0);
}

void Fan_onTickPWM(_task *task)
{
    static unsigned char i = 0;
    switch (task->state)
    {
        case PWM_LOW:
            if (H_fan > 0 && ++i > L_fan)
            {
                task->state = PWM_HIGH;
                i = 0;
                SETPIN(FAN_PIN, HIGH);
            }
            break;
        case PWM_HIGH:
            if (L_fan > 0 && ++i > H_fan)
            {
                task->state = PWM_LOW;
                i = 0;
                SETPIN(FAN_PIN, LOW);
            }
            break;
        default:
            systemFailure("PWM State");
            break;
    }

}