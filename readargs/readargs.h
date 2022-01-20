/*****************************************************************************
 ***                                                                       ***
 *** readargs.h                                                            ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef READARGS_H
#define READARGS_H


#define  ARGID_OTHER    -1
#define  ARGID_UNKNOWN  -2
#define  ARGTYPE_NONE    0
#define  ARGTYPE_BOOL    1
#define  ARGTYPE_INT     2
#define  ARGTYPE_FLOAT   3
#define  ARGTYPE_STRING  4



struct rdargs {
    long        argid;
    long        type;
    char        letter;
};

typedef struct arg {
    long        argid;
    long        type;

    long        boolval;
    long        intval;
    double      floatval;
    char       *strval;

    struct arg *next;
} *arglist;

#endif
