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

// in milliseconds
#define TICK_PERIOD 5

#include "pin_io.h"
#include "task.h"

#define X_MOTOR_P1 PD_7
#define X_MOTOR_P2 PC_0
#define X_MOTOR_INVERT 1

#define Y_MOTOR_P1 PC_1
#define Y_MOTOR_P2 PC_2
#define Y_MOTOR_INVERT 1

#define Z_MOTOR_P1 PC_4
#define Z_MOTOR_P2 PC_3
#define Z_MOTOR_INVERT 0

#define E_MOTOR_P1 PC_6
#define E_MOTOR_P2 PC_5
#define E_MOTOR_INVERT 0

#define X_ENDSTOP PD_4
#define Y_ENDSTOP PD_5
#define Z_ENDSTOP PD_6

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

#endif /* DEFS_H_ */