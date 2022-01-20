/*****************************************************************************
 ***                                                                       ***
 *** message.c  -  Contains the error/message functions.                   ***
 ***                                                                       ***
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>
#include "include/message.h"
#include "include/global.h"
#include "include/emit.h"
#include "include/emit_protos.h"



void ecdisplaystreampos(inputStream *iS);



/*
** Variables & functions to keep track of verbosity.
*/
static long verbosity=VERBOSITY_NORMAL;

long getverbosity(void)
{
    return(verbosity);
}
void setverbosity(long val)
{
    if(val>=VERBOSITYMIN && val<=VERBOSITYMAX) {
        verbosity=val;
    } else {
        verbosity=VERBOSITY_NORMAL;
    }
}



/*
** Variables & functions to keep track of result print mode.
*/
static long resmode=RESMODE_DECIMAL;

long getresultmode(void)
{
    return(resmode);
}
void setresultmode(long val)
{
    if(val>=RESMODEMIN && val<=RESMODEMAX) {
        resmode=val;
    } else {
        resmode=RESMODE_DECIMAL;
    }
}
long getresultmodefromstring(char *modestr)
{
long mode=RESMODE_UNKNOWN;

    if(modestr && *modestr) {
        if(strcasecmp(modestr,"dec")==0) mode=RESMODE_DECIMAL;
        else if(strcasecmp(modestr,"hex")==0) mode=RESMODE_HEXADECIMAL;
        else if(strcasecmp(modestr,"oct")==0) mode=RESMODE_OCTAL;
        else if(strcasecmp(modestr,"bin")==0) mode=RESMODE_BINARY;
        else if(strcasecmp(modestr,"sci")==0) mode=RESMODE_SCIENTIFIC;
        else if(strcasecmp(modestr,"hms")==0) mode=RESMODE_HHMMSS;
    }
return(mode);
}



/*
** Global error functions
*/
void ecparseerror(inputStream *iS,char *format,...)
{
va_list list;

    emit(ACTION_ERR,NULL,NULL,NULL);
    err=TRUE;

    if(getverbosity()>=VERBOSITY_QUIET) {
        if(iS) {
            ecdisplaystreampos(iS);

            fprintf(stderr,"ERROR: ");
            if(iS->macroname && *(iS->macroname)) {
                fprintf(stderr,"In macro \"%s()\"; ",iS->macroname);
                if(iS->filename && *(iS->filename)) {
                    fprintf(stderr,"called from file \"%s\"; ",iS->filename);
                }
            } else {
                if(iS->filename && *(iS->filename)) {
                    fprintf(stderr,"In file \"%s\"; ",iS->filename);
                }
            }
            //fprintf(stderr,"line %d; ",iS->linenumber);
        }
        va_start(list,format);
        vfprintf(stderr,format,list);
        va_end(list);
    }
}



void ecgeneralerror(char *format, ...)
{
va_list list;

    if(getverbosity()>=VERBOSITY_QUIET) {
        fprintf(stderr,"EC: Error; ");
        
        va_start(list,format);
        vfprintf(stderr,format,list);
        va_end(list);
    }
}



void ecwarning(char *format, ...)
{
va_list list;

    if(getverbosity()>=VERBOSITY_QUIET) {
        fprintf(stderr,"WARNING: ");
        va_start(list,format);
        vprintf(format,list);
        va_end(list);
    }
}



void ecmessage(char *format, ...)
{
va_list list;

    if(getverbosity()>=VERBOSITY_NORMAL) {
        va_start(list,format);
        vprintf(format,list);
        va_end(list);
    }
}



void ecforcedmessage(char *format, ...)
{
va_list list;

    va_start(list,format);
    vprintf(format,list);
    va_end(list);
}



void ecverbose(char *format, ...)
{
va_list list;

    if(getverbosity()>=VERBOSITY_VERBOSE) {
        va_start(list,format);
        vprintf(format,list);
        va_end(list);
    }
}



void ecverboseplus(char *format, ...)
{
va_list list;

    if(getverbosity()>=VERBOSITY_PLUS) {
        va_start(list,format);
        vprintf(format,list);
        va_end(list);
    }
}



void ecprintstring(char *str)
{
    if(getverbosity()>=VERBOSITY_ANS) {
        printf("%s",str);
        fflush(stdout);
    }
}

