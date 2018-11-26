/*
 * File: commands.h
 * Author : Justin Doss
 *
 * This file processes a GCODE object and implements the functionality for each command.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#include "gcode.h"

void initMovScheduler();
void proccess_command(GCode);

#endif //COMMANDS_H
