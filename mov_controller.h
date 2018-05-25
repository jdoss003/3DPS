/*
 * mov_controller.h
 *
 * Created: 5/21/2018 3:05:07 PM
 *  Author: Justin
 */ 
#ifndef MOV_CONTROLLER_H_
#define MOV_CONTROLLER_H_

typedef enum contrl_state { MOTOR_WAITING, MOTOR_MOVING, MOTOR_GOING_HOME } _contrl_state;

class MovController
{
	public:
		void init(_axis axis);
		void moveTo(float);
		float getPosition();

		
		unsigned char isMoving();
		void goHome();
		void hitEndstop();
		
		void onTick(_task*);

	
	private:
		_axis axis;
		_contrl_state state;
		float pos;
		signed long steps;
		unsigned char hasEndstop;
};

MovController* getMovController(_axis);

void goHomeAll();

#endif /* MOV_CONTROLLER_H_ */