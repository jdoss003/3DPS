/*
 * File: commands.cpp
 * Author : Justin Doss
 *
 * This file processes a GCODE object and implements the functionality for each command.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#include "defs.h"
#include "commands.h"
#include "mov_controller.h"
#include "lcd_menu.h"
#include "string.h"

_task mov_scheduler;

enum ScheduleState { SCHED_WAIT, SCHED_READY, SCHED_DONE};

volatile unsigned char relPositioning = 0;

struct ScheduledMov
{
    signed short x_steps;
    unsigned short x_peroid;
    signed short y_steps;
    unsigned short y_peroid;
    signed short z_steps;
    unsigned short z_peroid;
    signed short e_steps;
    unsigned short e_peroid;
} mov;

void scheduleMove(_task *task)
{
    if (!MovController::areAnyMotorsMoving() && task->state == SCHED_READY)
    {
        if (mov.x_steps != 0)
            MovController::getMovController(X_AXIS)->moveTo(mov.x_steps, mov.x_peroid);
        if (mov.y_steps != 0)
            MovController::getMovController(Y_AXIS)->moveTo(mov.y_steps, mov.y_peroid);
        if (mov.z_steps != 0)
            MovController::getMovController(Z_AXIS)->moveTo(mov.z_steps, mov.z_peroid);
        if (mov.e_steps != 0)
            MovController::getMovController(EXTRUDER)->moveTo(mov.e_steps, mov.e_peroid);
        task->state = SCHED_DONE;
    }
	else if (task->state == SCHED_READY)
	{
		LCDMainScreen::setMessage("Sched Ready");
	}
}

void initMovScheduler()
{
    mov_scheduler.state = SCHED_WAIT;
    mov_scheduler.period = 1;
    mov_scheduler.elapsedTime = 0;
    mov_scheduler.TickFct = &scheduleMove;
    addTask(&mov_scheduler);
}

void invalidCommandFailure()
{
    systemFailure("Invalid command...");
}

void do_g_command(GCode cmd)
{
    switch (cmd.getG())
    {
        case 0: // move command
        case 1:
            while (mov_scheduler.state == SCHED_READY || MovController::areAnyMotorsMoving()) { waitingLoop(1); }

            mov.x_steps = 0;
            mov.y_steps = 0;
            mov.z_steps = 0;
            mov.e_steps = 0;

            if (!relPositioning)
            {
                if (cmd.hasX())
                    mov.x_steps = ((cmd.getX() - MovController::getMovController(X_AXIS)->getPosition()) * MovController::getStepsPerMM(X_AXIS));
                if (cmd.hasY())
                    mov.y_steps = ((cmd.getY() - MovController::getMovController(Y_AXIS)->getPosition()) * MovController::getStepsPerMM(Y_AXIS));
                if (cmd.hasZ())
                    mov.z_steps = ((cmd.getZ() - MovController::getMovController(Z_AXIS)->getPosition()) * MovController::getStepsPerMM(Z_AXIS));
                if (cmd.hasE())
                    mov.e_steps = ((cmd.getE() - MovController::getMovController(EXTRUDER)->getPosition()) * MovController::getStepsPerMM(EXTRUDER));
            }
            else
            {
                if (cmd.hasX())
                    mov.x_steps = (cmd.getX() * MovController::getStepsPerMM(X_AXIS));
                if (cmd.hasY())
                    mov.y_steps = (cmd.getY() * MovController::getStepsPerMM(Y_AXIS));
                if (cmd.hasZ())
                    mov.z_steps = (cmd.getZ() * MovController::getStepsPerMM(Z_AXIS));
                if (cmd.hasE())
                    mov.e_steps = (cmd.getE() * MovController::getStepsPerMM(EXTRUDER));
            }

            if (mov.e_steps >= mov.x_steps && mov.e_steps >= mov.y_steps && mov.e_steps >= mov.z_steps)
            {
                mov.e_peroid = 1;
                if (mov.x_steps != 0)
                    mov.x_peroid = mov.e_peroid * abs(mov.e_steps / mov.x_steps);
                if (mov.y_steps != 0)
                    mov.y_peroid = mov.e_peroid * abs(mov.e_steps / mov.y_steps);
                if (mov.z_steps != 0)
                    mov.z_peroid = mov.e_peroid * abs(mov.e_steps / mov.z_steps);
            }
            else if (mov.x_steps >= mov.y_steps && mov.x_steps > mov.z_steps)
            {
                mov.x_peroid = 1;
                if (mov.e_steps != 0)
                    mov.e_peroid = mov.x_peroid * abs(mov.x_steps / mov.e_steps);
                if (mov.y_steps != 0)
                    mov.y_peroid = mov.x_peroid * abs(mov.x_steps / mov.y_steps);
                if (mov.z_steps != 0)
                    mov.z_peroid = mov.x_peroid * abs(mov.x_steps / mov.z_steps);
            }
            else if (mov.y_steps >= mov.z_steps)
            {
                mov.y_peroid = 1;
                if (mov.x_steps != 0)
                    mov.x_peroid = mov.y_peroid * abs(mov.y_steps / mov.x_steps);
                if (mov.e_steps != 0)
                    mov.e_peroid = mov.y_peroid * abs(mov.y_steps / mov.e_steps);
                if (mov.z_steps != 0)
                    mov.z_peroid = mov.y_peroid * abs(mov.y_steps / mov.z_steps);
            }
            else
            {
                mov.z_peroid = 1;
                if (mov.x_steps != 0)
                    mov.x_peroid = mov.z_peroid * abs(mov.z_steps / mov.x_steps);
                if (mov.y_steps != 0)
                    mov.y_peroid = mov.z_peroid * abs(mov.z_steps / mov.y_steps);
                if (mov.e_steps != 0)
                    mov.e_peroid = mov.z_peroid * abs(mov.z_steps / mov.e_steps);
            }

            mov_scheduler.state = SCHED_READY;
            break;

        case 28: // go home command
            while (MovController::areAnyMotorsMoving()) { waitingLoop(1); }
            if (!cmd.hasX() && !cmd.hasY() && !cmd.hasZ())
                MovController::goHomeAll();
            if (cmd.hasX())
                MovController::getMovController(X_AXIS)->goHome();
            if (cmd.hasY())
                MovController::getMovController(Y_AXIS)->goHome();
            if (cmd.hasZ())
                MovController::getMovController(Z_AXIS)->goHome();
            break;
        case 90: // set abs positioning; no movement takes place
            relPositioning = 0;
            break;
        case 91: // set rel positioning; no movement takes place
            relPositioning = 1;
            break;
        case 92: // set position; no movement takes place
            while (MovController::areAnyMotorsMoving()) { waitingLoop(1); }
            if (cmd.hasX())
                MovController::getMovController(X_AXIS)->setPosition(cmd.getX());
            if (cmd.hasY())
                MovController::getMovController(Y_AXIS)->setPosition(cmd.getY());
            if (cmd.hasZ())
                MovController::getMovController(Z_AXIS)->setPosition(cmd.getZ());
            if (cmd.hasE())
                MovController::getMovController(EXTRUDER)->setPosition(cmd.getE());
            break;

        default:
            //invalidCommandFailure();
            break;
    }
	
	if (cmd.fromSerial())
	{
		USART_sendLine("ok\n", 0);
		while (!USART_hasTransmittedLine(0));
	}
}

void do_m_command(GCode cmd)
{
    switch (cmd.getM())
    {
        case 84: // stop idle hold
            // TODO command
            break;

        case 104: // set extruder temp
            if (!cmd.hasS())
                invalidCommandFailure();
            Extruder_setTemp((unsigned char)cmd.getS(0));
            break;
		case 105:
		{
			if (cmd.fromSerial())
			{
				USART_sendLine("ok T:", 0);
				char rsp[4];
				dtostrf(Extruder_getTemp(), 3, 0, &rsp[0]);
				unsigned char i = 0;
				while (rsp[i] == ' ')
					++i;
				while (!USART_hasTransmittedLine(0));
				USART_sendLine(&rsp[i], 0);
				char* des = "/    ";
				utoa(Extruder_getDesiredTemp(), des + 1, 10);
				while (!USART_hasTransmittedLine(0));
				USART_sendLine(des, 0);
				while (!USART_hasTransmittedLine(0));
				USART_sendLine("\n", 0);
				
				return;
			}
			break;
		}
        case 106: // set fan speed S[255:0]
        {
            int32_t s = cmd.getS(0);
            if (!cmd.hasS() || s > 255)
                invalidCommandFailure();
            Fan_setPWM((uint8_t)s);
            break;
        }
        case 107: // turn fan off (M106 S0)
            Fan_off();
            break;
        case 109: // set extruder temp and wait
        {
            int32_t s = cmd.getS(0);
            if (!cmd.hasS() || s > 250)
                invalidCommandFailure();
            Extruder_setTemp(s);
            while (Extruder_getTemp() + 5.0 < s) { waitingLoop(1); }
            break;
        }
		case 115:
		{
			if (cmd.fromSerial())
			{
				char* rsp = "ok FIRMWARE_NAME:avr3DPS MACHINE_TYPE:Anet EXTRUDER_COUNT:1 \n";
				USART_sendLine(rsp, 0);
				while (!USART_hasTransmittedLine(0));
				LCDMainScreen::setMessage(rsp);
				return;
			}
			break;
		}
        case 117: // set a message on the LCD
            LCDMainScreen::setMessage(cmd.getString());
            break;
        case 140: // set heat bed temp (not use here but caught to avoid unknown cmd failure)
        case 190: // wait for heat bed to reach temp (not use here but caught to avoid unknown cmd failure)
            break;

        default:
            //invalidCommandFailure();
            break;
    }

	if (cmd.fromSerial())
	{
		USART_sendLine("ok\n", 0);
		while (!USART_hasTransmittedLine(0));
	}
}

void proccess_command(GCode cmd)
{
    if (cmd.hasFormatError())
        return;//invalidCommandFailure();

    if (cmd.hasG())
        do_g_command(cmd);
    else if (cmd.hasM())
        do_m_command(cmd);
//     else 
//        invalidCommandFailure();
}