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

_system_state system_state;
volatile char serialSorce = 0;

void* operator new(size_t objsize)
{
    return malloc(objsize);
}

void operator delete(void* obj)
{
    free(obj);
}

char* EMPTY_STR()
{
	char* r = (char*)malloc(sizeof(char));
	*r = 0;
	return r;
}

void waitingLoop(unsigned char delay)
{
    switch (system_state)
    {
        case SYS_WAITING:
//            Extruder_checkTemp();
            LCD_MENU::getCurrent()->updateButtons();
            break;
        case SYS_RUNNING:
//            Extruder_checkTemp();
            break;
        default:
            systemFailure("Bad system state");
            break;
    }
    //if (delay)
		_delay_ms(1);
}

void updateDisplay()
{
	//LCD_MENU::getCurrent()->updateButtons();
	//_delay_us(10);
    LCD_MENU::getCurrent()->update();
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
				char* serLine = USART_getLine(0);
                GCode command;
                if (command.parseAscii(serLine, 1))
                {
					LCDMainScreen::setMessage(serLine);
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
            waitingLoop(0);
            break;
        }
        case SYS_RUNNING:
        {
            GCode command;
            if (serialSorce)
            {
                if (USART_hasLine(0))
                {
					char* serLine = USART_getLine(0);
					if (command.parseAscii(serLine, 1))
					{
						LCDMainScreen::setMessage(serLine);
						//USART_clearBuf(0);
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
                return;
            }
            else
            {
                char* l = FIO_fileGetLine();
                
                if (command.parseAscii(l, 0))
                {
					LCDMainScreen::setMessage(l);
					proccess_command(command);
				}
            }
            waitingLoop(0);
            break;
        }
        default:
            systemFailure("Bad system state");
            break;
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
	
	MovController::getMovController(X_AXIS)->setPosition(50.0);

    while (1) { mainLoop(); }
}
