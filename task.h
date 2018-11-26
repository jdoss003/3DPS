/*
 * task.h
 * Author : Justin Doss
 *
 * This file is responsible for running each task element on each SM tick.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef TASK_H_
#define TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TASKS 10

// Given to us to use.
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct task _task;

struct task
{
    /*Tasks should have members that include: state, period,
        a measurement of elapsed time, and a function pointer.*/
    signed char state;              // Task's current state
    unsigned long period;           // Task period
    unsigned long elapsedTime;      // Time elapsed since last task tick
    void (*TickFct)(_task *);       // Task tick function
};

void addTask(_task *task);
void tickTasks();

#ifdef __cplusplus
}
#endif

#endif // TASK_H_
