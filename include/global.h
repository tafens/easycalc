/*****************************************************************************
 ***                                                                       ***
 *** global.h  -  Contains all global definitions                          ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

#include "../memaudit/memaudit.h"
#include "tokens.h"
//#include "symbol.h"

/*
** Version/author info
*/
#define VERSION   "v0.1a"
#define BUILDDATE __DATE__
#define AUTHOR    "Stefan Haglund <stefan.haglund@me.com>"
#define ECVERSION "EC " VERSION " by " AUTHOR ";\n" \
                  "Released under GPL in 2022; " "Built " BUILDDATE ".\n"

#define ECPROMPT  "ec> "

#define TRUE  1
#define FALSE 0

#define BUF_LEN 128    /* General buffer length   NOT USED!!!! */

#define MAX(a,b) ((a)>(b) ? (a) : (b))

extern long err;
extern inputStream *currIS;

#endif
