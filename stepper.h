/*
 * stepper.h
 *
 * Created: 5/19/2018 4:01:26 PM
 *  Author: Justin
 */


#ifndef STEPPER_H_
#define STEPPER_H_

class Stepper
{
    public:
        void init(_axis axis);
        void step(unsigned char reverse = false);

        static Stepper* getStepper(_axis);

    private:
        void setOutput();

        _axis axis;
        unsigned char index;
        unsigned char reversed;
};

#endif /* STEPPER_H_ */