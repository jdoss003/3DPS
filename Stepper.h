/*
 * Stepper.h
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
		void step(bool reverse = false);
	
	private:
		void setOutput();
		_axis axis;
		unsigned char index;
		unsigned char reversed;
};

Stepper* getStepper(_axis);

#endif /* STEPPER_H_ */