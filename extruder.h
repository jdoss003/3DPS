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

namespace Extruder
{
    void  init();
    void  setTemp(unsigned char);
    float getTemp();
    void checkTemp();
    void onTickHeater(_task *);
    void onTickSensor(_task *);
};

#endif /* EXTRUDER_H_ */