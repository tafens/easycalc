/*****************************************************************************
 ***                                                                       ***
 *** architecture.c  -  Implements functions needed by certain OS:es       ***
 ***                                                                       ***
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#if defined(EC_ARCH_LINUX) || defined(EC_ARCH_OSX)
  #include <unistd.h>
  #ifdef EC_USE_EDITLINE
    #include <histedit.h>
  #endif
  #ifdef EC_USE_GNU_READLINE
    #include <readline/readline.h>
    #include <readline/history.h>
  #endif
#endif
#include "include/global.h"
#include "include/message.h"
#include "include/architecture_protos.h"
#include "include/input_protos.h"
#include "include/message_protos.h"



#if defined(EC_ARCH_LINUX) || defined(EC_ARCH_OSX)

#ifdef EC_USE_EDITLINE
    EditLine *el_state;
    History *el_history;
    HistEvent el_histevent;

    char *el_prompt(EditLine *el_state) {
        return(ECPROMPT);
    }
#endif

/*****************************************************************************
 ***                                                                       ***
 *** For Linux/OSX                                                         ***
 ***                                                                       ***
 *****************************************************************************/
void echandlesig(int x)
{
    if(currIS) {
        ecparseerror(currIS,"BREAK signal sent by user.\n");
    } else {
        ecparseerror(NULL,"\nBREAK\n");
        #ifdef EC_USE_EDITLINE
            el_reset(el_state);
        #endif
        #ifdef EC_USE_GNU_READLINE
            rl_initialize();
            rl_redisplay();
        #endif
    }
    signal(SIGINT,echandlesig);
}

/*
** Code for arhitecture specific init goes here.
*/
void architecture_init(void)
{
    /*
    ** For Linux/OSX (Unix), check if user is running this in interactive shell.
    */
    if(isatty(0) && isatty(1)) {
        ecsetinteractive(TRUE);
        setverbosity(VERBOSITY_NORMAL);
    } else {
        ecsetinteractive(FALSE);
        setverbosity(VERBOSITY_QUIET);
    }

    /*
    ** Catch CTRL-C
    */
    signal(SIGINT,echandlesig);

    /*
    ** Init line editing
    */
    #ifdef EC_USE_EDITLINE
    /* see: http://stackoverflow.com/questions/23577677/saving-inputted-history-in-a-command-line-program-under-capabilty-mode */
        el_state=el_init("ec",stdin,stdout,stderr);
        el_set(el_state,EL_PROMPT,&el_prompt);
        //el_set(el_state,EL_EDITOR,"emacs");
        //el_set(el_state,EL_SIGNAL,1);
        /*
        el_set(el_state,EL_HIST,history,el_history);
        el_source(el_state,NULL);
        history(el_history,&el_histevent,H_SETSIZE,100);
        */
    #endif
    #ifdef EC_USE_GNU_READLINE
        rl_readline_name = "ec";
        rl_variable_bind("blink-matching-paren","1");
    #endif
}
#endif


#ifdef EC_ARCH_AMIGA_DICE
/*****************************************************************************
 ***                                                                       ***
 *** For DICE (Dillon's Integrated C Environment) V2 on Amiga              ***
 ***                                                                       ***
 *****************************************************************************/

/*
** snprintf()
**
** WARNING: this snprintf() will not respect size-constraint.
*/

int snprintf(char *str, size_t size, const  char  *format, ...)
{
int     res;
va_list list;

    va_start(list,format);
    res=vsprintf(str,format,list);
    va_end(list);

return(res);
}


/*
** Code for arhitecture specific init goes here.
*/
void architecture_init(void)
{
}
#endif
