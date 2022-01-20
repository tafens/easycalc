/*****************************************************************************
 ***                                                                       ***
 *** command_protos.h                                                      ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef COMMAND_PROTOS_H
#define COMMAND_PROTOS_H

#include "symbol.h"

long interactive_command(char *cmd,symTable *sT, double *res, long *running);

#endif
