/*****************************************************************************
 ***                                                                       ***
 *** symbol.h                                                              ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef SYMBOL_H
#define SYMBOL_H

#include "tokens.h"

#define TYPE_NONE           0
#define TYPE_ROOT           1    /* Root node         */
#define TYPE_VAR            2    /* A variable        */
#define TYPE_MACRO          4    /* A macro           */
#define TYPE_INTERNAL       8    /* Internal flag     */

typedef struct symentry {
    int    type;                /* Var or Func                           */
    int    nparams;             /* If macro, number of parameters        */
    char   id[MAX_IDENT_LEN];   /* Identifier                            */
    char   func[MAX_INPUT_LEN]; /* Body of function (if entry is a func) */
    double value;               /* Value of variable (if entry is a var) */
    struct symentry *next;
} symEntry;

typedef struct symtable {
    struct symentry *first;
    struct symtable *parent;
} symTable;

#endif
