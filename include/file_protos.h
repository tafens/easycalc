/*****************************************************************************
 ***                                                                       ***
 *** file_protos.h                                                         ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef FILE_PROTOS_H
#define FILE_PROTOS_H

#include <stdio.h>

void   read_rc_file(symTable *sT);
double read_file(char *fname,symTable *sT);
double parse_file(FILE *inf,char *fname,symTable *sT);

#endif
