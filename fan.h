/*
 * File: fan.h
 * Author : Justin Doss
 *
 * This file is a driver for a DC fan
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef FAN_H
#define FAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "defs.h"

void Fan_init();
void Fan_setPWM(unsigned char);
void Fan_off();
void Fan_onTickPWM(_task *);

#ifdef __cplusplus
}
#endif

#endif //FAN_H
