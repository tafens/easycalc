/*****************************************************************************
 ***                                                                       ***
 *** message_protos.h                                                      ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef MESSAGE_PROTOS_H
#define MESSAGE_PROTOS_H

#include "tokens.h"

void ecparseerror(inputStream *iS,char *format,...);
void ecgeneralerror(char *format, ...);
void ecwarning(char *format, ...);
void ecmessage(char *format, ...);
void ecforcedmessage(char *format, ...);  /* Skriver ut oberoende av interaktivläge */
void ecverbose(char *format, ...);
void ecverboseplus(char *format, ...);
void ecprintstring(char *str);
void ecprintresult(double res);
void ecprompt(char *str);

/*
void setverbose(long val);
void setquiet(long val);
long isverbose(void);
long isquiet(void);
*/

void setverbosity(long val);
long getverbosity(void);

void setresultmode(long val);
long getresultmode(void);
long getresultmodefromstring(char *modestr);

#endif

