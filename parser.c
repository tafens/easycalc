/*****************************************************************************
 ***                                                                       ***
 *** parser.c  -  The parser, of type Recursive Descent.                   ***
 ***                                                                       ***
 ***                                                                       ***
 *** The grammar                                                           ***
 *** --------------------------------------------------------------------- ***
 ***                                                                       ***
 *** S  --> M SP                                       (statement list)    ***
 *** SP --> ; M SP | &                                                     ***
 ***                                                                       ***
 *** M  --> str | if IFCOND(if) | while WHILE |        (str = \".*\")      ***
 ***        break | return E | quit | E                                    ***
 ***                                                                       ***
 *** E  --> C EP                                                           ***
 *** EP --> and C EP | && C EP | or C EP | || C EP | & (left associative)  ***
 ***                                                                       ***
 *** C  --> L CP                                                           ***
 *** CP --> == L CP | != L CP |                                            ***
 ***        >= L CP | <= L CP |  < L CP |  > L CP | &  (left associative)  ***
 ***                                                                       ***
 *** L  --> T LP                                                           ***
 *** LP --> + T LP | - T LP | &                        (left associative)  ***
 ***                                                                       ***
 *** T  --> X TP                                                           ***
 *** TP --> * X TP | / X TP | % X TP | &               (left associative)  ***
 ***                                                                       ***
 *** X  --> FAC XP                                                         ***
 *** XP --> ^ FAC XP | &                               (right associative) ***
 ***                                                                       ***
 *** FAC  --> F ! FACP | F                             (unary)             ***
 *** FACP --> ! FACP | &                                                   ***
 ***                                                                       ***
 *** F   --> -F | not F | !F | (E) | number | id FID | cond IFCOND(cond)   ***
 ***                                                                       ***
 *** FID --> (ELIST) | = FVM | &                                           ***
 ***                                                                       ***
 *** FVM --> macrodef | E                              (macrodef = "{.*}") ***
 ***                                                                       ***
 *** ELIST  --> E ELISTP                               (expression list)   ***
 *** ELISTP --> ,E ELISTP | &                                              ***
 ***                                                                       ***
 *** WHILE        --> (E,S)                                                ***
 *** IFCOND(if)   --> (E,S) | (E,S,S)                                      ***
 *** IFCOND(cond) --> (E,E) | (E,E,E)                                      ***
 ***                                                                       ***
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/global.h"
#include "include/tokens.h"
#include "include/symbol.h"
#include "include/emit.h"
#include "include/parse.h"
#include "include/safe_strncpy.h"
#include "include/lexer_protos.h"
#include "include/macro_protos.h"
#include "include/emit_protos.h"
#include "include/message_protos.h"

#define IFCOND_IF   1
#define IFCOND_COND 2


/*
** Constants
*/
double zero=0.0,one=1.0,infinity=9e9999;



/*
**
*/
typedef struct {
    inputStream *iS;
    symTable    *sT;

    int brk;         /* if brk=true, all operations are suspended until parsing is out of the loop construct. */
    int ret;         /* if ret=true, all operations are suspended until parsing is done. */
    int loopdepth;
    int parselevel;  /* PARSELEVEL_MACRO,PARSELEVEL_INTERACTIVE,PARSELEVEL_FILE */
} parseInfo;

/*
** if dry is >0, parsing will not emit any result, but will be a "dry run".
** I.e. all stack operations are suspended.
*/
long dry=0;

/*
** if quit=true, all operations are suspended until quit is reset (done externally).
*/
long quit=FALSE;



void ecenabledryparse(void);
void ecdisbledryparse(void);
void eccleardryparse(void);
long eccheckdryparse(void);

void ecsetinloop(parseInfo *pI);
void ecclrinloop(parseInfo *pI);
void ecrstinloop(parseInfo *pI);
long ecchkinloop(parseInfo *pI);

void match(long expectedToken, inputStream *iS);

