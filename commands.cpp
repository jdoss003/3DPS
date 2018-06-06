//
// Created by Justin on 5/31/18.
//

#include "defs.h"

_task mov_scheduler;

enum ScheduleState { SCHED_WAIT, SCHED_READY, SCHED_DONE};

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
    switch (cmd.G)
    {
        case 0: // move command
        case 1:
            while (mov_scheduler.state == SCHED_READY || MovController::areAnyMotorsMoving()) { keepAlive(); }

            mov.x_steps = 0;
            mov.y_steps = 0;
            mov.z_steps = 0;
            mov.e_steps = 0;

            if (cmd.hasX())
                mov.x_steps = ((cmd.X - MovController::getMovController(X_AXIS)->getPosition()) * MovController::getStepsPerMM(X_AXIS));
            if (cmd.hasY())
                mov.y_steps = ((cmd.Y - MovController::getMovController(Y_AXIS)->getPosition()) * MovController::getStepsPerMM(Y_AXIS));
            if (cmd.hasZ())
                mov.z_steps = ((cmd.Z - MovController::getMovController(Z_AXIS)->getPosition()) * MovController::getStepsPerMM(Z_AXIS));
            if (cmd.hasE())
                mov.e_steps = ((cmd.E - MovController::getMovController(EXTRUDER)->getPosition()) * MovController::getStepsPerMM(EXTRUDER));

            if (mov.e_steps >= mov.x_steps && mov.e_steps >= mov.y_steps && mov.e_steps >= mov.z_steps)
            {
                mov.e_peroid = 1;
                if (mov.x_steps != 0)
                    mov.x_peroid = abs(mov.e_steps / mov.x_steps);
                if (mov.y_steps != 0)
                    mov.y_peroid = abs(mov.e_steps / mov.y_steps);
                if (mov.z_steps != 0)
                    mov.z_peroid = abs(mov.e_steps / mov.z_steps);
            }
            else if (mov.x_steps >= mov.y_steps && mov.x_steps > mov.z_steps)
            {
                mov.x_peroid = 1;
                if (mov.e_steps != 0)
                    mov.e_peroid = abs(mov.x_steps / mov.e_steps);
                if (mov.y_steps != 0)
                    mov.y_peroid = abs(mov.x_steps / mov.y_steps);
                if (mov.z_steps != 0)
                    mov.z_peroid = abs(mov.x_steps / mov.z_steps);
            }
            else if (mov.y_steps >= mov.z_steps)
            {
                mov.y_peroid = 1;
                if (mov.x_steps != 0)
                    mov.x_peroid = abs(mov.y_steps / mov.x_steps);
                if (mov.e_steps != 0)
                    mov.e_peroid = abs(mov.y_steps / mov.e_steps);
                if (mov.z_steps != 0)
                    mov.z_peroid = abs(mov.y_steps / mov.z_steps);
            }
            else
            {
                mov.z_peroid = 1;
                if (mov.x_steps != 0)
                    mov.x_peroid = abs(mov.z_steps / mov.x_steps);
                if (mov.y_steps != 0)
                    mov.y_peroid = abs(mov.z_steps / mov.y_steps);
                if (mov.e_steps != 0)
                    mov.e_peroid = abs(mov.z_steps / mov.e_steps);
            }

            mov_scheduler.state = SCHED_READY;
            break;

        case 28: // go home command
            if (!cmd.hasX() && !cmd.hasY() && !cmd.hasZ())
                MovController::goHomeAll();
            if (cmd.hasX())
                MovController::getMovController(X_AXIS)->goHome();
            if (cmd.hasY())
                MovController::getMovController(Y_AXIS)->goHome();
            if (cmd.hasZ())
                MovController::getMovController(Z_AXIS)->goHome();
            break;

        case 92: // set position; no movement takes place

            if (cmd.hasX())
                MovController::getMovController(X_AXIS)->setPosition(cmd.X);
            if (cmd.hasY())
                MovController::getMovController(Y_AXIS)->setPosition(cmd.Y);
            if (cmd.hasZ())
                MovController::getMovController(Z_AXIS)->setPosition(cmd.Z);
            if (cmd.hasE())
                MovController::getMovController(EXTRUDER)->setPosition(cmd.E);
            break;

        default:
            invalidCommandFailure();
            break;
    }
}

void do_m_command(GCode cmd)
{
	static unsigned char i;
    switch (cmd.M)
    {
        case 84: // stop idle hold
            // TODO command
            break;

        case 104: // set extruder temp
            if (!cmd.hasS())
                invalidCommandFailure();
            Extruder::setTemp((unsigned char)cmd.S);
            break;
        case 106: // set fan speed S[255:0]
            // TODO command
            break;
        case 107: // turn fan off (M106 S0)
            // TODO command
            break;
        case 109: // set extruder temp and wait
            if (!cmd.hasS() || cmd.S > 250)
                invalidCommandFailure();
            Extruder::setTemp(cmd.S);
			i = 0;
            while (Extruder::getTemp() + 5.0 < cmd.S || ++i < 200) { keepAlive(); }
            break;

        case 140: // set heat bed temp (not use here but caught to avoid unknown cmd failure)
        case 190: // wait for heat bed to reach temp (not use here but caught to avoid unknown cmd failure)
            break;

        default:
            invalidCommandFailure();
            break;
    }
}

void proccess_command(GCode cmd)
{
    if (cmd.hasFormatError())
        invalidCommandFailure();
	Extruder::checkTemp();

    if (cmd.hasG())
        do_g_command(cmd);
    else if (cmd.hasM())
        do_m_command(cmd);
    else 
       invalidCommandFailure();
}