/*****************************************************************************
 ***                                                                       ***
 *** macro_protos.h                                                        ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef MACRO_PROTOS_H
#define MACRO_PROTOS_H

#include "global.h"
#include "symbol.h"

void  initmacros(symTable *sT);
long  defmacro(char *id, char *macrodef, inputStream *iS,symTable *sT);
long  domacro(char *id,inputStream *iS,symTable *sT,long argc,double *argv, double *result);

#endif
