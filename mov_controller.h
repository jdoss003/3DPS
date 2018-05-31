/*
 * mov_controller.h
 *
 * Created: 5/21/2018 3:05:07 PM
 *  Author: Justin
 */
#ifndef MOV_CONTROLLER_H_
#define MOV_CONTROLLER_H_

class MovController
{
    public:
        void init(_axis axis);
        void moveTo(float);
        float getPosition();
        unsigned char isMoving();
        void goHome();
        unsigned char hitEndstop();
        void onTick(_task *);

    private:
        _axis axis;
        _task task;
        float pos;
        signed short steps;
        unsigned char hasEndstop;
};

MovController *getMovController(_axis);

void goHomeX();
void goHomeY();
void goHomeZ();
void goHomeAll();

#endif /* MOV_CONTROLLER_H_ */