void pS(parseInfo *pI);
void pSP(parseInfo *pI);
void pM(parseInfo *pI);
void pE(parseInfo *pI);
void pEP(parseInfo *pI);
void pC(parseInfo *pI);
void pCP(parseInfo *pI);
void pL(parseInfo *pI);
void pLP(parseInfo *pI);
void pT(parseInfo *pI);
void pTP(parseInfo *pI);
void pX(parseInfo *pI);
void pXP(parseInfo *pI);
void pFAC(parseInfo *pI);
void pFACP(parseInfo *pI);
void pF(parseInfo *pI);
void pFID(char *id,parseInfo *pI);
void pFVM(char *id,parseInfo *pI);
void pELIST(double *numparams,parseInfo *pI);
void pELISTP(double *numparams,parseInfo *pI);
void pIFCOND(int iforcond,parseInfo *pI);
void pWHILE(parseInfo *pI);



void ecenabledryparse(void) { dry++; }
void ecdisbledryparse(void) { if(dry>0) dry--; }
void eccleardryparse(void)  { dry=0; }
long eccheckdryparse(void)  { return(dry); }

void ecsetinloop(parseInfo *pI) { pI->loopdepth++; }
void ecclrinloop(parseInfo *pI) { if(pI->loopdepth>0) pI->loopdepth--; }
void ecrstinloop(parseInfo *pI) { pI->loopdepth=0; }
long ecchkinloop(parseInfo *pI) { return(pI->loopdepth); }

long ecchkquit(void) { return(quit); }



void parser_init(void)
{
    if(quit) { quit=FALSE; ecdisbledryparse(); }
    /* dry=0 ? */
}

double parse(inputStream *iS,symTable *sT,int parselevel)
{
double result=0.0;
inputStream *oldIS;
parseInfo *pI;

    if(!ecchkquit()) {
        if(iS&&sT) {
            if((pI=malloc(sizeof(parseInfo)))) {
                pI->iS=iS; pI->sT=sT;
                pI->loopdepth=0; pI->brk=FALSE; pI->ret=FALSE;
                pI->parselevel=parselevel;

                oldIS=currIS;
                currIS=iS;

                /*
                ** Get first token and start parser. Then emit result.
                ** If expression was empty, do nothing.
                */
                if(getNextToken(iS)) {
                    if(iS->cT->token!=TOKEN_END) {
                        pS(pI);
                        if(!err) {
                            if(pI->ret) { pI->ret=FALSE; ecdisbledryparse(); }
                            if(!dry) emit(ACTION_POP,&result,iS,sT);
                            
                            if(iS->cT->token!=TOKEN_END) {
                                if(iS->cT->token==TOKEN_RPAR) {
                                    ecparseerror(iS,"Unbalanced paranthesis.\n");
                                } else {
                                    ecparseerror(iS,"Operator probably missing.\n");
                                }
                            }
                        }
                    }
                } else {
                    ecparseerror(iS,"parse(): getNextToken() failed.\n");
                }

                currIS=oldIS;
                free(iS->cT); iS->cT=NULL;
                free(pI);
            } else {
                ecparseerror(iS,"parse(): parseInfo allocation failed.\n");
            }
        } else {
            ecparseerror(iS,"parse(): Called with NULL reference.\n");
        }
    }

return(result);
}



void match(long expectedToken, inputStream *iS)
{
char tokbuf[MAX_TOKNAME_LEN],tokbuf2[MAX_TOKNAME_LEN];

    if(err) return;

    if(iS->cT->token==expectedToken) {
        if(getNextToken(iS)) {
            if(iS->cT->token==TOKEN_ERR) {
                iS->i++;
                ecparseerror(iS,"Unknown token '%s'.\n",iS->cT->id);
            }
        } else {
            ecparseerror(iS,"match(): getNextToken() failed.\n");
        }
    } else {
        if(iS->cT->token==TOKEN_END) {
            ecparseerror(iS,"Unexpected end of expression.\n");
        } else {
            tokenToStr(iS->cT->token,tokbuf,MAX_TOKNAME_LEN);
            tokenToStr(expectedToken,tokbuf2,MAX_TOKNAME_LEN);
            ecparseerror(iS,"Unexpected token '%s', expected '%s'.\n",tokbuf,tokbuf2);
        }
    }
}



void pS(parseInfo *pI)
{
    if(err) return;

    pM(pI);
    pSP(pI);
}

