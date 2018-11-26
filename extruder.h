/*
 * extruder.h
 * Author : Justin Doss
 *
 * This file implements the functionality of the temperature sensor and the dynamic
 * PWM for the extruder.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef EXTRUDER_H_
#define EXTRUDER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "defs.h"

void  Extruder_init();
void  Extruder_setTemp(unsigned char);
void  Extruder_preHeat();
float Extruder_getTemp();
unsigned short Extruder_getDesiredTemp();
void Extruder_checkTemp();
void Extruder_onTickHeater(_task *);
void Extruder_onTickSensor(_task *);

#ifdef __cplusplus
}
#endif

#endif //EXTRUDER_H_