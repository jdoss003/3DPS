/*
 * File: defs.h
 * Author : Justin Doss
 *
 * This file contains all the definitions (configs) for the project.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef DEFS_H_
#define DEFS_H_

#define F_CPU 8000000UL

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "string.h"
#include "ff.h"

void waitingLoop();
void updateDisplay();
void systemFailure(char*);
unsigned char getSysState();

#include "pin_io.h"
#include "lcd.h"
#include "task.h"

// in milliseconds
#define TICK_PERIOD_A 1
#define TICK_PERIOD_B 2000
#define TIMER_A_ISR tickTasks
#define TIMER_B_ISR updateDisplay

#include "timer.h"

#define LCD_ENABLE PD_2
#define LCD_RS PD_3
#define LCD_DATA0 PD_4
#define LCD_DATA1 PD_5
#define LCD_DATA2 PD_6
#define LCD_DATA3 PD_7
#define LCD_BTTNS PA_1

#define LCD_ENTER_LOW 430
#define LCD_ENTER_HIGH 530
#define LCD_BACK_LOW 50
#define LCD_BACK_HIGH 140
#define LCD_UP_LOW 590
#define LCD_UP_HIGH 690
#define LCD_DOWN_LOW 145
#define LCD_DOWN_HIGH 225
#define LCD_MENU_LOW 260
#define LCD_MENU_HIGH 360

#include "extruder.h"

#define T_SENSOR PA_0

#define EXTRUDER_PIN PB_3

#define X_MOTOR_DIR PC_0
#define X_MOTOR_STEP PC_1
#define X_MOTOR_INVERT 1

#define Y_MOTOR_DIR PC_2
#define Y_MOTOR_STEP PC_3
#define Y_MOTOR_INVERT 1

#define Z_MOTOR_DIR PC_5
#define Z_MOTOR_STEP PC_4
#define Z_MOTOR_INVERT 0

#define E_MOTOR_DIR PC_7
#define E_MOTOR_STEP PC_6
#define E_MOTOR_INVERT 0

#define X_ENDSTOP PA_3
#define Y_ENDSTOP PA_2
#define Z_ENDSTOP PA_4

typedef enum axis { X_AXIS, Y_AXIS, Z_AXIS, EXTRUDER } _axis;

#include "stepper.h"
#include "mov_controller.h"

#define X_STEPS_MM 5;
#define Y_STEPS_MM 5;
#define Z_STEPS_MM 10;
#define E_STEPS_MM 20;

#define X_HOME_OFFSET 200;
#define Y_HOME_OFFSET 20;
#define Z_HOME_OFFSET -50;

#define X_MAX_POS 220;
#define Y_MAX_POS 220;
#define Z_MAX_POS 400;

#include "commands.h"
#include "gcode.h"

#endif /* DEFS_H_ */