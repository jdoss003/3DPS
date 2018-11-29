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
#include <string.h>

#include "commands.h"
#include "lcd_menu.h"
#include "mov_controller.h"
#include "fileio.h"

_system_state system_state;
char serialSorce = 0;
volatile unsigned char doDisplayUpdate = 0;

GCode command;
char* nextLine;

void* operator new(size_t objsize)
{
    return malloc(objsize);
}

void operator delete(void* obj)
{
    free(obj);
}

void waitingLoop(unsigned char delay)
{
    switch (system_state)
    {
        case SYS_WAITING:
        case SYS_RUNNING:
			if (doDisplayUpdate)
			{
				LCD_MENU::getCurrent()->update();
				doDisplayUpdate = 0;
			}
            break;
        default:
            systemFailure("Bad system state");
            break;
    }
}

void updateDisplay()
{
    doDisplayUpdate = 1;
}

_system_state getSysState()
{
    return system_state;
}

void setSystemPrinting(unsigned char isSerial)
{
    system_state = SYS_RUNNING;
    serialSorce = isSerial;

    if (!isSerial)
    {
        USART_autoRecieve(0, 0);
        USART_clearBuf(0);
    }

    LCDMainScreen::makeCurrent();
    LCD_MENU::getCurrent()->disableButtons();
}

void systemFailure(char* msg)
{
    cli();
    TimerOffA();
    TimerOffB();

    Extruder_setTemp(0);
    SETPIN(EXTRUDER_PIN, LOW);
    system_state = SYS_FAILURE;
    MovController::stopAllMoves();

    LCD_clear();
    LCD_printCenter(msg, 1);

    while (1); // stay here until reset
}

void mainLoop()
{
	static unsigned char t = 0;
	
    switch (system_state)
    {
        case SYS_START:
            system_state = SYS_WAITING;
            LCDMainScreen::makeCurrent();
			//LCD_MENU::getCurrent()->disableButtons();
			break;
        case SYS_WAITING:
        {
            if (USART_hasLine(0))
            {
				 nextLine = USART_getLine(0);
                
                if (command.parseAscii(nextLine, 1))
                {
					LCDMainScreen::setMessage(nextLine);
					//USART_clearBuf(0);
                    proccess_command(command);
                    if (command.hasN())
                    {
                        setSystemPrinting(1);
                    }
                }
				else
				{
					//USART_clearBuf(0);
				}
                return;
            }
            //waitingLoop(0);
            break;
        }
        case SYS_RUNNING:
        {
            if (serialSorce)
            {
                if (USART_hasLine(0))
                {
					nextLine = USART_getLine(0);
					if (command.parseAscii(nextLine, 1))
					{
						LCDMainScreen::setMessage(nextLine);
						proccess_command(command);
					}
					else
					{
						//USART_clearBuf(0);
						char msg[] = "rs       \n";
						utoa(GCode::getLineNum(), msg + 3, 10);
						LCDMainScreen::setMessage(msg);
						USART_sendLine(msg, 0);
						while (!USART_hasTransmittedLine(0));
					}
                }
                //return;
            }
            else
            {
                nextLine = FIO_fileGetLine();
                
                if (*nextLine && command.parseAscii(nextLine, 0))
                {
					LCDMainScreen::setMessage(nextLine);
					proccess_command(command);
				}
            }
            //waitingLoop(0);
            break;
        }
        default:
            systemFailure("Bad system state");
            break;
    }

    if (doDisplayUpdate)
    {
	    LCD_MENU::getCurrent()->update();
		doDisplayUpdate = 0;
    }
	
	if (++t > 200)
	{
		LCD_MENU::getCurrent()->updateButtons();
		t = 0;
	}
}

int main()
{
	_delay_ms(100);

    LCD_init();
    LCD_printCenter("Starting...", 1);
    LCD_printCenter("Author: J DOSS", 2);

    LCDMenu_init();

    system_state = SYS_START;
    INITADC();

    initMovScheduler();

    unsigned char i;
    for (i = 0; i < 4; ++i)
    {
        MovController::getMovController((_axis) i)->init((_axis) i);
    }

    Extruder_init();
    Fan_init();

    INITPIN(MOTOR_DISABLE, OUTPUT, LOW);

    TimerSetA(TICK_PERIOD_A);
    TimerOnA();
    TimerSetB(TICK_PERIOD_B);
    TimerOnB();

    USART_init(0);
	USART_clearBuf(0);
    USART_autoRecieve(1, 0);
	
	MovController::getMovController(Z_AXIS)->setPosition(50.0);
	MovController::getMovController(EXTRUDER)->setPosition(50.0);

    while (1) { mainLoop(); }
}
