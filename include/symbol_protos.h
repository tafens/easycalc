/*****************************************************************************
 ***                                                                       ***
 *** symbol_protos.h                                                       ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef SYMBOL_PROTOS_H
#define SYMBOL_PROTOS_H

#include "symbol.h"

symTable *initsym(symTable *parent);
void  deletesym(symTable *symtab);
void  addsymbol(symTable *symtab,long type,char *id,
                                        double value,char *func,long nparams);
long  setsymbol(symTable *symtab,long type,char *symbol,
                                        double value,char *func,long nparams);
long  lookupvar(symTable *symtab,char *id,double *varval,int searchparents);
char *lookupmacro(symTable *symtab,char *id,long *type,long *nparams,int searchparents);
void  deletevar(symTable *symtab, char *id);
void  deletemacro(symTable *symtab, char *id);
void  printvars(symTable *symtab);
void  printmacros(symTable *symtab);
void  printconfigvars(symTable *symtab);

#endif
