/*****************************************************************************
 ***                                                                       ***
 *** macro.c                                                               ***
 ***                                                                       ***
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "include/macro.h"
#include "include/tokens.h"
#include "include/global.h"
#include "include/symbol.h"
#include "include/parse.h"
#include "include/safe_strncpy.h"
#include "include/parse_protos.h"
#include "include/symbol_protos.h"
#include "include/lexer_protos.h"
#include "include/macro_protos.h"
#include "include/message_protos.h"
#include "include/architecture.h"



/*
** Helper functions for internal macros
*/
double timeofday(void);
int double_cmp(const void *a, const void *b);



/*
**
*/
clock_t etime;



/*****************************************************************************
 ***                                                                       ***
 *** initmacros()  -  Puts the internal macros into the global symtable.   ***
 ***                                                                       ***
 *****************************************************************************/
void initmacros(symTable *sT)
{
long a;
char *intmacroname[]={"sin",  "cos",  "tan",  "asin", "acos", "atan",  "sqr",
                      "sqrt", "log",  "lg",   "ln",   "int",  "frac",  "floor",
                      "ceil", "round","rnd" , "trunc","rand", "abs" ,  "max",
                      "min",  "time", "tod",  "etime","print","avg",  "med",
                      NULL};
long  intmacroargc[]={1,      1,      1,      1,      1,      1,       1,
                      1,      2,      1,      1,      1,      1,       1,
                      1,      1,      1,      1,      1,      1,      -1,
                     -1,      0,     -1,     -1,     -1,     -1,      -1,
                      0};

   /*
   ** Add internal macros to symbol table.
   */
   for(a=0;intmacroname[a]!=NULL;a++) {
       addsymbol(sT,TYPE_INTERNAL|TYPE_MACRO,intmacroname[a],
                 0,"",intmacroargc[a]);
   }

   /*
   ** Start random number generator
   ** for rand(x),rnd(x) internal macro.
   */
   srand(time(NULL));

   /*
   ** Init start value for etime()-function to current start time for EC.
   */
   etime=clock();

   /*
   ** Config-vars for macros
   */
   addsymbol(sT,TYPE_INTERNAL|TYPE_VAR,"_printmode",0.0,NULL,-1);
}



/*****************************************************************************
 ***                                                                       ***
 *** defmacro()  -  Defines a macro.                                       ***
 ***                                                                       ***
 *****************************************************************************/
long defmacro(char *id,char *macro,inputStream *iS,symTable *sT)
{
double tmp;
long   i,nparams,bracketcount;
long   chktype,chknparams;

    /*
    ** Make sure the macro isn't already defined.
    */ 
    if(1) {
        /*
        ** Make sure it isn't a variable. Also,
        ** names beginning with a $ can't be defined.
        */
        if(!lookupvar(sT,id,&tmp,FALSE) || lookupmacro(sT,id,&chktype,&chknparams,FALSE)) {
            if(*id!='$') {
                i=nparams=bracketcount=0; 
                while(macro[i]) {
                    if(macro[i]=='{') bracketcount++;
                    if(macro[i]=='}') bracketcount--;
                    if(bracketcount==0 && macro[i]=='$' && macro[i+1]) {
                        if(strtol(&macro[i+1],NULL,10)>nparams) {
                            nparams=atoi(&macro[i+1]);
                        }
                    }
                    i++;
                }
                if(nparams<=MAX_MACRO_PARAM_COUNT) {
                    if(lookupmacro(sT,id,&chktype,&chknparams,FALSE)) {
                        ecwarning("Macro %s() is already defined, overwriting with new definition.\n",id);
                    }
                    setsymbol(sT,TYPE_MACRO,id,0.0,macro,nparams);
                } else {
                    ecparseerror(iS,"Too many parameters in macro definition, only %d allowed.\n",MAX_MACRO_PARAM_COUNT);
                    return(FALSE);
                }
            } else {
                ecparseerror(iS,"Error defining macro; Macro names can't begin with $.\n");
              return(FALSE);
            }
        } else {
            ecparseerror(iS,"Error defining macro; %s is a variable.\n",id);
            return(FALSE);
        }
    } else {
        ecparseerror(iS,"Error defining macro; %s() is already defined.\n",id);
        return(FALSE);
    }

return(TRUE);
}



/*****************************************************************************
 ***                                                                       ***
 *** domacro()  -  Executes a macro.                                       ***
 ***                                                                       ***
 *****************************************************************************/
