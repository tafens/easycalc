/*****************************************************************************
 ***                                                                       ***
 *** symbol.c                                                              ***
 ***                                                                       ***
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "include/tokens.h"
#include "include/global.h"
#include "include/symbol.h"
#include "include/safe_strncpy.h"
#include "include/symbol_protos.h"
#include "include/message_protos.h"




/*****************************************************************************
 ***                                                                       ***
 *** initsym()  -  Inits a symbol table by allocating a root node.         ***
 ***                                                                       ***
 *****************************************************************************/
symTable *initsym(symTable *parent)
{
symTable *tmpTab;

    if((tmpTab=malloc(sizeof(symTable)))) {
        tmpTab->parent=parent;
        tmpTab->first=NULL;
    } else {
        ecgeneralerror("initsym(): malloc() failed.\n");
        return(NULL);
    }

return(tmpTab);
}



/*****************************************************************************
 ***                                                                       ***
 *** deletesym()  -  Removes a symbol table and all its elements.          ***
 ***                                                                       ***
 *****************************************************************************/
void deletesym(symTable *symtab)
{
symEntry *tmp,*tm2;

    if(symtab) {
        if((tmp=symtab->first)) {
            for(;tmp;tmp=tm2) {
                tm2=tmp->next;
                free(tmp);
            }
        }
        free(symtab);
    } else {
        ecgeneralerror("deletesym(): called with NULL reference.\n");
    }
}



/*****************************************************************************
 ***                                                                       ***
 *** addsymbol()  -  Adds an entry to the symbol table in alphabetical     ***
 ***                 order.                                                ***
 ***                                                                       ***
 *****************************************************************************/
void addsymbol(symTable *symtab,long type,
               char *id,double value,char *func,long nparams)
{
symEntry *tmp,*newentry;
int       found;

    if(symtab && id) {
        /*
        ** See if symbol already exists
        */
        found=FALSE;
        for(tmp=symtab->first;tmp;tmp=tmp->next) {
            if(strcmp(id,tmp->id)==0) {
                found=TRUE; break;
            }
        }
        if(!found) {
            if((newentry=malloc(sizeof(symEntry)))) {
                /*
                ** Make a new entry
                */
                newentry->next=NULL;
                newentry->type=type;
                safe_strncpy(newentry->id,id,MAX_IDENT_LEN);
                if(type&TYPE_MACRO) {
                    safe_strncpy(newentry->func,func,MAX_INPUT_LEN);
                    newentry->value=0.0;
                    newentry->nparams=nparams;
                } else {
                    strcpy(newentry->func,"");
                    newentry->value=value;
                    newentry->nparams=0;
                }
                
                /*
                ** Put entry into table
                */
                if((tmp=symtab->first)) {
                    for(;(tmp->next && strcmp(id,tmp->next->id)>0);tmp=tmp->next);
                    newentry->next=tmp->next;
                    tmp->next=newentry;
                } else {
                    symtab->first=newentry;
                }
            } else {
                ecgeneralerror("addsymbol(): malloc() failed.\n");
            }
        } else {
            ecgeneralerror("addsymbol(): symbol already exists.\n");
        }
    } else {
        ecgeneralerror("addsymbol(): called with NULL reference.\n");
    }
}



/*****************************************************************************
 ***                                                                       ***
 *** setsymbol()  -  sets a symbol to a new value, or if the symbol        ***
 ***                 doesn't exist calls addsymbol().                      ***
 ***                                                                       ***
 *****************************************************************************/
long setsymbol(symTable *symtab,long type,
               char *id,double value,char *func,long nparams)
{
symEntry *tmp;
int       found=FALSE;

    if(symtab && id) {
        /*
        ** See if symbol already exists
        */
        for(tmp=symtab->first;tmp;tmp=tmp->next) {
            if(strcmp(id,tmp->id)==0) {
                found=TRUE; break;
            }
        }
        if(found) {
            tmp->value=value;
            if(func) {
                safe_strncpy(tmp->func,func,MAX_INPUT_LEN);
                if(nparams>=0) tmp->nparams=nparams;
            }
        } else {
            addsymbol(symtab,type,id,value,func,nparams);
        }
    } else {
        ecgeneralerror("setsymbol(): called with NULL reference.\n");
    }

return(found);
}



