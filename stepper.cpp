/*
 * stepper.cpp
 *
 * Created: 5/19/2018 4:00:09 PM
 *  Author: Justin
 */
#include "defs.h"

const _io_hl output1[] = { LOW, HIGH, HIGH, LOW };
const _io_hl output2[] = { HIGH, HIGH, LOW, LOW };

Stepper x_stepper, y_stepper, z_stepper, extruder_stepper;
Stepper* steppers[4] = { &x_stepper, &y_stepper, &z_stepper, &extruder_stepper };

Stepper* Stepper::getStepper(_axis axis)
{
    return steppers[axis];
}

void Stepper::init(_axis axis)
{
    this->axis = axis;
    this->index = 0;

    switch (axis)
    {
        case X_AXIS:
            this->reversed = X_MOTOR_INVERT;
            INITPIN(X_MOTOR_P1, OUTPUT, LOW);
            INITPIN(X_MOTOR_P2, OUTPUT, LOW);
            INITPIN(X_ENDSTOP, INPUT, HIGH);
            break;
        case Y_AXIS:
            this->reversed = Y_MOTOR_INVERT;
            INITPIN(Y_MOTOR_P1, OUTPUT, LOW);
            INITPIN(Y_MOTOR_P2, OUTPUT, LOW);
            INITPIN(Y_ENDSTOP, INPUT, HIGH);
            break;
        case Z_AXIS:
            this->reversed = Z_MOTOR_INVERT;
            INITPIN(Z_MOTOR_P1, OUTPUT, LOW);
            INITPIN(Z_MOTOR_P2, OUTPUT, LOW);
            INITPIN(Z_ENDSTOP, INPUT, HIGH);
            break;
        case EXTRUDER:
            this->reversed = E_MOTOR_INVERT;
            INITPIN(E_MOTOR_P1, OUTPUT, LOW);
            INITPIN(E_MOTOR_P2, OUTPUT, LOW);
            break;
        default:
			systemFailure("Init Stepper");
            break;
    }

    setOutput();
}

void Stepper::step(unsigned char reverse)
{
    if (this->reversed == reverse)
    {
        ++this->index;
        this->index = this->index % 4;
    }
    else
    {
        this->index = (this->index == 0 ? 3 : this->index - 1);
    }

    setOutput();
}

void Stepper::setOutput()
{
    switch (this->axis)
    {
        case X_AXIS:
            SETPIN(X_MOTOR_P1, output1[this->index]);
            SETPIN(X_MOTOR_P2, output2[this->index]);
            break;
        case Y_AXIS:
            SETPIN(Y_MOTOR_P1, output1[this->index]);
            SETPIN(Y_MOTOR_P2, output2[this->index]);
            break;
        case Z_AXIS:
            SETPIN(Z_MOTOR_P1, output1[this->index]);
            SETPIN(Z_MOTOR_P2, output2[this->index]);
            break;
        case EXTRUDER:
            SETPIN(E_MOTOR_P1, output1[this->index]);
            SETPIN(E_MOTOR_P2, output2[this->index]);
            break;
        default:
			systemFailure("Stepper Out");
            break;
    }
}