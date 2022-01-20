/*****************************************************************************
 ***                                                                       ***
 *** EasyCalc - Main                                                       ***
 ***                                                                       ***
 *****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/global.h"
#include "include/symbol.h"
#include "include/message.h"
#include "include/parse.h"
#include "include/safe_strncpy.h"
#include "include/architecture.h"
#include "include/parse_protos.h"
#include "include/symbol_protos.h"
#include "include/lexer_protos.h"
#include "include/macro_protos.h"
#include "include/command_protos.h"
#include "include/file_protos.h"
#include "include/message_protos.h"
#include "include/input_protos.h"
#include "include/architecture_protos.h"
#include "readargs/readargs.h"
#include "readargs/readargs_protos.h"



#define ECARG_QUIET   0
#define ECARG_VERBOSE 1
#define ECARG_HELP    2
#define ECARG_EXECUTE 3
#define ECARG_MODE    4
#define ECARG_ALTRC   5
#define ECARG_NORC    6
#define ECNUMARGS     7



int main(int argc,char *argv[])
{
symTable      *sT;
inputStream   *iS;
double         res;
long           running,doexit,norc,resmode,rcread=FALSE;
char          *buf;
arglist        ecargs,argtmp;
struct rdargs  ecrdargs[]={{ECARG_QUIET,  ARGTYPE_BOOL,  'q'},
                           {ECARG_VERBOSE,ARGTYPE_INT,   'v'},
                           {ECARG_HELP,   ARGTYPE_BOOL,  'h'},
                           {ECARG_EXECUTE,ARGTYPE_STRING,'e'},
                           {ECARG_MODE,   ARGTYPE_STRING,'m'},
                           {ECARG_ALTRC,  ARGTYPE_STRING,'c'},
                           {ECARG_NORC,   ARGTYPE_BOOL,  'n'}};

    /*
    ** Init calculator
    */
    architecture_init();
    sT=initsym(NULL);
    initmacros(sT);


    /*
    ** Parse the args
    */
    doexit=FALSE; norc=FALSE;
    if((ecargs=readargs(argc,argv,ECNUMARGS,ecrdargs))) {
        for(argtmp=ecargs;argtmp;argtmp=argtmp->next) {

            if(argtmp->argid==ECARG_QUIET)   setverbosity(VERBOSITY_QUIET);
            if(argtmp->argid==ECARG_VERBOSE) {
                setverbosity(argtmp->intval);
            }
            if(argtmp->argid==ECARG_MODE) {
                if(argtmp->strval) {
                    if((resmode=getresultmodefromstring(argtmp->strval))) {
                        setresultmode(resmode);
                    } else {
                        ecgeneralerror("Output mode \"%s\" unknown, setting decimal.\n", argtmp->strval);
                    }
                } else {
                    ecgeneralerror("Output mode not specified for -m.\n");
                }
            }

            if(argtmp->argid==ECARG_ALTRC) {
                if(argtmp->strval) {
                    read_file(argtmp->strval,sT);
                    rcread=TRUE;
                } else {
                    ecgeneralerror("No rc-file specified for -c.\n");
                }
            }
            if(argtmp->argid==ECARG_NORC) {
                norc=TRUE;
            }

            if(argtmp->argid==ECARG_HELP || argtmp->argid==ARGID_UNKNOWN) {
                if(argtmp->argid==ARGID_UNKNOWN && argtmp->strval) {
                    ecgeneralerror("Unknown option: %s\n\n",argtmp->strval);
                }
                ecforcedmessage(ECVERSION "\n"
                        "Usage:\n"
                        "ec [-qnh] [-v -3..2] [-m mode] [-c rcfile] [[-e expression] ...] [file ...]\n"
                        "\n"
                        "-q   Quiet.\n"
                        "-n   Do not read .ecrc resource file.\n"
                        "-h   This help.\n"
                        "-v   Verbose (-3=nothing,-2,-1,default 0,1,2=all).\n"
                        "-m   Output mode (default dec=decimal, hex=hexadecimal, oct=octal, bin=binary,\n"
                        "                  sci=scientific, hms=HH:MM:SS.SS)\n"
                        "-c   Read specified file instead of .ecrc resource file.\n"
                        "-e   Calculate [expression] quietly then exit.\n"
                        "\n"
                        "If [file] is given, parse file quietly and exit.\n"
                        "Options are acted upon in the order they are specified,\n"
                        "except that files are always parsed after all other options.\n"
                        "\n");
                exit(EXIT_SUCCESS);
            }

            /*
            ** Execute a calculation after the "-e" flag and exit;
            ** "One-shot calculation"
            */
            if(argtmp->argid==ECARG_EXECUTE) {
                ecsetinteractive(FALSE);
                doexit=TRUE;
                if(!norc && !rcread) { read_rc_file(sT); rcread=TRUE; }
                if(argtmp->strval) {
                    setverbosity(VERBOSITY_QUIET);

                    if((buf=strdup(argtmp->strval))) {
                        cleanup_buffer(buf);
                        if((iS=allocStream("one-shot","","",buf))) {
                            parser_init();
                            res=parse(iS,sT,PARSELEVEL_FILE);
                            if(!err) ecprintresult(res);

                            freeStream(iS);
                        }
                        free(buf);
                    }
                }
            }

            /*
            ** If a file name is specified, parse the file.
            */
            if(argtmp->argid==ARGID_OTHER) {
                ecsetinteractive(FALSE);
                doexit=TRUE;
                if(!norc && !rcread) { read_rc_file(sT); rcread=TRUE; }
                if(argtmp->strval) {
                    setverbosity(VERBOSITY_QUIET);
                    res=read_file(argtmp->strval,sT);
                    if(!err) ecprintresult(res);
                }
            }
        }
        freeargs(ecargs);
    }

    if(!doexit) {
        ecmessage("\n" ECVERSION "\n");

        /*
        ** Print options used
        */
        if(getverbosity()>=VERBOSITY_VERBOSE) {
            ecmessage("Verbosity:     %s\n",
                      (getverbosity()>=VERBOSITY_PLUS ? "Verbose plus" : "Verbose"));
            ecmessage("Output mode:   %s\n",
                      (getresultmode()==RESMODE_DECIMAL ? "Decimal" :
                       (getresultmode()==RESMODE_SCIENTIFIC ? "Scientific" :
                        (getresultmode()==RESMODE_HHMMSS ? "HH:MM:SS.SS" :
                         (getresultmode()==RESMODE_HEXADECIMAL ? "Hexadecimal" :
                          (getresultmode()==RESMODE_OCTAL ? "Octal" :
                           (getresultmode()==RESMODE_BINARY ? "Binary" :
                            "unknown")))))));
            ecmessage("Read .rc-file: %s\n",(norc ? "No" : "Yes"));
            ecmessage("\n");
        }
        
        /*
        ** Read the calculation/command history from file
        */
        ecverbose("Loading history...\n");
        loadhistory();

        /*
        ** Read .rc-file
        */
        if(!norc && !rcread) { read_rc_file(sT); rcread=TRUE; }

        /*
        ** Main-loop
        */
        ecmessage("Type !help or !h for help, !license to see license.\n\n");

        running=TRUE; err=FALSE;

        while(running) {
            if((buf=ecreadline(ECPROMPT))) {
                err=FALSE;

                if((iS=allocStream("interactive","","",buf))) {
                    if(iS->len>0 && iS->buffer[0]!='#') {
                        if(iS->buffer[0]=='!') {
                            if(interactive_command(&(iS->buffer[1]),sT,&res,&running)) {
                                if(!err) {
                                    setsymbol(sT,TYPE_VAR,"ans",res,"",-1);
                                    ecverbose("");
                                    ecmessage("ans = ");
                                    ecprintresult(res);
                                }
                            }
                        } else {
                            parser_init();
                            res=parse(iS,sT,PARSELEVEL_INTERACTIVE);
                            if(!err) {
                                setsymbol(sT,TYPE_VAR,"ans",res,"",-1);
                                ecverbose("");
                                ecmessage("ans = ");
                                ecprintresult(res);
                            }
                        }   
                    }
                    freeStream(iS);
                }
                free(buf);
            } else {
                ecmessage("\n");
                running=FALSE;
            }
        }

        /*
        ** Save the history to file
        */
        savehistory();
    }

    deletesym(sT);

#ifdef MEMAUDIT
    macheck();
#endif

exit(EXIT_SUCCESS);
}