/*****************************************************************************
 ***                                                                       ***
 *** lookupvar()  -  Finds the value of a symbol or returns FALSE.         ***
 ***                 Looks through this symbol table as well as            ***
 ***                 its parents.                                          ***
 ***                                                                       ***
 *****************************************************************************/
long lookupvar(symTable *symtab,char *id,double *varval,int searchparents)
{
symTable *tmptab;
symEntry *tmp;

    *varval=0.0;
    if(symtab && id) {
        for(tmptab=symtab;tmptab;tmptab=tmptab->parent) {
            for(tmp=tmptab->first;tmp;tmp=tmp->next) {
                if(strcmp(id,tmp->id)==0) {
                    *varval=tmp->value;
                    return(TRUE);
                }
            }
            if(!searchparents) return(FALSE);
        }
    } else {
        ecgeneralerror("lookupvar(): called with NULL refrence.\n");
    }

return(FALSE);
}



/*****************************************************************************
 ***                                                                       ***
 *** lookupmacro()  -  Finds and returns pointer to macro string or FALSE. ***
 ***                                                                       ***
 *****************************************************************************/
char *lookupmacro(symTable *symtab,char *id,long *type,long *nparams,int searchparents)
{
symTable *tmptab;
symEntry *tmp;

    if(symtab && id) {
        for(tmptab=symtab;tmptab;tmptab=tmptab->parent) {
            for(tmp=tmptab->first;tmp;tmp=tmp->next) {
                if(strcmp(id,tmp->id)==0 && tmp->type&TYPE_MACRO) {
                    *type=tmp->type;
                    *nparams=tmp->nparams;
                    return(tmp->func);
                }
            }
            if(!searchparents) return(FALSE);
        }
    } else {
        ecgeneralerror("lookupmacro(): called with NULL reference.\n");
    }

return(NULL);
}



/*****************************************************************************
 ***                                                                       ***
 *** deletemacro()  -  removes a macro from the symbol table.              ***
 ***                                                                       ***
 *****************************************************************************/
void deletemacro(symTable *symtab, char *id)
{
struct symentry *tmp,*tm2;

    if(symtab && id) {
        if((tmp=symtab->first)) {
            for(;tmp;tmp=tmp->next) {
                if(tmp->next && tmp->next->type&TYPE_MACRO && strcmp(tmp->next->id,id)==0) {
                    tm2=tmp->next;
                    tmp->next=tmp->next->next;
                    free(tm2);
                    break;
                }
            }
        }
    } else {
        ecgeneralerror("deletemacro(): called with NULL reference.\n");
    }
}



/*****************************************************************************
 ***                                                                       ***
 *** deletevar()  -  Removes a variable from the symbol table.             ***
 ***                                                                       ***
 *****************************************************************************/
void deletevar(symTable *symtab, char *id)
{
struct symentry *tmp,*tm2;

    if(symtab && id) {
        if((tmp=symtab->first)) {
            for(;tmp;tmp=tmp->next) {
                if(tmp->next && tmp->next->type&TYPE_VAR && strcmp(tmp->next->id,id)==0) {
                    tm2=tmp->next;
                    tmp->next=tmp->next->next;
                    free(tm2);
                    break;
                }
            }
        }
    } else {
        ecgeneralerror("deletevar(): called with NULL reference.\n");
    }
}



/*****************************************************************************
 ***                                                                       ***
 *** printmacros()  -  Prints a list of defined macros.                    ***
 ***                                                                       ***
 *****************************************************************************/