void pSP(parseInfo *pI)
{
    if(err) return;

    if(pI->iS->cT->token==TOKEN_SEMI) {
        match(TOKEN_SEMI,pI->iS);
        if(!dry&&!err) emit(ACTION_POP,NULL,pI->iS,pI->sT);    /* Throw away result of previous M */
        pM(pI);
        pSP(pI);
    } else {
        /* Epsilon */
    }
}

void pM(parseInfo *pI)
{
    if(err) return;

    switch(pI->iS->cT->token) {
        case TOKEN_IF:
            match(TOKEN_IF,pI->iS);
            pIFCOND(IFCOND_IF,pI);
            break;

        case TOKEN_WHILE:
            match(TOKEN_WHILE,pI->iS);
            pWHILE(pI);
            break;

        case TOKEN_BREAK:
            if(ecchkinloop(pI)) {
                match(TOKEN_BREAK,pI->iS);
                if(!dry&&!err) {
                    emit(ACTION_BREAK,NULL,pI->iS,pI->sT);
                    emit(ACTION_PUSH,&zero,pI->iS,pI->sT);   /* Value of break is 0 */
                    pI->brk=TRUE; ecenabledryparse();
                }
            } else {
                ecparseerror(pI->iS,"Break statement only allowed inside a loop construct.\n");
            }        
            break;

        case TOKEN_RETURN:
            if(pI->parselevel==PARSELEVEL_MACRO) {
                match(TOKEN_RETURN,pI->iS);
                pE(pI);
                if(!dry&&!err) {
                    emit(ACTION_RETURN,NULL,pI->iS,pI->sT);
                    pI->ret=TRUE; ecenabledryparse();
                }
            } else {
                ecparseerror(pI->iS,"Return statement only allowed inside macros.\n");
            }
            break;

        case TOKEN_QUIT:
            /* quit has no value, since it suspends *all* further parsing and stack operations until end of parsing. */
            match(TOKEN_QUIT,pI->iS);
            if(!dry&&!err) {
                emit(ACTION_QUIT,NULL,pI->iS,pI->sT);
                quit=TRUE; ecenabledryparse();
            }
            break;

        case TOKEN_STR:
            match(TOKEN_STR,pI->iS);
            if(!dry&&!err) emit(ACTION_PUSH,&zero,pI->iS,pI->sT);   /* Value of a string is 0 */
            if(!dry&&!err) ecprintstring(pI->iS->cT->string);
            break;

        default:
            pE(pI);
            break;
    }
}

void pE(parseInfo *pI)
{
    if(err) return;

    pC(pI);
    pEP(pI);
}

void pEP(parseInfo *pI)
{
long op=0;

    if(err) return;

    if(pI->iS->cT->token==TOKEN_AND || pI->iS->cT->token==TOKEN_ANDAND || pI->iS->cT->token==TOKEN_OR || pI->iS->cT->token==TOKEN_OROR) {
        if(pI->iS->cT->token==TOKEN_AND || pI->iS->cT->token==TOKEN_ANDAND) {
            op=ACTION_AND;
            if(pI->iS->cT->token==TOKEN_AND) { match(TOKEN_AND,pI->iS); } else { match(TOKEN_ANDAND,pI->iS); }
        } else {
            op=ACTION_OR;
            if(pI->iS->cT->token==TOKEN_OR) { match(TOKEN_OR,pI->iS); } else { match(TOKEN_OROR,pI->iS); }
        }
        pC(pI);
        if(!dry&&!err) emit(op,NULL,pI->iS,pI->sT);
        pEP(pI);
    } else {
        /* Epsilon */
    }
}

void pC(parseInfo *pI)
{
    if(err) return;

    pL(pI);
    pCP(pI);
}

