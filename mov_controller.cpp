/*
 * mov_controller.cpp
 *
 * Created: 5/21/2018 3:05:26 PM
 *  Author: Justin
 */
#include "defs.h"

static MovController x_motor, y_motor, z_motor, extruder;
static MovController *motors[4] = {&x_motor, &y_motor, &z_motor, &extruder};

signed short getStepsPerMM(_axis axis)
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
            return 0;
    }
}

signed short getHomeOffset(_axis axis)
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
            return 0;
    }
}

void onTickX(_task *task)
{
    getMovController(X_AXIS)->onTick(task);
}

void onTickY(_task *task)
{
    getMovController(Y_AXIS)->onTick(task);
}

void onTickZ(_task *task)
{
    getMovController(Z_AXIS)->onTick(task);
}

void onTickE(_task *task)
{
    getMovController(EXTRUDER)->onTick(task);
}

MovController *getMovController(_axis axis)
{
    return motors[axis];
}

void goHomeAll()
{
    SETPIN(PB_0, HIGH); // TODO remove
    getMovController(X_AXIS)->goHome();
    while (getMovController(X_AXIS)->isMoving()) { SETPIN(PB_0, LOW); }

    getMovController(Y_AXIS)->goHome();
    while (getMovController(Y_AXIS)->isMoving()) { SETPIN(PB_0, LOW); }

    getMovController(Z_AXIS)->goHome();
    while (getMovController(Z_AXIS)->isMoving()) { SETPIN(PB_0, LOW); }
    SETPIN(PB_0, HIGH);
}

void MovController::init(_axis axis)
{
    this->axis = axis;
    this->pos = -237.0;
    this->steps = 0;
    this->hasEndstop = (this->axis != EXTRUDER);
    getStepper(this->axis)->init(this->axis);

    this->task.state = MOTOR_WAITING;
    this->task.elapsedTime = 0;
    this->task.period = 1;                    // TODO

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
            break;
    }
    addTask(&this->task);
}

void MovController::moveTo(float pos)
{
    this->task.state = MOTOR_MOVING;
    this->steps = (signed short) ((pos - this->pos) * getStepsPerMM(this->axis));
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
        this->task.state = MOTOR_GOING_HOME;
    }
    // TODO retract filament?
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
            return 0;
    }
}

void MovController::onTick(_task *task)
{
    static unsigned char wait_ticks;
    switch (this->task.state)
    {
        case MOTOR_MOVING:
            this->pos += (this->steps < 0 ? -1.0 / (float) getStepsPerMM(this->axis) : 1.0 / (float) getStepsPerMM(
                    this->axis));
            this->steps += (this->steps < 0 ? 1 : -1);
            if (this->steps == 0)
            {
                this->task.state = MOTOR_WAITING;
            }
            break;
        case MOTOR_GOING_HOME:
            this->task.period = 1;
            if (this->hitEndstop())
            {
                wait_ticks = 10;
                this->task.state = MOTOR_AT_HOME;
                this->task.period = 1;
            }
            break;
        case MOTOR_AT_HOME:
            if (wait_ticks > 0)
            { --wait_ticks; }
            else if (!this->hitEndstop())
            {
                this->task.state = MOTOR_MOVING;
                this->steps = getHomeOffset(this->axis);
                this->task.period = 1;
                this->pos = (float) this->steps / (float) getStepsPerMM(this->axis);
            }
            break;
        case MOTOR_WAITING:
        default:
            break;
    }

    switch (this->task.state)
    {
        case MOTOR_MOVING:
            getStepper(this->axis)->step(this->steps < 0);
            break;
        case MOTOR_GOING_HOME:
            getStepper(this->axis)->step(1);
            break;
        case MOTOR_AT_HOME:
            if (wait_ticks == 0)
            {
                getStepper(this->axis)->step(0);
            }
            break;
        case MOTOR_WAITING:
        default:
            break;
    }
}