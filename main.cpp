/*
 * main.cpp
 * Author : Justin Doss
 *
 * This file contains the main loop and initialization code for the program.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#include "defs.h"
#include "lcd_menu.h"

void* operator new(size_t objsize)
{
    return malloc(objsize);
}

void operator delete(void* obj)
{
    free(obj);
}

enum system_states { SYS_START, SYS_WAITING, SYS_RUNNING, SYS_FAILURE } system_state;

void waitingLoop()
{
    switch (system_state)
    {
        case SYS_WAITING:
            Extruder::checkTemp();
            LCD_MENU::getCurrent()->updateButtons();
            break;
        case SYS_RUNNING:
            Extruder::checkTemp();
            break;
        default:
            systemFailure("Bad system state");
            break;
    }
	_delay_ms(10);
}

void updateDisplay()
{
    LCD_MENU::getCurrent()->update();
}

unsigned char getSysState()
{
    return system_state;
}

void systemFailure(char *msg)
{
    cli();
    TimerOffA();
    TimerOffB();

    Extruder::setTemp(0);
    SETPIN(EXTRUDER_PIN, LOW);
    system_state = SYS_FAILURE;
    MovController::stopAllMoves();

    LCD::get()->clear();
    LCD::get()->printCenter(msg, 1);

    while (1); // stay here until reset
}

void mainLoop()
{
    switch (system_state)
    {
        case SYS_START:
            system_state = SYS_WAITING;
            LCDMainScreen::makeCurrent();
        case SYS_WAITING:
            waitingLoop();
            break;
        case SYS_RUNNING:
            // TODO
            break;
        default:
            systemFailure("Bad system state");
            break;
    }
}

int main()
{
    LCD::get()->init();
    LCD::get()->printCenter("Starting...", 1);
    LCD::get()->printCenter("Author: J DOSS", 2);

    system_state = SYS_START;
    INITADC();

    initMovScheduler();

    unsigned char i;
    for (i = 0; i < 4; ++i)
    {
        MovController::getMovController((_axis) i)->init((_axis) i);
    }

    Extruder::init();

    TimerSetA(TICK_PERIOD_A);
    TimerOnA();
    TimerSetB(TICK_PERIOD_B);
    TimerOnB();

    while (1) { mainLoop(); }
}
