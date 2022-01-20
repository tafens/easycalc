/*****************************************************************************
 ***                                                                       ***
 *** file.c                                                                ***
 ***                                                                       ***
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/global.h"
#include "include/symbol.h"
#include "include/message.h"
#include "include/parse.h"
#include "include/safe_strncpy.h"
#include "include/parse_protos.h"
#include "include/lexer_protos.h"
#include "include/command_protos.h"
#include "include/file_protos.h"
#include "include/message_protos.h"
#include "include/input_protos.h"



/*****************************************************************************
 ***                                                                       ***
 *** read_file()  -  Starts parsing of a file                              ***
 ***                                                                       ***
 *****************************************************************************/
double read_file(char *fname,symTable *sT)
{
FILE   *inf;
long    overbosity;
double  res;

    if(fname) {
        if((inf=fopen(fname,"r"))) {
            ecverbose("Parsing file \"%s\".\n",fname);

            overbosity=getverbosity(); setverbosity(VERBOSITY_QUIET);
            res=parse_file(inf,fname,sT);
            setverbosity(overbosity);

            fclose(inf);
            return(res);
        } else {
            ecgeneralerror("File \"%s\" not found.\n",fname);
        }
    } else {
        ecgeneralerror("read_file(): called with NULL reference\n");
    }

return(0.0);
}



/*****************************************************************************
 ***                                                                       ***
 *** read_rc_file()  -  Starts parsing of the rc file.                     ***
 ***                    Tries several locations.                           ***
 ***                                                                       ***
 *****************************************************************************/
#define RCFILENAME ".ecrc"
void read_rc_file(symTable *sT)
{
FILE   *inf=NULL;
long    overbosity;
double  res;
char   *fname,*homedir;
 
    /*
    ** Try ".ecrc"
    */
    if(!inf) {
        if((fname=strdup(RCFILENAME))) {
            ecverbose("Trying \"%s\"...\n",fname);
            inf=fopen(fname,"r");
        }
    }

    /*
    ** Try "$HOME/.ecrc"
    */
    if(!inf) {
        if((homedir=getenv("HOME"))) {
            if((fname=malloc(strlen(homedir)+1+strlen(RCFILENAME)+1))) {
                strcpy(fname,homedir);
                if(fname[strlen(fname)]!='/') { strcat(fname,"/"); }
                strcat(fname,RCFILENAME);

                ecverbose("Trying \"%s\"...\n",fname);
                inf=fopen(fname,"r");
            }
        }
    }

    /*
    ** Try "/etc/ecrc"
    */
    if(!inf) {
        if((fname=strdup("/etc/ecrc"))) {
            ecverbose("Trying \"%s\"...\n",fname);
            inf=fopen(fname,"r");
        }
    }

    /*
    ** Parse the file
    */
    if(inf) {
        ecverbose("Parsing .rc file \"%s\".\n",fname);

        overbosity=getverbosity(); setverbosity(VERBOSITY_QUIET);
        res=parse_file(inf,fname,sT);
        setverbosity(overbosity);

        fclose(inf);
        free(fname);
    }

return;
}



/*****************************************************************************
 ***                                                                       ***
 *** parse_file()  -  Reads the file and parses every row. Also            ***
 ***                  executes interactive commands.                       ***
 ***                                                                       ***
 *****************************************************************************/

#define MAX_FILEPARSE_DEPTH 16

double parse_file(FILE *inf,char *fname,symTable *sT)
{
inputStream  *iS;
long          running=TRUE;
double        res=0.0;
static int    depth=0;
char         *buf;

    if(depth>MAX_FILEPARSE_DEPTH) {
        depth=0;
        ecgeneralerror("Parsing file: Maximum depth (%d) reached, "
                       "possible recursive loop.\n",MAX_FILEPARSE_DEPTH);
        err=TRUE; return(FALSE);
    }

    if(inf) {
        depth++;
        while(running && (buf=getrow(inf))) {
            if((iS=allocStream("file",fname,"",buf))) {
                if(iS->len>0 && iS->buffer[0]!='#') {
                    if(iS->buffer[0]=='!') {
                        interactive_command(&(iS->buffer[1]),sT,&res,&running);
                    } else {
                        parser_init();
                        res=parse(iS,sT,PARSELEVEL_FILE);
                    }
                }
                freeStream(iS);
            }
            free(buf);

            if(err) running=FALSE;
            if(ecchkquit()) { running=FALSE; }
        }
        depth--;
    } else {
        ecgeneralerror("parse_file(): Invalid FILE handle (NULL)\n");
    }

return(res);
}
