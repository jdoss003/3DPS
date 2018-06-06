//
// Created by Justin Doss on 5/25/18.
//
#include "defs.h"

static unsigned char numTasks = 0;
static _task *tasks[MAX_TASKS];

extern void addTask(_task *task)
{
    if (numTasks < MAX_TASKS)
    {
        tasks[numTasks++] = task;
    }
    else
    {
        systemFailure("Task init");
    }
}

extern void tickTasks()
{
	if (getSysState() == 3) return;
    unsigned char i;
    for (i = 0; i < numTasks; i++)
    {
        // Task is ready to tick
        if (tasks[i]->elapsedTime >= tasks[i]->period)
        {
            // Setting next state for task
            tasks[i]->TickFct(tasks[i]);
            // Reset the elapsed time for next tick.
            tasks[i]->elapsedTime = 0;
        }
        tasks[i]->elapsedTime += 1;
    }
}