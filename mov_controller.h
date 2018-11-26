/*
 * mov_controller.h
 * Author : Justin Doss
 *
 * This file implements the control of each motor and its movement.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef MOV_CONTROLLER_H_
#define MOV_CONTROLLER_H_

#include "defs.h"
#include "task.h"

class MovController
{
    public:
        void init(_axis axis);
        void moveTo(signed short, unsigned short);
        void setPosition(float);
        float getPosition();
        unsigned char isMoving();
        void goHome();
        void stop();
        unsigned char hitEndstop();
        void onTick(_task *);

        static signed short getStepsPerMM(_axis);
        static signed short getMaxPos(_axis axis);
        static MovController* getMovController(_axis);

        static void goHomeAll();
        static void stopAllMoves();
        static unsigned char areAnyMotorsMoving();
        static void disableSteppers();
        static void enableSteppers();

    private:
        _axis axis;
        _task task;
        float pos;
        signed short steps;
        unsigned char hasEndstop;
};

#endif /* MOV_CONTROLLER_H_ */