/*
 * stepper.h
 * Author : Justin Doss
 *
 * This file implements an abstraction layer to allow simple control of forward and reverse motor stepping.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#include "defs.h"

class Stepper
{
    public:
        void init(_axis axis);
        void step(unsigned char reverse = false);

        static Stepper* getStepper(_axis);

    private:
        _axis axis;
        unsigned char index;
        unsigned char reversed;
};

#endif //STEPPER_H_