/*****************************************************************************
 ***                                                                       ***
 *** input.c                                                               ***
 ***                                                                       ***
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/global.h"
#include "include/message.h"
#include "include/message_protos.h"
#include "include/input_protos.h"

#ifdef EC_USE_EDITLINE
#include <histedit.h>
#endif
#ifdef EC_USE_GNU_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif



#ifdef EC_USE_EDITLINE
extern EditLine *el_state;

#define HISTFILENAME ".ec_history"
#define MAX_HISTLINES 1024
void loadhistory(void)
{
char *homedir,*fname;
}

void savehistory(void)
{
char *homedir,*fname;
}
#endif


#ifdef EC_USE_GNU_READLINE

#define HISTFILENAME ".ec_history"
#define MAX_HISTLINES 1024
void loadhistory(void)
{
char *homedir,*fname;

    using_history();
    stifle_history(MAX_HISTLINES);
    if((homedir=getenv("HOME"))) {
        if((fname=malloc(strlen(homedir)+1+strlen(HISTFILENAME)+1))) {
            strcpy(fname,homedir);
            if(fname[strlen(fname)]!='/') { strcat(fname,"/"); }
            strcat(fname,HISTFILENAME);

            read_history(fname);
            free(fname);
        }
    }
}

void savehistory(void)
{
char *homedir,*fname;
 
    if((homedir=getenv("HOME"))) {
        if((fname=malloc(strlen(homedir)+1+strlen(HISTFILENAME)+1))) {
            strcpy(fname,homedir);
            if(fname[strlen(fname)]!='/') { strcat(fname,"/"); }
            strcat(fname,HISTFILENAME);

            write_history(fname);
            history_truncate_file(fname,MAX_HISTLINES);

            free(fname);
        }
    }
}
#endif

#if !defined(EC_USE_EDITLINE) && !defined(EC_USE_GNU_READLINE)
void loadhistory(void) {}
void savehistory(void) {}
#endif


/*
** Variables & functions to keep track of if EC is in interactive mode.
*/
static long ecinteractive=TRUE;

void ecsetinteractive(long ia) { ecinteractive=ia; }
long ecgetinteractive(void)    { return(ecinteractive); }



char *ecreadline(char *prompt)
{
#ifdef EC_USE_EDITLINE
  char *buf=NULL;
  int cnt;

  buf=strdup(el_gets(el_state,&cnt));
  cleanup_buffer(buf);
  return(buf);
#endif

#ifdef EC_USE_GNU_READLINE
HISTORY_STATE *histstate;
char *buf=NULL;

    if(ecgetinteractive()) { 
        if(getverbosity()>=VERBOSITY_NORMAL) {
            buf=readline(prompt);
        } else {
            buf=readline(NULL);
        }
        if(buf && *buf) {
            histstate=history_get_history_state();
            
/* segfaults if no history-file..
            if(histstate->length<=0 ||
               strcmp(histstate->entries[histstate->length-1]->line,buf)!=0) {
                add_history(buf);
            }
*/

            if(histstate->length<=0) {
                add_history(buf);
            } else {
                if(strcmp(histstate->entries[histstate->length-1]->line,buf)!=0) {
                    add_history(buf);
                }
            }
            cleanup_buffer(buf);
        }
        return(buf);
    } else {
        return(getrow(stdin));
    }
#endif

#if !defined(EC_USE_EDITLINE) && !defined(EC_USE_GNU_READLINE)
    if(ecgetinteractive()) {
        ecprompt(prompt);
    }
    return(getrow(stdin));
#endif
}



#define BUF_ADD 128      /* Must be larger than 1!          */
#define BUF_MAX 1048576  /* To prevent infinite allocations */
char *getrow(FILE *inf)
{
char *buf=NULL;
long a,c,len;

    if(inf) {
        len=a=c=0; buf=NULL;
        do {
            c=fgetc(inf);
            if(c=='\\') {
                /* allow for use of \ followed by newline to merge this line with the next */
                c=fgetc(inf);
                if(c!='\n' && c!='\r') {
                    ungetc(c,inf); c='\\';
                } else {
                    if((c=='\n' || c=='\r') && c!=EOF) { c=fgetc(inf); }; /* handle row ending by both \n or \r\n */
                    if((c=='\n' || c=='\r') && c!=EOF) { c=fgetc(inf); };
                }
            }
            if(c!=EOF) {
                if(a>=(len-1)) {
                    if(len+BUF_ADD<=BUF_MAX) {
                        if(!(buf=realloc(buf,sizeof(char)*(len+BUF_ADD)))) {
                            while(c!='\n' && c!=EOF) { c=fgetc(inf); };
                            ecgeneralerror("Not enough memory to read input. Truncated.\n");
                            break;
                        }
                        len+=BUF_ADD;
                    } else {
                        while(c!='\n' && c!=EOF) { c=fgetc(inf); };
                        ecgeneralerror("Max input length exceeded. Truncated.\n");
                        break;
                    }
                }
                buf[a++]=c;
            }
        } while(c!='\n' && c!=EOF);

        if(buf) {
            buf[a]='\0';
            
            /*
            ** Shrink buffer to fit
            */
            if(len>(a+1)) {
                buf=realloc(buf,sizeof(char)*(a+1));
            }

            /*
            ** Make sure buffer has no whitespace at beginning nor end.
            */
            cleanup_buffer(buf);
        }
    } else {
        ecgeneralerror("getrow(): called with NULL reference.\n");
    }
    
return(buf);
}



void cleanup_buffer(char *buf)
{
long  a,c,l;
char *bufp;

    if(buf && *buf) {
        if((bufp=strchr(buf,'\n'))) { *bufp='\0'; }
        
        l=strlen(buf);
        a=0; while(buf[a] && isspace(buf[a])) a++;
        if(a==l) {
            buf[0]='\0';                      /* buf is only spaces, ignore. */
        } else {
            if(a>0) {
                for(c=0;buf[a];a++,c++) {
                    buf[c]=buf[a];
                }
                buf[c]='\0';
            } else {
                c=l;
            }
            for(--c;c>0 && isspace(buf[c]);c--) buf[c]='\0';
        }
    }
}
