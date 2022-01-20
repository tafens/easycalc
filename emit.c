/*****************************************************************************
 ***                                                                       ***
 *** emit.c                                                                ***
 ***                                                                       ***
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "include/global.h"
#include "include/emit.h"
#include "include/symbol.h"
#include "include/message.h"
#include "include/parse_protos.h"
#include "include/symbol_protos.h"
#include "include/macro_protos.h"
#include "include/message_protos.h"



static double stack[MAX_STACK_PTR];
static long   sp=-1;



/*****************************************************************************
 ***                                                                       ***
 *** emit()  -  Perform operations on the stack.                           ***
 ***                                                                       ***
 *****************************************************************************/
void emit(long action,void *value,inputStream *iS,symTable *sT)
{
double        tmp;
long          a,type,nparams;

    if(err) return;

    switch(action) {
        case ACTION_ADD:
            stack[sp-1]=stack[sp-1]+stack[sp];
            sp--;
            ecverboseplus("ADD    -> %-14.5f",stack[sp]);
            break;

        case ACTION_SUB:
            stack[sp-1]=stack[sp-1]-stack[sp];
            sp--;
            ecverboseplus("SUB    -> %-14.5f",stack[sp]);
            break;

        case ACTION_MUL:
            stack[sp-1]=stack[sp-1]*stack[sp];
            sp--;
            ecverboseplus("MUL    -> %-14.5f",stack[sp]);
            break;

        case ACTION_DIV:
            if(stack[sp]==0.0) {
                ecparseerror(iS,"Division by zero.\n");
            } else {
                stack[sp-1]=stack[sp-1]/stack[sp];
                sp--;
                ecverboseplus("DIV    -> %-14.5f",stack[sp]);
            }
            break;

        case ACTION_MOD:
            if(stack[sp]==0.0) {
                ecparseerror(iS,"Modulus (division) by zero.\n");
            } else {
                /*
                ** Formula: (a/b)*b+a%b==a  ==>  a%b=a-(a/b)*b
                */
#if 0
                stack[sp-1]=((stack[sp-1]/stack[sp])-
                            floor(stack[sp-1]/stack[sp]))*stack[sp];
#endif
                stack[sp-1]=stack[sp-1]-(floor(stack[sp-1]/stack[sp]))*stack[sp];
                sp--;
                ecverboseplus("MOD    -> %-14.5f",stack[sp]);
            }
            break;

        case ACTION_POW:
            stack[sp-1]=pow(stack[sp-1],stack[sp]);
            sp--;
            ecverboseplus("POW    -> %-14.5f",stack[sp]);
            break;

        case ACTION_FAC:
            if(floor(stack[sp])==stack[sp]) {
                if((long)stack[sp]>=0) {
                    for(tmp=1,a=1;a<=(long)stack[sp];a++) {
                        tmp*=a;
                    }
                    stack[sp]=tmp;
                    ecverboseplus("FAC    -> %-14.5f",stack[sp]);
                } else {
                    ecparseerror(iS,"Faculty undefined for negative numbers.\n");
                }
            } else {
                ecparseerror(iS,"Faculty undefined for non-integers.\n");
            }
            break;

        case ACTION_NEG:
            stack[sp]=-stack[sp];
            ecverboseplus("NEG    -> %-14.5f",stack[sp]);
            break;

        case ACTION_PUSH:
            if(sp<-1) {
                ecparseerror(iS,"Stack out of bounds.\n");
                break;
            }
            if(sp>=(MAX_STACK_PTR-1)) {
                ecparseerror(iS,"Stack overflow.\n");
                break;
            }
            sp++;
            stack[sp]=*((double*)value);
            ecverboseplus("PUSH   -> %-14.5f",*((double*)value));
            break;

        case ACTION_VAR:
            if(sT->parent || *((char*)value)!='$') {
                ecverboseplus("VAR    %s\n",(char*)value);
                tmp=0.0; lookupvar(sT,(char*)value,&tmp,TRUE);
                emit(ACTION_PUSH,&tmp,iS,sT);
            } else {
                ecparseerror(iS,"$-variables can only be used inside macros.\n");
            }
            break;

        case ACTION_ASSIGN:
            if(!lookupmacro(sT,(char*)value,&type,&nparams,FALSE)) {
                if(sT->parent || *((char*)value)!='$') {
                    setsymbol(sT,TYPE_VAR,(char*)value,stack[sp],NULL,-1);
                    ecverboseplus("ASSIGN %s = %-14.5f\n",(char*)value,stack[sp]);
                } else {
                    ecparseerror(iS,"$-variables can only be assigned inside macros.\n");
                }
            } else {
                ecparseerror(iS,"%s() is a macro.\n",(char*)value);
            }
            break;
            
        case ACTION_AND:
            stack[sp-1]=((stack[sp-1]!=0 && stack[sp]!=0) ? 1 : 0);
            sp--;
            ecverboseplus("AND    -> %-14.5f",stack[sp]);
            break;

        case ACTION_OR:
            stack[sp-1]=((stack[sp-1]!=0 || stack[sp]!=0) ? 1 : 0);
            sp--;
            ecverboseplus("OR     -> %-14.5f",stack[sp]);
            break;

        case ACTION_NOT:
            stack[sp]=(stack[sp]==0 ? 1 : 0);
            ecverboseplus("NOT    -> %-14.5f",stack[sp]);
            break;

        case ACTION_CMP_EQ:
            stack[sp-1]=((stack[sp-1]==stack[sp]) ? 1 : 0);
            sp--;
            ecverboseplus("CMP_EQ -> %-14.5f",stack[sp]);
            break;

        case ACTION_CMP_NE:
            stack[sp-1]=((stack[sp-1]!=stack[sp]) ? 1 : 0);
            sp--;
            ecverboseplus("CMP_NE -> %-14.5f",stack[sp]);
            break;

        case ACTION_CMP_GE:
            stack[sp-1]=((stack[sp-1]>=stack[sp]) ? 1 : 0);
            sp--;
            ecverboseplus("CMP_GE -> %-14.5f",stack[sp]);
            break;

        case ACTION_CMP_LE:
            stack[sp-1]=((stack[sp-1]<=stack[sp]) ? 1 : 0);
            sp--;
            ecverboseplus("CMP_LE -> %-14.5f",stack[sp]);
            break;

        case ACTION_CMP_GT:
            stack[sp-1]=((stack[sp-1]>stack[sp]) ? 1 : 0);
            sp--;
            ecverboseplus("CMP_GT -> %-14.5f",stack[sp]);
            break;

        case ACTION_CMP_LT:
            stack[sp-1]=((stack[sp-1]<stack[sp]) ? 1 : 0);
            sp--;
            ecverboseplus("CMP_LT -> %-14.5f",stack[sp]);
            break;

        case ACTION_CALL:
            if(getverbosity()>=VERBOSITY_PLUS) {
                ecverboseplus("CALL   %s(",(char*)value);
                if(((long)stack[sp])>0)
                    ecverboseplus("%f",stack[sp-(long)stack[sp]]);

                for(a=1;a<((long)stack[sp]);a++)
                    ecverboseplus(",%f",stack[sp-(long)stack[sp]+a]);

                ecverboseplus(") [%ld]\n",(long)stack[sp]);
            }

            if(domacro((char*)value,iS,sT,(long)stack[sp],&stack[sp-(long)stack[sp]],&tmp)) {
                if(!err) {
                    setsymbol(sT,TYPE_VAR,(char*)value,tmp,NULL,-1);
                    ecverboseplus("       %s() = %-14.5f\n",(char*)value,tmp);
                    for(a=(long)stack[sp];a>=0;a--) sp--;
                    emit(ACTION_PUSH,&tmp,iS,sT);
                }
            }
            break;

        case ACTION_POP:
            if(sp>=0) {
                if(value) {
                    ecverboseplus("POP    -> %-14.5f\n",stack[sp]);
                    *((double*)value)=stack[sp];
                } else {
                    ecverboseplus("POP\n");
                }
                sp--;
            } else {
                ecparseerror(iS,"emit(ACTION_POP): Stack pointer out of bounds.\n");
                *((double*)value)=0.0;
            }
            break;

        case ACTION_ERR:
            ecverboseplus("ERROR\n");
            emit(ACTION_CLEARSTACK,NULL,NULL,NULL);
            err=TRUE;
            break;

        case ACTION_BREAK:
            ecverboseplus("BREAK\n");
            break;

        case ACTION_RETURN:
            ecverboseplus("RET   -> %-14.5f\n",stack[sp]);
            break;

        case ACTION_QUIT:
            ecverboseplus("QUIT\n");
            emit(ACTION_CLEARSTACK,NULL,NULL,NULL);
            break;

        case ACTION_PRINTSTACK:
            ecmessage("sp=%ld\n",sp);
            for(a=0;a<=20;a++) ecmessage("%02ld: %f\n",a,stack[a]);
            break;

        case ACTION_CLEARSTACK:
            sp=-1;
            for(a=0;a<MAX_STACK_PTR;a++) stack[a]=0;
            ecverboseplus("CLEAR\n");
            break;

        default:
            ecparseerror(iS,"emit(): Unknown action %ld\n",action);
            break;
    }

    if(!err && getverbosity()>=VERBOSITY_PLUS
                           && (action==ACTION_ADD    || action==ACTION_SUB    || 
                               action==ACTION_MUL    || action==ACTION_DIV    || 
                               action==ACTION_MOD    || action==ACTION_POW    || 
                               action==ACTION_FAC    || action==ACTION_NEG    || 
                               action==ACTION_AND    || action==ACTION_OR     || action==ACTION_NOT     ||
                               action==ACTION_CMP_EQ || action==ACTION_CMP_NE ||
                               action==ACTION_CMP_GT || action==ACTION_CMP_LT ||
                               action==ACTION_CMP_GE || action==ACTION_CMP_LE ||
                               action==ACTION_PUSH)) {
        ecverboseplus(" (%4.2f",stack[0]);
        for(a=1;a<=sp;a++) {
            ecverboseplus(", %4.2f",stack[a]);
        }
        ecverboseplus(")\n");
    }
}
