/*
 * task.c
 * Author : Justin Doss
 *
 * This file is responsible for running each task element on each SM tick.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#include "defs.h"

static unsigned char numTasks = 0;
static _task *tasks[MAX_TASKS];

void addTask(_task *task)
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

void tickTasks()
{
    if (getSysState() == 3) { return; }
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