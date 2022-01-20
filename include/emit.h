/*****************************************************************************
 ***                                                                       ***
 *** emit.h                                                                ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef EMIT_H
#define EMIT_H

#define ACTION_ADD    1
#define ACTION_SUB    2
#define ACTION_MUL    3
#define ACTION_DIV    4
#define ACTION_MOD    5
#define ACTION_POW    6
#define ACTION_NEG    7
#define ACTION_FAC    8
#define ACTION_CALL   9
#define ACTION_VAR    10
#define ACTION_PUSH   11
#define ACTION_ASSIGN 12
#define ACTION_AND    13
#define ACTION_OR     14
#define ACTION_NOT    15
#define ACTION_CMP_EQ 16
#define ACTION_CMP_NE 17
#define ACTION_CMP_GT 18
#define ACTION_CMP_LT 19
#define ACTION_CMP_GE 20
#define ACTION_CMP_LE 21
#define ACTION_POP    22
#define ACTION_BREAK  23
#define ACTION_RETURN 24
#define ACTION_QUIT   25
#define ACTION_ERR    26

#define ACTION_PRINTSTACK 100
#define ACTION_CLEARSTACK 101

#define MAX_STACK_PTR 128

#endif
