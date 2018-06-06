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
        void moveTo(signed short, unsigned short);
        void setPosition(float);
        float getPosition();
        unsigned char isMoving();
        void goHome();
        void stop();
        unsigned char hitEndstop();
        void onTick(_task *);

        static signed short getStepsPerMM(_axis);
        static MovController *getMovController(_axis);

        static void goHomeAll();
        static void stopAllMoves();
        static unsigned char areAnyMotorsMoving();

    private:
        _axis axis;
        _task task;
        float pos;
        signed short steps;
        unsigned char hasEndstop;
};

#endif /* MOV_CONTROLLER_H_ */