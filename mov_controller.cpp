/*
 * mov_controller.cpp
 * Author : Justin Doss
 *
 * This file implements the control of each motor and its movement.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#include "mov_controller.h"
#include "stepper.h"
#include "defs.h"

#define HOME_PERIOD1 1
#define HOME_PERIOD2 3

enum contrl_state { MOTOR_WAITING, MOTOR_MOVING, MOTOR_HOME1, MOTOR_HOME2, MOTOR_HOME3, MOTOR_HOME4 };

MovController x_motor, y_motor, z_motor, extruder;
MovController* motors[4] = {&x_motor, &y_motor, &z_motor, &extruder};

signed short MovController::getStepsPerMM(_axis axis)
{
    switch (axis)
    {
        case X_AXIS:
            return X_STEPS_MM;
        case Y_AXIS:
            return Y_STEPS_MM;
        case Z_AXIS:
            return Z_STEPS_MM;
        case EXTRUDER:
            return E_STEPS_MM;
        default:
            systemFailure("Steps MM");
            return 0;
    }
}

float getHomeOffset(_axis axis)
{
    switch (axis)
    {
        case X_AXIS:
            return X_HOME_OFFSET;
        case Y_AXIS:
            return Y_HOME_OFFSET;
        case Z_AXIS:
            return Z_HOME_OFFSET;
        case EXTRUDER:
        default:
            systemFailure("Home Offset");
            return 0;
    }
}

signed short MovController::getMaxPos(_axis axis)
{
    switch (axis)
    {
        case X_AXIS:
            return X_MAX_POS;
        case Y_AXIS:
            return Y_MAX_POS;
        case Z_AXIS:
            return Z_MAX_POS;
        case EXTRUDER:
        default:
            systemFailure("MAX Pos");
            return 0;
    }
}

void onTickX(_task *task)
{
    MovController::getMovController(X_AXIS)->onTick(task);
}

void onTickY(_task *task)
{
    MovController::getMovController(Y_AXIS)->onTick(task);
}

void onTickZ(_task *task)
{
    MovController::getMovController(Z_AXIS)->onTick(task);
}

void onTickE(_task *task)
{
    MovController::getMovController(EXTRUDER)->onTick(task);
}

MovController* MovController::getMovController(_axis axis)
{
    return motors[axis];
}

void MovController::init(_axis axis)
{
    this->axis = axis;
    this->pos = -237.0;
    this->steps = 0;
    this->hasEndstop = (this->axis != EXTRUDER);
    Stepper::getStepper(this->axis)->init(this->axis);

    this->task.state = MOTOR_WAITING;
    this->task.elapsedTime = 0;
    this->task.period = 1;

    switch (this->axis)
    {
        case X_AXIS:
            this->task.TickFct = &onTickX;
            break;
        case Y_AXIS:
            this->task.TickFct = &onTickY;
            break;
        case Z_AXIS:
            this->task.TickFct = &onTickZ;
            break;
        case EXTRUDER:
            this->task.TickFct = &onTickE;
            break;
        default:
            systemFailure("Init axis");
            break;
    }
    addTask(&this->task);
}

void MovController::moveTo(signed short steps, unsigned short period)
{
    this->task.state = MOTOR_MOVING;
    this->steps = steps;
    this->task.period = period;
    this->task.elapsedTime = period; // start next task tick
}

void MovController::setPosition(float pos)
{
    this->pos = pos;
}

float MovController::getPosition()
{
    return this->pos;
}

unsigned char MovController::isMoving()
{
    return this->task.state != MOTOR_WAITING;
}

void MovController::goHome()
{
    if (this->hasEndstop)
    {
        this->task.state = MOTOR_HOME1;
        this->task.period = HOME_PERIOD1;
//        this->moveTo(-getMaxPos(this->axis) * getStepsPerMM(this->axis) + getHomeOffset(this->axis) + 100, 5);
//        while (this->isMoving()) { waitingLoop(1); }
//        this->setPosition(-(float)getHomeOffset(this->axis) / (float)getStepsPerMM(this->axis));
//        this->moveTo(getHomeOffset(this->axis), 20);
    }
}

void MovController::stop()
{
    this->steps = 0;
    this->task.state = MOTOR_WAITING;
}

unsigned char MovController::hitEndstop()
{
    switch (this->axis)
    {
        case X_AXIS:
            return GETPIN(X_ENDSTOP, 1);
        case Y_AXIS:
            return GETPIN(Y_ENDSTOP, 1);
        case Z_AXIS:
            return GETPIN(Z_ENDSTOP, 1);
        case EXTRUDER:
        default:
            systemFailure("Endstop axis");
            return 0;
    }
}

void MovController::onTick(_task *task)
{
    switch (this->task.state)
    {
        case MOTOR_MOVING:
            if (this->steps == 0)
            {
                this->task.state = MOTOR_WAITING;
            }
            break;
        case MOTOR_WAITING:
            break;
        case MOTOR_HOME1:
            if (this->hitEndstop())
            {
                this->task.state = MOTOR_HOME2;
                this->steps = getStepsPerMM(this->axis) / 10;
            }
            break;
        case MOTOR_HOME2:
            if (this->steps == 0)
            {
                this->task.state = MOTOR_HOME3;
                this->task.period = HOME_PERIOD2;
            }
            break;
        case MOTOR_HOME3:
            if (this->hitEndstop())
            {
                this->task.state = MOTOR_MOVING;
                this->steps = getHomeOffset(this->axis) * getStepsPerMM(this->axis);
				this->pos = -getHomeOffset(this->axis);
                this->task.period = HOME_PERIOD1;
            }
			break;
        default:
            systemFailure("Motor state");
            break;
    }

    switch (this->task.state)
    {
        case MOTOR_MOVING:
            if (this->hasEndstop)
				if (this->steps < 0 && this->hitEndstop())
					{
						char* s = "    ";
						itoa(this->steps, s, 10);
						//systemFailure("Invalid pos1");
						systemFailure(s);
					}
				else if (this->steps > 0 && this->pos > getMaxPos(this->axis))
					systemFailure("Invalid pos2");
            Stepper::getStepper(this->axis)->step(this->steps < 0);
            this->pos += (this->steps < 0 ? -1.0 / (float) getStepsPerMM(this->axis) : 1.0 / (float) getStepsPerMM(this->axis));
            this->steps += (this->steps < 0 ? 1 : -1);
            break;
        case MOTOR_WAITING:
            break;
        case MOTOR_HOME1:
        case MOTOR_HOME3:
            Stepper::getStepper(this->axis)->step(1);
            break;
        case MOTOR_HOME2:
            Stepper::getStepper(this->axis)->step(0);
			if (!this->hitEndstop())
				--this->steps;
            break;
        default:
            systemFailure("Motor State");
            break;
    }
}

void MovController::goHomeAll()
{
    getMovController(X_AXIS)->goHome();
    getMovController(Y_AXIS)->goHome();
    getMovController(Z_AXIS)->goHome();
}

void MovController::stopAllMoves()
{
    unsigned char i;
    for (i = 0; i < 4; ++i)
    {
        getMovController((_axis) i)->stop();
    }
}

unsigned char MovController::areAnyMotorsMoving()
{
    unsigned char ret = 0;
    unsigned char i;
    for (i = 0; i < 4; ++i)
    {
        ret |= getMovController((_axis) i)->isMoving();
    }

    return ret;
}

void MovController::disableSteppers()
{
    SETPIN(MOTOR_DISABLE, HIGH);
}

void MovController::enableSteppers()
{
    SETPIN(MOTOR_DISABLE, LOW);
}
