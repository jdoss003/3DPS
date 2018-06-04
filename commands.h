//
// Created by Justin on 5/31/18.
//

#ifndef COMMANDS_H
#define COMMANDS_H

#include "defs.h"

class GCode;

void initMovScheduler();
void proccess_command(GCode);

#endif //COMMANDS_H
