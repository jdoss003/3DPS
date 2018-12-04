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

#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum axis { X_AXIS, Y_AXIS, Z_AXIS, EXTRUDER } _axis;
typedef enum system_states { SYS_START, SYS_WAITING, SYS_RUNNING, SYS_FAILURE } _system_state;

void waitingLoop(unsigned char delay);
void updateDisplay();
void systemFailure(char* msg);
_system_state getSysState();
void setSystemPrinting(unsigned char isSerial);

#ifdef __cplusplus
}
#endif

// in milliseconds
#define TICK_PERIOD_A 1
#define TICK_PERIOD_B 2000
#define TIMER_A_ISR tickTasks
#define TIMER_B_ISR updateDisplay

#include "pin_io.h"
#include "timer.h"
#include "usart1284.h"
#include "lcd.h"
#include "task.h"
#include "extruder.h"
#include "fan.h"
#include "ff.h"
#include "fileio.h"

#define LCD_ENABLE PD_2
#define LCD_RS PD_3
#define LCD_DATA0 PD_4
#define LCD_DATA1 PD_5
#define LCD_DATA2 PD_6
#define LCD_DATA3 PD_7
#define LCD_BTTNS PA_1

#define LCD_ENTER_LOW 430
#define LCD_ENTER_HIGH 470
#define LCD_BACK_LOW 60
#define LCD_BACK_HIGH 100
#define LCD_UP_LOW 600
#define LCD_UP_HIGH 640
#define LCD_DOWN_LOW 140
#define LCD_DOWN_HIGH 180
#define LCD_MENU_LOW 260
#define LCD_MENU_HIGH 300

#define FAN_PIN PB_0

#define T_SENSOR PA_0
#define EXTRUDER_PIN PA_5

#define E_CUTOFF 1000
#define KP 13.5
#define KI 0.097
#define KD 24

#define MOTOR_DISABLE PA_7

#define X_MOTOR_DIR PC_0
#define X_MOTOR_STEP PC_1
#define X_MOTOR_INVERT 0

#define Y_MOTOR_DIR PC_2
#define Y_MOTOR_STEP PC_3
#define Y_MOTOR_INVERT 1

#define Z_MOTOR_DIR PC_4
#define Z_MOTOR_STEP PC_5
#define Z_MOTOR_INVERT 0

#define E_MOTOR_DIR PC_6
#define E_MOTOR_STEP PC_7
#define E_MOTOR_INVERT 1

#define X_ENDSTOP PA_2
#define Y_ENDSTOP PA_3
#define Z_ENDSTOP PA_4

#define X_STEPS_MM 100
#define Y_STEPS_MM 100
#define Z_STEPS_MM 400
#define E_STEPS_MM 95

#define X_HOME_OFFSET 36
#define Y_HOME_OFFSET 7
#define Z_HOME_OFFSET 0.8f

#define X_MAX_POS 220
#define Y_MAX_POS 220
#define Z_MAX_POS 240

#endif //DEFS_H_