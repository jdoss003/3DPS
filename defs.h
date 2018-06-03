/*
 * defs.h
 *
 * Created: 5/19/2018 4:04:01 PM
 *  Author: Justin
 */
#ifndef DEFS_H_
#define DEFS_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd_io.h"

// in milliseconds
#define TICK_PERIOD 5

#include "pin_io.h"
#include "task.h"

#include "extruder.h"

#define T_SENSOR PA_0
#define TEMP_R1 4700

#define EXTRUDER_PIN PB_3

#define X_MOTOR_P1 PC_0
#define X_MOTOR_P2 PC_1
#define X_MOTOR_INVERT 1

#define Y_MOTOR_P1 PC_2
#define Y_MOTOR_P2 PC_3
#define Y_MOTOR_INVERT 1

#define Z_MOTOR_P1 PC_5
#define Z_MOTOR_P2 PC_4
#define Z_MOTOR_INVERT 0

#define E_MOTOR_P1 PC_7
#define E_MOTOR_P2 PC_6
#define E_MOTOR_INVERT 0

#define X_ENDSTOP PA_3
#define Y_ENDSTOP PA_2
#define Z_ENDSTOP PA_4

typedef enum axis { X_AXIS, Y_AXIS, Z_AXIS, EXTRUDER } _axis;

#include "stepper.h"
#include "mov_controller.h"

#define X_STEPS_MM 20;
#define Y_STEPS_MM 20;
#define Z_STEPS_MM 20;
#define E_STEPS_MM 20;

#define X_HOME_OFFSET 200;
#define Y_HOME_OFFSET 200;
#define Z_HOME_OFFSET 200;

inline void keepAlive()
{
    volatile unsigned char keep;
    keep = !keep;
}

#endif /* DEFS_H_ */