/*****************************************************************************
 ***                                                                       ***
 *** global.c  -  Contains all global definitions and functions            ***
 ***                                                                       ***
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "include/symbol.h"
#include "include/tokens.h"
#include "include/global.h"

/*
** if err is TRUE, a parse error has occured, and all parse and stack
** operations are suspended.
*/
long err=FALSE;

/*
** currIS is set to the inputStream thet parse() is currently parsing,
** or NULL otherwise.
*/
inputStream *currIS=NULL;