void pCP(parseInfo *pI)
{
long op=0;

    if(err) return;

    if(pI->iS->cT->token==TOKEN_CMP_EQ || pI->iS->cT->token==TOKEN_CMP_NE ||
       pI->iS->cT->token==TOKEN_CMP_GE || pI->iS->cT->token==TOKEN_CMP_LE ||
       pI->iS->cT->token==TOKEN_CMP_GT || pI->iS->cT->token==TOKEN_CMP_LT) {
        switch(pI->iS->cT->token) {
            case TOKEN_CMP_EQ:
                op=ACTION_CMP_EQ;
                match(TOKEN_CMP_EQ,pI->iS);
                break;
            case TOKEN_CMP_NE:
                op=ACTION_CMP_NE;
                match(TOKEN_CMP_NE,pI->iS);
                break;
            case TOKEN_CMP_GE:
                op=ACTION_CMP_GE;
                match(TOKEN_CMP_GE,pI->iS);
                break;
            case TOKEN_CMP_LE:
                op=ACTION_CMP_LE;
                match(TOKEN_CMP_LE,pI->iS);
                break;
            case TOKEN_CMP_GT:
                op=ACTION_CMP_GT;
                match(TOKEN_CMP_GT,pI->iS);
                break;
            case TOKEN_CMP_LT:
                op=ACTION_CMP_LT;
                match(TOKEN_CMP_LT,pI->iS);
                break;
        }
        pL(pI);
        if(!dry&&!err) emit(op,NULL,pI->iS,pI->sT);
        pCP(pI);
    } else {
        /* Epsilon */
    }
}

void pL(parseInfo *pI)
{
    if(err) return;

    pT(pI);
    pLP(pI);
}

void pLP(parseInfo *pI)
{
long op=0;

    if(err) return;

    if(pI->iS->cT->token==TOKEN_ADD || pI->iS->cT->token==TOKEN_SUB) {
        if(pI->iS->cT->token==TOKEN_ADD) {
            op=ACTION_ADD;
            match(TOKEN_ADD,pI->iS);
        } else {
            op=ACTION_SUB;
            match(TOKEN_SUB,pI->iS);
        }
        pT(pI);
        if(!dry&&!err) emit(op,NULL,pI->iS,pI->sT);
        pLP(pI);
    } else {
        /* Epsilon */
    }
}

void pT(parseInfo *pI)
{
    if(err) return;

    pX(pI);
    pTP(pI);
}

void pTP(parseInfo *pI)
{
long op=0;

    if(err) return;

    if(pI->iS->cT->token==TOKEN_MUL || pI->iS->cT->token==TOKEN_DIV || pI->iS->cT->token==TOKEN_MOD) {
        if(pI->iS->cT->token==TOKEN_MUL) {
            op=ACTION_MUL;
            match(TOKEN_MUL,pI->iS);
        } else {
            if(pI->iS->cT->token==TOKEN_MOD) {
                op=ACTION_MOD;
                match(TOKEN_MOD,pI->iS);
            } else {
                op=ACTION_DIV;
                match(TOKEN_DIV,pI->iS);
            }
        }
        pX(pI);
        if(!dry&&!err) emit(op,NULL,pI->iS,pI->sT);
        pTP(pI);
    } else {
        /* Epsilon */
    }
}

void pX(parseInfo *pI)
{
    if(err) return;

    pFAC(pI);
    pXP(pI);
}

void pXP(parseInfo *pI)
{
    if(err) return;

    if(pI->iS->cT->token==TOKEN_POW) {
        match(TOKEN_POW,pI->iS);
        pFAC(pI);
        pXP(pI);
        if(!dry&&!err) emit(ACTION_POW,NULL,pI->iS,pI->sT);  /* To make left associative, place before XP */
    } else {
        /* Epsilon */
    }
}

void pFAC(parseInfo *pI)
{
    if(err) return;

    pF(pI);
    if(pI->iS->cT->token==TOKEN_EXCL) {
        pFACP(pI);
    }
}

void pFACP(parseInfo *pI)
{
    if(err) return;

    if(pI->iS->cT->token==TOKEN_EXCL) {
        match(TOKEN_EXCL,pI->iS);
        if(!dry&&!err) emit(ACTION_FAC,NULL,pI->iS,pI->sT);
        pFACP(pI);
    } else {
        /* Epsilon */
    }
}