void ecprintresult(double res)
{
int           hh,mm;
unsigned long resd,a;
double        ss;

    if(getverbosity()>=VERBOSITY_ANS) {
        switch(getresultmode()) {
            case RESMODE_BINARY:
                if(res<0) { printf("-"); res=fabs(res); }
                if(res<=LONG_MAX) {
                    printf("0b");
                    resd=(int)fabs(res);
                    a=ULONG_MAX-LONG_MAX;
                    for(;a && !(resd&a);a>>=1);
                    for(;a;a>>=1) {
                        printf("%d",(resd&a ? 1 : 0));
                    }
                    resd=(unsigned long)((res-(unsigned long)res)*(1<<30));
                    if(resd) {
                        printf(".");
                        for(a=1;a<=30 && (1.0/pow(2,a))>(res-(int)res);a++) { printf("0"); }
                        a=ULONG_MAX-LONG_MAX;
                        for(;a && !(resd&a);a>>=1);
                        for(;a && resd;a>>=1) {
                            printf("%d",(resd&a ? 1 : 0));
                            resd-=resd&a;
                        }
                    }
                    puts("");
                } else {
                    printf("overflow\n");
                }
                break;

            case RESMODE_OCTAL:
                if(res<0) { printf("-"); res=fabs(res); }
                if(res<=LONG_MAX) {
                    printf("0o%lo",(unsigned long)res);
                    resd=(unsigned long)((res-(unsigned long)res)*(1<<30));
                    if(resd) {
                        printf(".");
                        for(a=1;a<=10 && (1.0/pow(8,a))>(res-(int)res);a++) { printf("0"); }
                        while(resd && (resd&0x7)==0) resd>>=3;
                        printf("%lo",resd);
                    }
                    puts("");
                } else {
                    printf("overflow\n");
                }
                break;

            case RESMODE_HEXADECIMAL:
                if(res<0) { printf("-"); res=fabs(res); }
                if(res<=LONG_MAX) {
                    printf("0x%lX",(unsigned long)res);
                    resd=(unsigned long)((res-(unsigned long)res)*(1<<28));
                    if(resd) {
                        printf(".");
                        for(a=1;a<=7 && (1.0/pow(16,a))>(res-(int)res);a++) { printf("0"); }
                        while(resd && (resd&0xF)==0) resd>>=4;
                        printf("%lX",resd);
                    }
                    puts("");
                } else {
                    printf("overflow\n");
                }
                break;

            case RESMODE_HHMMSS:
                if(res<0) { printf("-"); res=fabs(res); }
                if(res<=LONG_MAX) {
                    hh=(int)res;
                    mm=(int)((res-(int)res)*60);
                    ss=(res-abs(hh)-(mm/60.0))*3600;
                    if(ss>59.995) { ss=0; mm++; };
                    if(mm>59) { mm=0; hh++; };
                    printf("%d:%02d:%s%2.2f\n",hh,mm,(ss < 9.995 ? "0" : ""),ss);
                } else {
                    printf("overflow\n");
                }
                break;

            case RESMODE_SCIENTIFIC:
                printf("%e\n",res);
                break;

            case RESMODE_DECIMAL:
            default:
                if(res==0||(res>=0.000001&&res<1e12)||(res<=-0.000001&&res>-1e12)) {
                    printf("%-14f\n",res);
                } else {
                    printf("%e\n",res);
                }
                break;
        }
    }
}



void ecprompt(char *str)
{
    if(getverbosity()>=VERBOSITY_NORMAL) {
        printf("%s",str);
    }
}



#define MAX_SHOWLEN 70
void ecdisplaystreampos(inputStream *iS)
{
int  a,s;

    puts("");
    if(iS->len<=MAX_SHOWLEN) {
        printf("%s\n",iS->buffer);

        for(a=0;a<iS->tstrt;a++) printf(" ");
        for(a=iS->tstrt;a<=iS->tstop;a++) printf("^");
    } else {
#if 0    
        if(iS->tstrt<10) s=0; else s=iS->tstrt-10;

        if(s>0) printf("...");
        for(a=s;a<iS->i+(MAX_SHOWLEN/2) && iS->buffer[a];a++) {
            putchar(iS->buffer[a]);
        }
        if(a<iS->len) printf("...\n"); else puts("");

        if(s>0) printf("   ");
        for(a=s;a<iS->tstrt;a++) printf(" ");
        for(a=MAX(s,iS->tstrt);a<=iS->tstop;a++) printf("^");
#else
        s=(iS->i>(MAX_SHOWLEN) ? iS->i-(MAX_SHOWLEN/2) : 0);

        if(s>0) printf("...");
        for(a=s;a<iS->i+(MAX_SHOWLEN/2) && iS->buffer[a];a++) {
            putchar(iS->buffer[a]);
        }
        if(a<iS->len) printf("...\n"); else puts("");

        if(s>0) printf("   ");
        for(a=s;a<iS->tstrt;a++) printf(" ");
        for(a=MAX(s,iS->tstrt);a<=iS->tstop;a++) printf("^");
#endif
    }
    puts("");
}
