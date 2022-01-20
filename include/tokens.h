/*****************************************************************************
 ***                                                                       ***
 *** tokens.h                                                              ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef TOKENS_H
#define TOKENS_H

#define MAX_INPUT_LEN   8192
#define MAX_MACRO_LEN   8192
#define MAX_STRING_LEN  8192
#define MAX_IDENT_LEN   32
#define MAX_TOKNAME_LEN 24

#define TOKEN_NONE     0     /*                     */
#define TOKEN_ADD      1     /*  +                  */
#define TOKEN_SUB      2     /*  -                  */
#define TOKEN_MUL      3     /*  *                  */
#define TOKEN_DIV      4     /*  /                  */
#define TOKEN_MOD      5     /*  %                  */
#define TOKEN_POW      6     /*  ^                  */
#define TOKEN_EXCL     7     /*  !                  */
#define TOKEN_QUESTION 8     /*  ?                  */
#define TOKEN_LPAR     9     /*  (                  */
#define TOKEN_RPAR     10    /*  )                  */
#define TOKEN_COMMA    11    /*  ,                  */
#define TOKEN_SEMI     12    /*  ;                  */
#define TOKEN_COLON    13    /*  :                  */
#define TOKEN_ASSIGN   14    /*  =                  */
#define TOKEN_AND      15    /*  and                */
#define TOKEN_ANDAND   16    /*  &&                 */
#define TOKEN_OR       17    /*  or                 */
#define TOKEN_OROR     18    /*  ||                 */
#define TOKEN_NOT      19    /*  not                */
#define TOKEN_CMP_EQ   20    /*  ==                 */
#define TOKEN_CMP_NE   21    /*  !=                 */
#define TOKEN_CMP_GT   22    /*  >                  */
#define TOKEN_CMP_LT   23    /*  <                  */
#define TOKEN_CMP_GE   24    /*  >=                 */
#define TOKEN_CMP_LE   25    /*  <=                 */
#define TOKEN_NUM      26    /* num                 */
#define TOKEN_ID       27    /* id                  */
#define TOKEN_STR      28    /* "string"            */
#define TOKEN_IF       29    /* if                  */
#define TOKEN_COND     30    /* cond                */
#define TOKEN_WHILE    31    /* while               */
#define TOKEN_INPUT    32    /* input               */
#define TOKEN_RETURN   33    /* return              */
#define TOKEN_BREAK    34    /* break               */
#define TOKEN_QUIT     35    /* quit                */
#define TOKEN_TRUE     36    /* true                */
#define TOKEN_FALSE    37    /* false               */
#define TOKEN_INFINITY 38    /* infinity            */
#define TOKEN_MDEF     39    /* {macrodef}          */
#define TOKEN_END      40    /* # end of expression */
#define TOKEN_ERR      -1    /* error               */



typedef struct {
    int     token;
    double  value;
    char    id[MAX_IDENT_LEN];
    char    macrodef[MAX_MACRO_LEN];
    char    string[MAX_STRING_LEN];
} currentToken;

typedef struct {
    int           i,len;                      /* i=position in stream, len=length  */
    char          name[MAX_IDENT_LEN];        /* Name of this stream               */
    char          filename[MAX_IDENT_LEN];    /* Name of file this stream is from  */
    char          macroname[MAX_IDENT_LEN];   /* Name of macro this dtream is from */
    char         *buffer;                     /* The stream                        */
    currentToken *cT;                         /* Current Token                     */
    int           tstrt,tstop;                /* Token start pos, token end pos    */
} inputStream;

#endif
