/*****************************************************************************
 ***                                                                       ***
 *** command.c                                                             ***
 ***                                                                       ***
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/global.h"
#include "include/emit.h"
#include "include/symbol.h"
#include "include/message.h"
#include "include/architecture.h"
#include "include/parse_protos.h"
#include "include/emit_protos.h"
#include "include/symbol_protos.h"
#include "include/file_protos.h"
#include "include/message_protos.h"



long interactive_command(char *cmd,symTable *sT, double *res, long *running)
{
double tmp;
long   i,j,type,nparams;
char   id[MAX_IDENT_LEN],*p;


    if(strcasecmp(cmd,"c")==0 || strcasecmp(cmd,"com")==0 ||
       strcasecmp(cmd,"comms")==0 || strcasecmp(cmd,"commands")==0) {
        ecmessage(
            "+--------------------+------------------+-------------------------------------+\n"
            "| Command            | Equivalent       | Description                         |\n"
            "+--------------------+------------------+-------------------------------------+\n"
            "| !help              | !h               | this help text                      |\n"
            "| !commands          | !com,!c          | print this list                     |\n"
            "| !functions         | !func,!f         | print internal functions            |\n"
            "| !macros            | !mac,!m          | print macros                        |\n"
            "| !variables         | !var,!v          | print variables                     |\n"
            "| !unset             | !u               | unset a macro or variable           |\n"
            "| !parse             | !p               | parse a file                        |\n"
            "| !quit              | !exit, !bye, !q  | exit calculator                     |\n"
            "| !version           | !ver             | version information                 |\n"
            "| !mode              | !md              | output mode:                        |\n"
            "|                    |                  | dec,hex,oct,bin,sci,hms             |\n"
            "+--------------------+------------------+-------------------------------------+\n"
            "| Debug/Learning                                                              |\n"
            "+--------------------+------------------+-------------------------------------+\n"
            "| !clearstack        | !cs              | clear stack                         |\n"
            "| !printstack        | !ps              | stack dump                          |\n"
            "| !dryrun            | !dry             | no-action parsing                   |\n"
            "| !verbose           | !vb              | verbose mode (-3..2)                |\n"
            "+--------------------+------------------+-------------------------------------+\n"
            "\n");
        return(FALSE);
    }


    if(strcasecmp(cmd,"f")==0 || strcasecmp(cmd,"func")==0 ||
       strcasecmp(cmd,"funcs")==0 || strcasecmp(cmd,"functions")==0) {
        ecmessage(
            "+-----------------+-----------------------------------------------------------+\n"
            "| Function        |  Description                                              |\n"
            "+-----------------+-----------------------------------------------------------+\n"
            "| asin(x), sin(x) | (arc) sine of x    (radians).                             |\n"
            "| acos(x), cos(x) | (arc) cosine of x  (radians).                             |\n"
            "| atan(x), tan(x) | (arc) tangent of x (radians).                             |\n"
            "| sqr(x), sqrt(x) | square root of x.                                         |\n"
            "| lg(x)           | log x of base 10.                                         |\n"
            "| ln(x)           | log x of base e.                                          |\n"
            "| log(y,x)        | log x of base y.                                          |\n"
            "| int(x),trunc(x) | integer part of x.                                        |\n"
            "| frac(x)         | fractional part of x.                                     |\n"
            "| round(x)        | round x to closest integer.                               |\n"
            "| ceil(x)         | closest integer larger or equal to x.                     |\n"
            "| floor(x)        | closest integer lower or equal to x.                      |\n"
            "| rnd(x), rand(x) | random number between 0 and x (inclusive).                |\n"
            "| tod()           | time of day.                                              |\n"
            "| etime()         | elapsed computing time. etime(0) resets counter.          |\n"
            "| min(...)        | returns minimum of all parameters passed.                 |\n"
            "| max(...)        | returns maximum of all parameters passed.                 |\n"
            "| avg(...)        | returns average of all parameters passed.                 |\n"
            "| med(...)        | returns median of all parameters passed.                  |\n"
            "| print(...)      | each parameter passed will be printed on a                |\n"
            "|                 | separate row. If the variable _printmode is               |\n"
            "|                 | set, the values will be in that format:                   |\n"
            "|                 | %d=dec, %d=hex, %d=oct, %d=bin, %d=sci, %d=hms                  |\n"
            "+-----------------+-----------------------------------------------------------+\n"
            "\n",
            RESMODE_DECIMAL,RESMODE_HEXADECIMAL,RESMODE_OCTAL,
            RESMODE_BINARY,RESMODE_SCIENTIFIC,RESMODE_HHMMSS);
        return(FALSE);
    }


    if(strcasecmp(cmd,"cf")==0 || strcasecmp(cmd,"config")==0) {
        printconfigvars(sT);
        return(FALSE);
    }


    if(strcasecmp(cmd,"h")==0 || strcasecmp(cmd,"help")==0 || strcasecmp(cmd,"?")==0) {
        ecmessage(
            "+-----------------------------------------------------------------------------+\n"
            "| EasyCalc is a command line calculator that can handle                       |\n"
            "| complex expressions containing:                                             |\n"
            "|                                                                             |\n"
            "| addition (+), subtraction (-), multiplication (*), division (/),            |\n"
            "| negation (-),  modulus (%%), power of (^), faculty (!),                      |\n"
            "| comparisons (equal ==, not equal !=, larger >, lesser <,                    |\n"
            "| larger or equal >=, lesser or equal <=), logical operators (and &&,         |\n"
            "| or ||, not !), parantheses, variables, mathematical functions and macros.   |\n"
            "|                                                                             |\n"
            "| Several expressions can be chained together with a semicolon (;) and the    |\n"
            "| value will be equal to the last expression in the chain.                    |\n"
            "|                                                                             |\n"
            "| Program flow can be controlled by the statements                            |\n"
            "| while(expr,statements), if(expression,true-statements,false-statements),    |\n"
            "| break, return and quit.                                                     |\n"
            "|                                                                             |\n"
            "| For a list of...                                                            |\n"
            "|   ...commands,           type \"!commands\" or !c                             |\n"
            "|   ...internal functions, type \"!funcs\"    or !f                             |\n"
            "|   ...config variables,   type \"!config\"   or !cf                            |\n"
            "|   ...defined macros,     type \"!macros\"   or !m                             |\n"
            "|   ...set variables,      type \"!vars\"     or !v                             |\n"
            "|                                                                             |\n"
            "| To change output mode to...                                                 |\n"
            "|   ...Decimal,            type \"!mode dec\"                                   |\n"
            "|   ...Hexadecimal,        type \"!mode hex\"                                   |\n"
            "|   ...Octal,              type \"!mode oct\"                                   |\n"
            "|   ...Binary,             type \"!mode bin\"                                   |\n"
            "|   ...Scientific,         type \"!mode sci\"                                   |\n"
            "|   ...HH:MM:SS.SS,        type \"!mode hms\"                                   |\n"
            "+-----------------------------------------------------------------------------+\n");
        return(FALSE);
    }


    if(strcasecmp(cmd,"l")==0 || strcasecmp(cmd,"lic")==0 ||
       strcasecmp(cmd,"license")==0) {
        ecmessage(ECVERSION);
        return(FALSE);
    }


    if(strcasecmp(cmd,"m")==0 || strcasecmp(cmd,"mac")==0 ||
       strcasecmp(cmd,"macros")==0 || strcasecmp(cmd,"macs")==0) {
        printmacros(sT);
        return(FALSE);
    }


    if(strncasecmp(cmd,"md",2)==0 || strncasecmp(cmd,"mode",4)==0) {
        p=cmd; i=j=0;
        while(*p!=' ' && *p) p++; while(*p && isspace(*p)) p++;
        while(*p && *p!=' ' && i<(MAX_IDENT_LEN-1)) id[i++]=*p++;
        id[i]='\0';

        if(id && *id) {
            if((i=getresultmodefromstring(id))) {
                setresultmode(i);
            } else {
                setresultmode(RESMODE_DECIMAL);
                ecgeneralerror("Requested output mode unknown, using decimal.\n");
            }
        } else {
            i=getresultmode();
            ecmessage("Output mode: %s\nAvailable modes: dec,hex,oct,bin,sci,hms\n",
                      (i==RESMODE_DECIMAL ? "Decimal" :
                       (i==RESMODE_SCIENTIFIC ? "Scientific" :
                        (i==RESMODE_HHMMSS ? "HH:MM:SS.SS" :
                         (i==RESMODE_HEXADECIMAL ? "Hexadecimal" :
                          (i==RESMODE_OCTAL ? "Octal" :
                           (i==RESMODE_BINARY ? "Binary" :
                            "Unknown")))))));
        }
        return(FALSE);
    }


    if(strcasecmp(cmd,"p")==0 || strcasecmp(cmd,"p ")==0) {
        ecgeneralerror("No file specified.\n");
        return(FALSE);
    }
    if(strncasecmp(cmd,"p ",2)==0 || strncasecmp(cmd,"parse ",6)==0) {
        p=cmd; i=j=0;
        while(*p!=' ' && *p) p++; while(*p && isspace(*p)) p++;
        while(*p && *p!=' ' && i<(MAX_IDENT_LEN-1)) id[i++]=*p++;
        id[i]='\0';

        if(id && *id) {
            *res=read_file(id,sT);
            return(TRUE);
        } else {
            ecgeneralerror("Missing filename.\n");
            return(FALSE);
        }
    }


    if(strcasecmp(cmd,"q")==0 || strcasecmp(cmd,"quit")==0 ||
       strcasecmp(cmd,"exit")==0 || strcasecmp(cmd,"bye")==0) {
        if(strcasecmp(cmd,"bye")==0) {
            ecmessage("Goodbye.\n");
        }
        *running=FALSE;
        return(FALSE);
    }


    if(strcasecmp(cmd,"s")==0 || strcasecmp(cmd,"sh")==0 || strcasecmp(cmd,"shexec")==0) {
        ecgeneralerror("No program specified.\n");
        return(FALSE);
    }
    if(strncasecmp(cmd,"s",1)==0 || strncasecmp(cmd,"sh",2)==0 ||
       strncasecmp(cmd,"shexec",6)==0) {
        p=cmd; i=j=0;
        while(*p!=' ' && *p) p++; while(*p && isspace(*p)) p++;
        while(*p && *p!=' ' && i<(MAX_IDENT_LEN-1)) id[i++]=*p++;
        id[i]='\0';

        if(id && *id) {
            ecgeneralerror("ShExec \"%s\", Not implemented.\n",id);
        }

        *res=0.0;
        return(TRUE);
    }


    if(strcasecmp(cmd,"u")==0 || strcasecmp(cmd,"u ")==0) {
        ecgeneralerror("No macro or variable specified.\n");
        return(FALSE);
    }
    if(strncasecmp(cmd,"u ",2)==0 || strncasecmp(cmd,"unset ",6)==0) {
        p=cmd; i=j=0;
        while(*p!=' ' && *p) p++; while(*p && isspace(*p)) p++;
        while(*p && *p!=' ' && i<(MAX_IDENT_LEN-1)) id[i++]=*p++;
        id[i]='\0';

        if(id && *id) {
            if(lookupmacro(sT,id,&type,&nparams,FALSE)) {
                deletemacro(sT,id);
                ecmessage("%s() now unset.\n",id);
            } else {
                if(lookupvar(sT,id,&tmp,FALSE)) {
                    deletevar(sT,id);
                    ecmessage("%s now unset.\n",id);
                } else {
                    ecgeneralerror("%s is not a macro or variable.\n",id);
                }
            }
        } else {
            ecgeneralerror("Missing identifier.\n");
        }

        return(FALSE);
    }


    if(strcasecmp(cmd,"v")==0 || strcasecmp(cmd,"var")==0 ||
       strcasecmp(cmd,"vars")==0 || strcasecmp(cmd,"variables")==0) {
        printvars(sT);
        return(FALSE);
    }


    if(strcasecmp(cmd,"ver")==0 || strcasecmp(cmd,"version")==0) {
        ecmessage(ECVERSION);
        return(FALSE);
    }


    /*
    ** Commands for Debug/Learning
    */
    if(strcasecmp(cmd,"cs")==0 || strcasecmp(cmd,"clearstack")==0) {
        emit(ACTION_CLEARSTACK,NULL,NULL,NULL);
        *res=0.0;
        return(TRUE);
    }


    if(strcasecmp(cmd,"ps")==0 || strcasecmp(cmd,"printstack")==0) {
        emit(ACTION_PRINTSTACK,NULL,NULL,NULL);
        return(FALSE);
    }


    if(strcasecmp(cmd,"vb")==0 || strcasecmp(cmd,"verbose")==0) {
        setverbosity(VERBOSITY_VERBOSE);
        return(FALSE);
    }
    if(strncasecmp(cmd,"vb ",3)==0 || strncasecmp(cmd,"verbose ",8)==0) {
        p=cmd; i=j=0;
        while(*p!=' ' && *p) p++; while(*p && isspace(*p)) p++;

        setverbosity(strtol(p,NULL,10));
        return(FALSE);
    }


    if(strcasecmp(cmd,"dry")==0 || strcasecmp(cmd,"dryrun")==0) {
        ecmessage("No-action parsing now ");
        if(eccheckdryparse()) {
            eccleardryparse();
            ecmessage("disabled.\n");
        } else {
            ecenabledryparse();
            ecmessage("enabled.\n");
        }
        return(FALSE);
    }


    ecgeneralerror("Unknown interactive command. Try \"!commands\" or \"!help\".\n");
    return(FALSE);
}