long domacro(char *id,inputStream *iS,symTable *sT,long argc,double *argv,double *result)
{
long         a,omode,type,nparams;
char        *macro;
double       res,tmp;
inputStream *iSnew=NULL;
symTable    *sTlocal=NULL;
char         sym[8];

    /*
    ** Try to find macro by name.
    */
    if((macro=lookupmacro(sT,id,&type,&nparams,TRUE))) {
        /*
        ** Check if this is an Internal macro and if so, execute it.
        */
        if(type&TYPE_INTERNAL) {
            ecverboseplus("FUNC   %s() = <internal>\n",id);
            if(argc==nparams || nparams==-1) {
                res=0.0;
                if(strcmp(id,"sin")==0)   res=sin(argv[0]);
                if(strcmp(id,"cos")==0)   res=cos(argv[0]);
                if(strcmp(id,"tan")==0)   res=tan(argv[0]);
                if(strcmp(id,"asin")==0)  res=asin(argv[0]);
                if(strcmp(id,"acos")==0)  res=acos(argv[0]);
                if(strcmp(id,"atan")==0)  res=atan(argv[0]);
                if(strcmp(id,"lg")==0)    res=log10(argv[0]);
                if(strcmp(id,"ln")==0)    res=log(argv[0]);
                if(strcmp(id,"log")==0)   res=(log(argv[1])/log(argv[0]));
                if(strcmp(id,"abs")==0)   res=((argv[0]<0 || argv[0]==-0)? -argv[0] : argv[0]);
                if(strcmp(id,"floor")==0) res=floor(argv[0]);
                if(strcmp(id,"ceil")==0)  res=ceil(argv[0]);
                if(strcmp(id,"round")==0) res=((argv[0]>=0) ? (double)((int)(argv[0]+.5)) : (double)((int)(argv[0]-.5)));

                if(strcmp(id,"time")==0)  res=time(NULL);
                if(strcmp(id,"frac")==0)  res=((argv[0]>=0) ? argv[0]-floor(argv[0]) : argv[0]-ceil(argv[0]));
                if(strcmp(id,"int")==0 || strcmp(id,"trunc")==0) res=floor(argv[0]);
                if(strcmp(id,"sqr")==0 || strcmp(id,"sqrt")==0) res=sqrt(argv[0]);
                if(strcmp(id,"rnd")==0 || strcmp(id,"rand")==0)
                    res=(double)(rand()%((int)floor(argv[0])+1));
                
                if(strcmp(id,"tod")==0) {
                    res=timeofday();
                }

                if(strcmp(id,"etime")==0) {
                    if(argc==0||argc==1) {
                        if(argc==0) {
                            res=((double)(clock()-etime)/CLOCKS_PER_SEC/3600); /* Result in seconds */
                        } else {
                            etime=clock();
                        }
                    } else {
                        ecparseerror(iS,"etime() called with %ld parameter%s, should be 0 or 1.\n",argc,(argc==1 ? "" : "s"));
                        res=0.0;
                    }
                }

                if(strcmp(id,"print")==0) {
                    if(argc>0) {
                        for(a=0;a<argc;a++) {
                            lookupvar(sT,"_printmode",&tmp,TRUE);
                            omode=getresultmode();
                            setresultmode((long)tmp);
                            ecprintresult(argv[a]);
                            setresultmode(omode);
                        }
                        res=argv[argc-1];
                    } else {
                        res=0.0;
                    }
                }

                if(strcmp(id,"min")==0) {
                    if(argc>0) {
                        res=argv[0];
                        for(a=0;a<argc;a++) if(argv[a]<res) res=argv[a];
                    } else {
                        res=0.0;
                    }
                }
                if(strcmp(id,"max")==0) {
                    if(argc>0) {
                        res=argv[0];
                        for(a=0;a<argc;a++) if(argv[a]>res) res=argv[a];
                    } else {
                        res=0.0;
                    }
                }

                if(strcmp(id,"avg")==0) {
                    if(argc>0) {
                        for(tmp=0,a=0;a<argc;a++) tmp+=argv[a];
                        res=tmp/argc;
                    } else {
                        res=0.0;
                    }
                }
                if(strcmp(id,"med")==0) {
                    if(argc>0) {
                        qsort(argv,argc,sizeof(double),double_cmp);
                        if(argc&1) {
                            res=argv[argc/2];
                        } else {
                            res=(argv[argc/2-1]+argv[argc/2])/2;
                        }
                    } else {
                        res=0.0;
                    }
                }

                *result=res;
                return(TRUE);
            } else {
                ecparseerror(iS,"%s() called with %ld parameter%s, should be %ld.\n",
                            id,argc,(argc==1 ? "" : "s"),nparams);
                *result=0.0;
                return(FALSE);
            }
        } else {
            /*
            ** This is a defined macro
            */
            ecverboseplus("FUNC   %s() = {%s}\n",id,macro);

            /*
            ** Check number of parameters
            */
            if(nparams!=argc) {
                ecparseerror(iS,"%s() called with %ld parameter%s, should be %ld.\n",
                        id,argc,(argc==1 ? "" : "s"),nparams);
                return(FALSE);
            }

            /*
            ** Set up local symbol table for the macro arguments
            */
            if(argc<=MAX_MACRO_PARAM_COUNT) {
                sTlocal=initsym(sT);
                for(a=0;a<argc;a++) {
                    snprintf(sym,8,"$%ld",a+1);
                    addsymbol(sTlocal,TYPE_VAR,sym,argv[a],NULL,0);
                }
            } else {
                ecparseerror(iS,"More than %d parameters in macro call not allowed.\n",MAX_MACRO_PARAM_COUNT);
                *result=0.0;
                return(FALSE);
            }

            /*
            ** Set up input stream
            */
            if((iSnew=allocStream("macro",iS->filename,id,macro))) {
                res=parse(iSnew,sTlocal,PARSELEVEL_MACRO);
                
                freeStream(iSnew);
                deletesym(sTlocal);

                *result=res;
                return(TRUE);
            } else {
                ecparseerror(iS,"Out of memory for macro %s()'s inputStream/currentToken.\n",id);
                *result=0.0;
                return(FALSE);
            }
        }
    } else {
        ecparseerror(iS,"Unknown macro %s().\n",id);
        *result=0.0;
        return(FALSE);
    }
}



/*
** Internal macro helper functions
*/
double timeofday(void)
{
time_t t;
struct tm *tx;

    t=time(NULL);
    tx=localtime(&t);

    return(tx->tm_hour+tx->tm_min/60.0+tx->tm_sec/3600.0);
}


int double_cmp(const void *a, const void *b)
{
    return(((*(double *)a<*(double *)b) ? -1 :
            ((*(double *)a>*(double *)b) ? 1 : 0)));
}