void pF(parseInfo *pI)
{
char *id,*tokbuf;

    if(err) return;

    switch(pI->iS->cT->token) {
        case TOKEN_LPAR:
            match(TOKEN_LPAR,pI->iS);
            pE(pI);
            if(pI->iS->cT->token==TOKEN_RPAR) {
                match(TOKEN_RPAR,pI->iS);
            } else {
                ecparseerror(pI->iS,"Unbalanced paranthesis.\n");
            }
            break;

        case TOKEN_SUB:
            match(TOKEN_SUB,pI->iS);
            pF(pI);
            if(!dry&&!err) emit(ACTION_NEG,NULL,pI->iS,pI->sT);
            break;

        case TOKEN_NOT:
        case TOKEN_EXCL:
            if(pI->iS->cT->token==TOKEN_NOT) { match(TOKEN_NOT,pI->iS); } else { match(TOKEN_EXCL,pI->iS); }
            pF(pI);
            if(!dry&&!err) emit(ACTION_NOT,NULL,pI->iS,pI->sT);
            break;

        case TOKEN_NUM:
            if(!dry&&!err) emit(ACTION_PUSH,&(pI->iS->cT->value),pI->iS,pI->sT);
            match(TOKEN_NUM,pI->iS);
            break;

        case TOKEN_ID:
            if((id=malloc(strlen(pI->iS->cT->id)+1))) {
                strcpy(id,pI->iS->cT->id); /* safe */
                match(TOKEN_ID,pI->iS);
                pFID(id,pI);
                free(id);
            } else {
                ecparseerror(pI->iS,"F(): malloc() failed for identifier.\n");
            }
            break;

        case TOKEN_TRUE:
            if(!dry&&!err) emit(ACTION_PUSH,&one,pI->iS,pI->sT);
            match(TOKEN_TRUE,pI->iS);
            break;

        case TOKEN_FALSE:
            if(!dry&&!err) emit(ACTION_PUSH,&zero,pI->iS,pI->sT);
            match(TOKEN_FALSE,pI->iS);
            break;

        case TOKEN_INFINITY:
            if(!dry&&!err) emit(ACTION_PUSH,&infinity,pI->iS,pI->sT);
            match(TOKEN_INFINITY,pI->iS);
            break;

        case TOKEN_COND:
            match(TOKEN_COND,pI->iS);
            pIFCOND(IFCOND_COND,pI);
            break;

        case TOKEN_END:
            ecparseerror(pI->iS,"Unexpected end of expression.\n");
            break;

        default:
            if((tokbuf=malloc(MAX_TOKNAME_LEN))) {
                tokenToStr(pI->iS->cT->token,tokbuf,MAX_TOKNAME_LEN);
                ecparseerror(pI->iS,"Unexpected token '%s'.\n",tokbuf);
                free(tokbuf);
            } else {
                ecparseerror(pI->iS,"F(): malloc() failed for tokname buffer.\n");
            }
            break;
    }
}

void pFID(char *id,parseInfo *pI)
{
double numparams=0;

    if(err) return;

    if(pI->iS->cT->token==TOKEN_LPAR) {
        match(TOKEN_LPAR,pI->iS);
        pELIST(&numparams,pI);
        if(!dry&&!err) emit(ACTION_PUSH,&numparams,pI->iS,pI->sT);
        match(TOKEN_RPAR,pI->iS);
        if(!dry&&!err) emit(ACTION_CALL,id,pI->iS,pI->sT);
    } else {
        if(pI->iS->cT->token==TOKEN_ASSIGN) {
            match(TOKEN_ASSIGN,pI->iS);
            pFVM(id,pI);
        } else {
            if(!dry&&!err) emit(ACTION_VAR,id,pI->iS,pI->sT);
        }
    }
}

void pFVM(char *id,parseInfo *pI)
{
    if(err) return;

    if(pI->iS->cT->token==TOKEN_MDEF) {
        if(!dry&&!err) defmacro(id,pI->iS->cT->macrodef,pI->iS,pI->sT);
        match(TOKEN_MDEF,pI->iS);
        if(!dry&&!err) emit(ACTION_PUSH,&zero,pI->iS,pI->sT);      /* Value of a macrodef is 0 */
    } else {
        pL(pI);
        if(!dry&&!err) emit(ACTION_ASSIGN,id,pI->iS,pI->sT);
    }
}