void printmacros(symTable *symtab)
{
symEntry *tmp;
char     *buf,nbuf[64];
long      nprinted=0,a=0,b=0,len=0;

    ecmessage("+-----------------+--------------+--------------------------------------------+\n"
              "| Macro           | Last value   | Definition                                 |\n"
              "+-----------------+--------------+--------------------------------------------+\n");

    if(symtab) {
        if((tmp=symtab->first)) {
            for(;tmp;tmp=tmp->next) {
                if(tmp->type&TYPE_MACRO && !(tmp->type&TYPE_INTERNAL)) {
                    snprintf(nbuf,64,"%s(%d)",tmp->id,tmp->nparams);

                    if(strlen(tmp->func)<=40) {
                        ecmessage("| %-15s | %-12g | %-42s |\n",nbuf,tmp->value,tmp->func);
                    } else {
                        /* if macrodef is wider than the display-width, break it up */
                        if((buf=malloc((42+1)*sizeof(char)))) {
                            a=0; b=a+42-1; buf[0]='\0'; len=strlen(tmp->func);
                            while(a<len-1) {
                                b=a;
                                if(b+42-1<len) {
                                    if(b==a) { b=a+42-1; while((b>a)&&tmp->func[b]!=';') { b--; } if(tmp->func[b]==';') b++; }
                                    if(b==a) { b=a+42-1; while((b>a)&&tmp->func[b]!=',') { b--; } if(tmp->func[b]==',') b++; }
                                    if(b==a) {
                                        b=a+42-1;
                                        while((b>a)&&(tmp->func[b]!='+'&&tmp->func[b]!='-'&&tmp->func[b]!='*'&&tmp->func[b]!='/'&&tmp->func[b]!='%')) { b--; }
                                        if(tmp->func[b]!='+'||tmp->func[b]!='-'||tmp->func[b]!='*'||tmp->func[b]!='/'||tmp->func[b]!='%') b++;
                                    }
                                    if(b==a) { b=a+42-1; while((b>a)&&tmp->func[b]!='=') { b--; } if(tmp->func[b]=='=') b++; }
                                    if(b==a) { b=a+42-1; }
                                } else {
                                    b=len;
                                }
                                safe_strncpy(buf,&(tmp->func[a]),b-a+1);

                                if(a==0) {
                                    ecmessage("| %-15s | %-12g | %-42s |\n",nbuf,tmp->value,buf);
                                } else {
                                    ecmessage("|                 |              | %-42s |\n",buf);
                                }
                                a=b;
                            }
                            free(buf);
                        }
                    }
                    nprinted++;
                }
            }
        }
        if(nprinted==0) ecmessage("| No macros defined.                                                          |\n");
        ecmessage("+-----------------+--------------+--------------------------------------------+\n");
    } else {
        ecgeneralerror("printmacros(): called with NULL reference.\n");
    }
}



/*****************************************************************************
 ***                                                                       ***
 *** printvars()  -  Prints a list of defined variables.                   ***
 ***                                                                       ***
 *****************************************************************************/
void printvars(symTable *symtab)
{
symEntry *tmp;
long      nprinted=0;

    if(symtab) {
        if((tmp=symtab->first)) {
            ecmessage("+--------------+--------------------------------------------------------------+\n"
                      "| Variable     | Value                                                        |\n"
                      "+--------------+--------------------------------------------------------------+\n");
            for(;tmp;tmp=tmp->next) {
                if(tmp->type&TYPE_VAR && !(tmp->type&TYPE_INTERNAL)) {
                    ecmessage("| %-12s | %-20.5f                                         |\n",tmp->id,tmp->value);
                    nprinted++;
                }
            }
        }
        if(nprinted==0) ecmessage("| No variables defined.                                                       |\n");
        ecmessage("+--------------+--------------------------------------------------------------+\n");
    } else {
        ecgeneralerror("printvars(): called with NULL reference.\n");
    }
}



/*****************************************************************************
 ***                                                                       ***
 *** printconfigvars()  -  Prints a list of defined variables.             ***
 ***                                                                       ***
 *****************************************************************************/
void printconfigvars(symTable *symtab)
{
symEntry *tmp;
long      nprinted=0;

    if(symtab) {
        if((tmp=symtab->first)) {
            ecmessage("+--------------+--------------------------------------------------------------+\n"
                      "| Setting      | Value                                                        |\n"
                      "+--------------+--------------------------------------------------------------+\n");
            for(;tmp;tmp=tmp->next) {
                if(tmp->type&TYPE_VAR && tmp->type&TYPE_INTERNAL) {
                    ecmessage("| %-12s | %-20.5f                                         |\n",tmp->id,tmp->value);
                    nprinted++;
                }
            }
        }
        if(nprinted==0) ecmessage("| No config variables defined.                                                |\n");
        ecmessage("+--------------+--------------------------------------------------------------+\n");
    } else {
        ecgeneralerror("printconfigvars(): called with NULL reference.\n");
    }
}
