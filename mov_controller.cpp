/*
 * mov_controller.cpp
 *
 * Created: 5/21/2018 3:05:26 PM
 *  Author: Justin
 */ 
#include "defs.h"

MovController x_motor, y_motor, z_motor, extruder;
MovController *motors[4] = { &x_motor, &y_motor, &z_motor, &extruder };

MovController* getMovController(_axis axis)
{
	return motors[axis];
}

void MovController::init(_axis axis)
{
	this->axis = axis;
	this->state = MOTOR_WAITING;
	this->pos = -237.0;
	this->steps = 0;
	this->hasEndstop = (this->axis != EXTRUDER);
	getStepper(this->axis)->init(this->axis);
}

void MovController::moveTo(float pos)
{
	
}

float MovController::getPosition()
{
	return this->pos;
}

unsigned char MovController::isMoving()
{
	return this->state == MOTOR_WAITING;
}

void MovController::goHome()
{
	this->state = MOTOR_GOING_HOME;
}

void MovController::hitEndstop()
{
	
}

void MovController::onTick(_task*)
{
	
}