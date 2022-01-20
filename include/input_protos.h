/*****************************************************************************
 ***                                                                       ***
 *** input_protos.h                                                        ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef INPUT_PROTOS_H
#define INPUT_PROTOS_H

#include <stdio.h>

char *ecreadline(char *prompt);
char *getrow(FILE *inf);
void  cleanup_buffer(char *buf);

void ecsetinteractive(long ia);
long ecgetinteractive(void);

void loadhistory(void);
void savehistory(void);

#endif