void pELIST(double *numparams,parseInfo *pI)
{
    if(err) return;

    if(pI->iS->cT->token!=TOKEN_RPAR) {
        (*numparams)++;
        pE(pI);
        pELISTP(numparams,pI);
    } else {
        (*numparams)=0;
    }
}

void pELISTP(double *numparams,parseInfo *pI)
{
    if(err) return;

    if(pI->iS->cT->token==TOKEN_COMMA) {
        match(TOKEN_COMMA,pI->iS);
        (*numparams)++;
        pE(pI);
        pELISTP(numparams,pI);
    } else {
        /* Epsilon */
    }
}


/*
** Productions for language-constructs.
*/

void pIFCOND(int iforcond,parseInfo *pI)
{
double val;

    if(err) return;

    match(TOKEN_LPAR,pI->iS);

    if(dry) {
        pE(pI);
        match(TOKEN_COMMA,pI->iS);
        if(iforcond==IFCOND_IF) { pS(pI); } else { pE(pI); }
        if(pI->iS->cT->token==TOKEN_COMMA) { /* else-branch */
            match(TOKEN_COMMA,pI->iS);
            if(iforcond==IFCOND_IF) { pS(pI); } else { pE(pI); }
        }
    } else {
        pE(pI);
        match(TOKEN_COMMA,pI->iS);
        if(!err) {
            if(!dry) emit(ACTION_POP,&val,pI->iS,pI->sT);
            if(!dry && (int)val!=0) {
                if(iforcond==IFCOND_IF) { pS(pI); } else { pE(pI); }
                if(pI->iS->cT->token==TOKEN_COMMA) {
                    match(TOKEN_COMMA,pI->iS);
                    ecenabledryparse(); if(iforcond==IFCOND_IF) { pS(pI); } else { pE(pI); } ecdisbledryparse();
                }
            } else {
                ecenabledryparse(); if(iforcond==IFCOND_IF) { pS(pI); } else { pE(pI); } ecdisbledryparse();
                if(pI->iS->cT->token==TOKEN_COMMA) {
                    match(TOKEN_COMMA,pI->iS);
                    if(iforcond==IFCOND_IF) { pS(pI); } else { pE(pI); }
                } else {
                    if(!dry&&!err) emit(ACTION_PUSH,&zero,pI->iS,pI->sT); /* value of empty else is 0 */
                }
            }
        }
    }

    /* Value of if() is always zero */
    if(iforcond==IFCOND_IF) {
        if(!dry&&!err) emit(ACTION_POP,NULL,pI->iS,pI->sT);
        if(!dry&&!err) emit(ACTION_PUSH,&zero,pI->iS,pI->sT);
    }

    match(TOKEN_RPAR,pI->iS);
}

void pWHILE(parseInfo *pI)
{
double val=0.0;
int    rememberPos;

    if(err) return;

    ecsetinloop(pI); /* for semantic check of break statement only allowed inside loop constructs */

    rememberPos=pI->iS->i;
    match(TOKEN_LPAR,pI->iS);

    if(dry) {
        pE(pI);
        match(TOKEN_COMMA,pI->iS);
        pS(pI); 
    } else {
        do {
            pE(pI);
            match(TOKEN_COMMA,pI->iS);
            if(!err) {
                if(!dry) emit(ACTION_POP,&val,pI->iS,pI->sT);
                if(!dry && (int)val!=0) {
                    pS(pI);
                    if(!dry) {
                        emit(ACTION_POP,NULL,pI->iS,pI->sT); /* throw away */
                        pI->iS->i=rememberPos; getNextToken(pI->iS);
                    }
                } else {
                    ecenabledryparse(); pS(pI); ecdisbledryparse();
                }
            }
        } while(!dry&&!err && (int)val!=0);

        val=0.0;
        if(!dry&&!err) emit(ACTION_PUSH,&val,pI->iS,pI->sT);

        if(pI->brk==TRUE) { pI->brk=FALSE; ecdisbledryparse(); }
    }

    match(TOKEN_RPAR,pI->iS);
    ecclrinloop(pI);
}
