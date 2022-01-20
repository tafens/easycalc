/*****************************************************************************
 ***                                                                       ***
 *** readargs.c  -  Parses command-line options                            ***
 ***                                                                       ***
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "readargs.h"
#include "readargs_protos.h"

#define  TRUE  1
#define  FALSE 0




/*****************************************************************************
 ***                                                                       ***
 *** readargs() - Parses the arguments in argv[] and returns a linked list ***
 ***              with the values.                                         ***
 ***                                                                       ***
 *****************************************************************************/
arglist readargs(long argc, char *argv[], long nargs, struct rdargs *args)
{
arglist  tmp=NULL,otmp=NULL,tm2=NULL,normargs=NULL,otherargs=NULL,result=NULL;
long     a,b,c,argfound,dobreak,stronly;

    /*
    ** Loop through all argv[x]
    */
    stronly=FALSE;
    for(a=1;a<argc;a++) {
	if(strcmp(argv[a],"--")==0) {
	    stronly=TRUE;                    /* Rest of options are literal. */
	    continue;
	}

	/*
	** Is it an option argument?
	*/
	if(argv[a] && argv[a][0]=='-' && !stronly) {
	    /*
	    ** Then loop through each letter
	    */
            dobreak=FALSE; b=1;
	    while(argv[a] && argv[a][b]) {
		argfound=FALSE;
		for(c=0;c<nargs;c++) {
		    if(args[c].letter==argv[a][b]) { argfound=TRUE; break; }
		}
		if(argfound) {
		    if((tm2=malloc(sizeof(struct arg)))) {
			tm2->argid=args[c].argid;
			tm2->type=args[c].type;
			tm2->boolval=FALSE;
			tm2->intval=0;
			tm2->floatval=0.0;
			tm2->strval=NULL;

			tm2->next=NULL;

			/*
			** Convert BOOL
			*/
			if(args[c].type==ARGTYPE_BOOL) {
			    tm2->boolval=TRUE;
			}
			/*
			** Convert STRING
			*/
			if(args[c].type==ARGTYPE_STRING) {
			    if(argv[a][b+1]=='\0') {
				if(a+1<argc) {
				    tm2->strval=strdup(argv[a+1]);
				    a++; dobreak=TRUE;
				} else {
				    tm2->strval=NULL;
				}
			    } else {
				tm2->strval=strdup(&argv[a][b+1]);
				dobreak=TRUE;
			    }
			}
			/*
			** Convert INT
			*/
			if(args[c].type==ARGTYPE_INT) {
			    if(argv[a][b+1]=='\0') {
				if(a+1<argc) {
				    tm2->intval=strtol(argv[a+1],NULL,10);
				    a++; dobreak=TRUE;
				} else {
				    tm2->intval=0;
				}
			    } else {
				tm2->intval=strtol(&argv[a][b+1],NULL,10);
				while(isdigit(argv[a][b+1])) b++;
			    }
			}
			/*
			** Convert FLOAT
			*/
			if(args[c].type==ARGTYPE_FLOAT) {
			    if(argv[a][b+1]=='\0') {
				if(a+1<argc) {
				    tm2->floatval=strtod(argv[a+1],NULL);
				    a++; dobreak=TRUE;
				} else {
				    tm2->floatval=0.0;
				}
			    } else {
				tm2->floatval=strtod(&argv[a][b+1],NULL);
				while(isdigit(argv[a][b+1]) || argv[a][b+1]=='.') b++;
			    }
			}

			if(!normargs) {
			    normargs=tmp=tm2;
			} else {
			    tmp->next=tm2; tmp=tm2;
			}

			if(dobreak) break;
		    }
		} else {
		    /*
		    ** Convert UNKNOWN (as string)
		    */
		    if((tm2=malloc(sizeof(struct arg)))) {
			tm2->argid=ARGID_UNKNOWN;
			tm2->type=ARGTYPE_STRING;
			if((tm2->strval=malloc(sizeof(char)*2))) {
			    tm2->strval[0]=argv[a][b];
			    tm2->strval[1]='\0';
			}
			tm2->next=NULL;
			
			if(tm2) {
			    if(!normargs) {
				normargs=tmp=tm2;
			    } else {
				tmp->next=tm2; tmp=tm2;
			    }
			}
		    }
		}
		b++;
	    }
	} else {
	    /*
	    ** Convert OTHER (as string)
	    */
	    if((tm2=malloc(sizeof(struct arg)))) {
		tm2->argid=ARGID_OTHER;
		tm2->type=ARGTYPE_STRING;
		tm2->strval=strdup(argv[a]);
		tm2->next=NULL;

		if(tm2) {
		    if(!otherargs) {
			otherargs=otmp=tm2;
		    } else {
			otmp->next=tm2; otmp=tm2;
		    }
		}
	    }
	}
    }

    /*
    ** Put normal args first, then the other args
    */
    if(normargs) {
	result=normargs;
	if(otherargs) {
	    for(tmp=normargs;tmp->next;tmp=tmp->next);
	    tmp->next=otherargs;
	}
    } else {
	if(otherargs) result=otherargs;
    }

return(result);
}



/*****************************************************************************
 ***                                                                       ***
 *** freeargs() - Frees the arglist returned by readargs().                ***
 ***                                                                       ***
 *****************************************************************************/
void freeargs(arglist args)
{
arglist iter,next;

    for(iter=args;iter;iter=next) {
	next=iter->next;
	if(iter->strval) free(iter->strval);
	free(iter);
    }

return;
}




/*
** Test Code
**

#define MYARG_QUIET   0
#define MYARG_VERBOSE 1
#define MYARG_VOLUME  2
#define MYARG_BOOST   3
#define MYARG_EXECUTE 4
#define NUMARGS 5

int main(int argc, char *argv[])
{
arglist        myargs,tmp;
struct rdargs  rdargs[]={{MYARG_QUIET,  ARGTYPE_BOOL,  'q'},
                         {MYARG_VERBOSE,ARGTYPE_BOOL,  'v'},
                         {MYARG_VOLUME, ARGTYPE_INT ,  'i'},
                         {MYARG_BOOST,  ARGTYPE_FLOAT, 'b'},
                         {MYARG_EXECUTE,ARGTYPE_STRING,'e'}};

    if((myargs=readargs(argc,argv,NUMARGS,rdargs))) {
        for(tmp=myargs;tmp;tmp=tmp->next) {
            if(tmp->argid==MYARG_QUIET)
                printf("-q is %s\n",(tmp->boolval ? "TRUE" : "FALSE"));
            if(tmp->argid==MYARG_VERBOSE)
                printf("-v is %s\n",(tmp->boolval ? "TRUE" : "FALSE"));
            if(tmp->argid==MYARG_VOLUME)
                printf("-i is %ld\n",tmp->intval);
            if(tmp->argid==MYARG_BOOST)
                printf("-b is %lf\n",tmp->floatval);
            if(tmp->argid==MYARG_EXECUTE)
                printf("-e is %s\n",tmp->strval);
            if(tmp->argid==ARGID_OTHER)
                printf("other arg: %s\n",tmp->strval);
            if(tmp->argid==ARGID_UNKNOWN)
                printf("unknown option: %s\n",tmp->strval);        
        }
        freeargs(myargs);
    } else {
        printf("Couldn't read args...\n");
    }

return(EXIT_SUCCESS);
}
*/
