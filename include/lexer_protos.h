/*****************************************************************************
 ***                                                                       ***
 *** lexer_protos.h                                                        ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef LEXER_PROTOS_H
#define LEXER_PROTOS_H

#include "tokens.h"

inputStream *allocStream(char *name,char *filename,char *macroname,char *buf);
void         freeStream(inputStream *iS);

int  getNextToken(inputStream *iS);
void tokenToStr(long token, char *buf,int buflen);

#endif
