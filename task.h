/*
 * task.h
 *
 * Created: 5/21/2018 3:25:09 PM
 *  Author: Justin
 */ 


#ifndef TASK_H_
#define TASK_H_

// Given to us to use.
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct task
{
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state;			//Task's current state
	unsigned long period;		//Task period
	unsigned long elapsedTime;  //Time elapsed since last task tick
	char (*TickFct)(char);		//Task tick function
} _task;



#endif /* TASK_H_ */