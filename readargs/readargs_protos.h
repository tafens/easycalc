/*****************************************************************************
 ***                                                                       ***
 *** readargs_protos.h                                                     ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef READARGS_PROTOS_H
#define READARGS_PROTOS_H

#include "readargs.h"

arglist readargs(long argc, char *argv[], long nargs, struct rdargs *args);
void    freeargs(arglist args);

#endif
