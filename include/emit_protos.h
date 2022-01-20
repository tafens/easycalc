/*****************************************************************************
 ***                                                                       ***
 *** emit_protos.h                                                         ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef EMIT_PROTOS_H
#define EMIT_PROTOS_H

#include "symbol.h"
#include "tokens.h"

void emit(long action,void *value,inputStream *iS,symTable *sT);

#endif
