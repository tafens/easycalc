/*****************************************************************************
 ***                                                                       ***
 *** lexer.c  -  Lexical analysator for EasyCalc                           ***
 ***                                                                       ***
 ***             This is the magic that sorts out all the sweet little     ***
 ***             tokens from the big nasty text.                           ***
 ***                                                                       ***
 *****************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "include/global.h"
#include "include/tokens.h"
#include "include/safe_strncpy.h"
#include "include/message_protos.h"

#define ISDIGIT(x)    ((x)>='0' && (x)<='9')
#define ISALPHA(x)    (((x)>='a' && (x)<='z') || ((x)>='A' && (x)<='Z'))
#define ISLOWER(x)    ((x)>='a' && (x)<='z')
#define ISUPPER(x)    ((x)>='A' && (x)<='Z')
#define ISSPACE(x)    ((x)==' ' || (x)=='\t'|| (x)=='\n')



int getnumber(double *num,inputStream *iS);



/*****************************************************************************
 ***                                                                       ***
 *** allocStream()  -  Allocates memory and inits an empty inputsStream    ***
 ***                   with an empty currentToken.                         ***
 *** freeStream()   -  Frees the inputStream.                              ***
 ***                                                                       ***
 *****************************************************************************/
inputStream *allocStream(char *name,char *filename,char *macroname,char *buf)
{
inputStream  *iS;
currentToken *cT;

    iS=NULL; cT=NULL;
    if((iS=malloc(sizeof(inputStream)))) {
        iS->name[0]='\0'; iS->filename[0]='\0'; iS->macroname[0]='\0';
        if(name!=NULL) safe_strncpy(iS->name,name,MAX_IDENT_LEN);
        if(filename!=NULL) safe_strncpy(iS->filename,filename,MAX_IDENT_LEN);
        if(macroname!=NULL) safe_strncpy(iS->macroname,macroname,MAX_IDENT_LEN);

        iS->len=strlen(buf); iS->buffer=buf; iS->i=0; iS->tstrt=iS->tstop=0;
        iS->cT=NULL;

        if((cT=malloc(sizeof(currentToken)))) {
            cT->token=TOKEN_NONE;
            cT->value=0.0; cT->id[0]='\0'; cT->macrodef[0]='\0';
            iS->cT=cT;
        } else {
            free(iS); iS=NULL;
        }
    }

    return(iS);
}

void freeStream(inputStream *iS)
{
    if(iS) {
        if(iS->cT) { free(iS->cT); }
        free(iS);
    }
}



/*****************************************************************************
 ***                                                                       ***
 *** tokenToStr()  -  Converts a token to its string value or description  ***
 ***                  (mainly used for error messages)                     ***
 ***                                                                       ***
 *****************************************************************************/
void tokenToStr(long token, char *buf, int buflen)
{
    switch(token) {
        case TOKEN_NONE:
            safe_strncpy(buf,"<none>",buflen);
            break;
        case TOKEN_ADD:
            safe_strncpy(buf,"+",buflen);
            break;
        case TOKEN_SUB:
            safe_strncpy(buf,"-",buflen);
            break;
        case TOKEN_MUL:
            safe_strncpy(buf,"*",buflen);
            break;
        case TOKEN_DIV:
            safe_strncpy(buf,"/",buflen);
            break;
        case TOKEN_MOD:
            safe_strncpy(buf,"%",buflen);
            break;
        case TOKEN_POW:
            safe_strncpy(buf,"^",buflen);
            break;
        case TOKEN_EXCL:
            safe_strncpy(buf,"!",buflen);
            break;
        case TOKEN_QUESTION:
            safe_strncpy(buf,"?",buflen);
            break;
        case TOKEN_LPAR:
            safe_strncpy(buf,"(",buflen);
            break;
        case TOKEN_RPAR:
            safe_strncpy(buf,")",buflen);
            break;
        case TOKEN_COMMA:
            safe_strncpy(buf,",",buflen);
            break;
        case TOKEN_SEMI:
            safe_strncpy(buf,";",buflen);
            break;
        case TOKEN_COLON:
            safe_strncpy(buf,":",buflen);
            break;
        case TOKEN_ASSIGN:
            safe_strncpy(buf,"=",buflen);
            break;
        case TOKEN_AND:
            safe_strncpy(buf,"and",buflen);
            break;
        case TOKEN_ANDAND:
            safe_strncpy(buf,"&&",buflen);
            break;
        case TOKEN_OR:
            safe_strncpy(buf,"or",buflen);
            break;
        case TOKEN_OROR:
            safe_strncpy(buf,"||",buflen);
            break;
        case TOKEN_NOT:
            safe_strncpy(buf,"not",buflen);
            break;
        case TOKEN_CMP_EQ:
            safe_strncpy(buf,"==",buflen);
            break;
        case TOKEN_CMP_NE:
            safe_strncpy(buf,"!=",buflen);
            break;
        case TOKEN_CMP_GE:
            safe_strncpy(buf,">=",buflen);
            break;
        case TOKEN_CMP_LE:
            safe_strncpy(buf,"<=",buflen);
            break;
        case TOKEN_CMP_GT:
            safe_strncpy(buf,">",buflen);
            break;
        case TOKEN_CMP_LT:
            safe_strncpy(buf,"<",buflen);
            break;
        case TOKEN_NUM:
            safe_strncpy(buf,"<number>",buflen);
            break;
        case TOKEN_ID:
            safe_strncpy(buf,"<identifier>",buflen);
            break;
        case TOKEN_STR:
            safe_strncpy(buf,"<string>",buflen);
            break;
        case TOKEN_IF:
            safe_strncpy(buf,"if",buflen);
            break;
        case TOKEN_COND:
            safe_strncpy(buf,"cond",buflen);
            break;
        case TOKEN_WHILE:
            safe_strncpy(buf,"while",buflen);
            break;
        case TOKEN_INPUT:
            safe_strncpy(buf,"input",buflen);
            break;
        case TOKEN_RETURN:
            safe_strncpy(buf,"return",buflen);
            break;
        case TOKEN_BREAK:
            safe_strncpy(buf,"break",buflen);
            break;
        case TOKEN_QUIT:
            safe_strncpy(buf,"quit",buflen);
            break;
        case TOKEN_MDEF:
            safe_strncpy(buf,"<macrodef>",buflen);
            break;
        case TOKEN_END:
            safe_strncpy(buf,"<end of expression>",buflen);
            break;
        case TOKEN_ERR:
            safe_strncpy(buf,"<error>",buflen);
            break;
        default:
            safe_strncpy(buf,"<unknown>",buflen);
            break;
    }
}



/*****************************************************************************
 ***                                                                       ***
 *** getNextToken()  -  Fetches the next token from the input stream.      ***
 ***                                                                       ***
 *****************************************************************************/
int getNextToken(inputStream *iS)
{
double        number=0,minutes=0,seconds=0;
long          j=0,bracketcount=0,lexerror=0;
currentToken *cT;

    if(!iS || !(iS->cT)) return(FALSE);

    /*
    ** Set default values
    */
    cT=iS->cT; 
    cT->token=TOKEN_ERR;
    cT->value=0.0;
    cT->id[0]=cT->macrodef[0]='\0';

    /*
    ** Skip white space and/or detect end of stream
    */
    while(ISSPACE(iS->buffer[iS->i]) && iS->i!=iS->len) {
        iS->i++;
    }
    if(iS->i==iS->len || iS->buffer==NULL) {
        cT->token=TOKEN_END;
        return(TRUE);
    }

    /*
    ** Find tokens!
    */
    iS->tstrt=iS->tstop=iS->i;

    /*
    ** Two-character tokens...
    */
    if(iS->i<(iS->len-1)) {
        if(iS->buffer[iS->i]=='&' && iS->buffer[iS->i+1]=='&') {iS->i+=2; cT->token=TOKEN_ANDAND; return(TRUE);}
        if(iS->buffer[iS->i]=='|' && iS->buffer[iS->i+1]=='|') {iS->i+=2; cT->token=TOKEN_OROR;   return(TRUE);}
        if(iS->buffer[iS->i]=='=' && iS->buffer[iS->i+1]=='=') {iS->i+=2; cT->token=TOKEN_CMP_EQ; return(TRUE);}
        if(iS->buffer[iS->i]=='!' && iS->buffer[iS->i+1]=='=') {iS->i+=2; cT->token=TOKEN_CMP_NE; return(TRUE);}
        if(iS->buffer[iS->i]=='>' && iS->buffer[iS->i+1]=='=') {iS->i+=2; cT->token=TOKEN_CMP_GE; return(TRUE);}
        if(iS->buffer[iS->i]=='<' && iS->buffer[iS->i+1]=='=') {iS->i+=2; cT->token=TOKEN_CMP_LE; return(TRUE);}
    }

    /*
    ** One-character tokens...
    */
    if(iS->buffer[iS->i]=='+') {iS->i++; cT->token=TOKEN_ADD;      return(TRUE);}
    if(iS->buffer[iS->i]=='-') {iS->i++; cT->token=TOKEN_SUB;      return(TRUE);}
    if(iS->buffer[iS->i]=='*') {iS->i++; cT->token=TOKEN_MUL;      return(TRUE);}
    if(iS->buffer[iS->i]=='/') {iS->i++; cT->token=TOKEN_DIV;      return(TRUE);}
    if(iS->buffer[iS->i]=='%') {iS->i++; cT->token=TOKEN_MOD;      return(TRUE);}
    if(iS->buffer[iS->i]=='^') {iS->i++; cT->token=TOKEN_POW;      return(TRUE);}
    if(iS->buffer[iS->i]=='!') {iS->i++; cT->token=TOKEN_EXCL;     return(TRUE);}
    if(iS->buffer[iS->i]=='?') {iS->i++; cT->token=TOKEN_QUESTION; return(TRUE);}
    if(iS->buffer[iS->i]=='(') {iS->i++; cT->token=TOKEN_LPAR;     return(TRUE);}
    if(iS->buffer[iS->i]==')') {iS->i++; cT->token=TOKEN_RPAR;     return(TRUE);}
    if(iS->buffer[iS->i]==',') {iS->i++; cT->token=TOKEN_COMMA;    return(TRUE);}
    if(iS->buffer[iS->i]==';') {iS->i++; cT->token=TOKEN_SEMI;     return(TRUE);}
    /* interferes with hours:mins:secs
    if(iS->buffer[iS->i]==':') {iS->i++; cT->token=TOKEN_COLON;    return(TRUE);}
    */
    if(iS->buffer[iS->i]=='=') {iS->i++; cT->token=TOKEN_ASSIGN;   return(TRUE);}
    if(iS->buffer[iS->i]=='>') {iS->i++; cT->token=TOKEN_CMP_GT;   return(TRUE);}
    if(iS->buffer[iS->i]=='<') {iS->i++; cT->token=TOKEN_CMP_LT;   return(TRUE);}
    if(iS->buffer[iS->i]=='#') {iS->i++; cT->token=TOKEN_END;      return(TRUE);}

    /*
    ** Number
    **
    ** (0-9)*.(0-9)+ union (0-9)+.(0-9)* union (0-9)+
    **               union (0-9)*.(0-9)*e(0-9)*
    **
    */
    if(iS->buffer[iS->i]=='.' || iS->buffer[iS->i]==':' || ISDIGIT(iS->buffer[iS->i])) {
        getnumber(&number,iS);
        if(iS->buffer[iS->i]==':') {   /* HH:MM:SS.XX */
            iS->i++;
            getnumber(&minutes,iS);
            if(iS->buffer[iS->i]==':') {
                iS->i++;
                getnumber(&seconds,iS);
            }
            number+=minutes/60+seconds/3600;
        }
        iS->tstop=iS->i-1;

        cT->token=TOKEN_NUM;
        cT->value=number;
        return(TRUE);
    }
    
    /*
    ** Identifier or word-token
    **
    ** (@,$,_,A-Za-z)(A-Za-z,_,0-9)*
    **
    ** and, or, not, cond, while, input, return, break, quit
    */
    if(ISALPHA(iS->buffer[iS->i]) ||
       iS->buffer[iS->i]=='_' || iS->buffer[iS->i]=='$' || iS->buffer[iS->i]=='@') {
        j=0;
        if(iS->buffer[iS->i]=='$' || iS->buffer[iS->i]=='@' ||
           iS->buffer[iS->i]=='_' || ISALPHA(iS->buffer[iS->i])) {
            cT->id[j]=iS->buffer[iS->i];
            iS->i++; j++;
        }
        while((iS->buffer[iS->i]=='_' ||
               ISDIGIT(iS->buffer[iS->i]) || ISALPHA(iS->buffer[iS->i]))) {
            if(j<MAX_IDENT_LEN) {
                cT->id[j]=iS->buffer[iS->i];
                if(j<(MAX_IDENT_LEN-1)) j++;
            }
            iS->i++;
        }
        iS->tstop=iS->i-1;

        cT->id[j]='\0';
        cT->token=TOKEN_ID;

        /*
        ** Check for word-tokens
        */
        if(strcasecmp(cT->id,"and")==0)      { cT->id[0]='\0'; cT->token=TOKEN_AND;      }
        if(strcasecmp(cT->id,"or")==0)       { cT->id[0]='\0'; cT->token=TOKEN_OR;       }
        if(strcasecmp(cT->id,"not")==0)      { cT->id[0]='\0'; cT->token=TOKEN_NOT;      }
        if(strcasecmp(cT->id,"true")==0)     { cT->id[0]='\0'; cT->token=TOKEN_TRUE;     }
        if(strcasecmp(cT->id,"false")==0)    { cT->id[0]='\0'; cT->token=TOKEN_FALSE;    }
        if(strcasecmp(cT->id,"infinity")==0) { cT->id[0]='\0'; cT->token=TOKEN_INFINITY; }
        if(strcasecmp(cT->id,"if")==0)       { cT->id[0]='\0'; cT->token=TOKEN_IF;       }
        if(strcasecmp(cT->id,"cond")==0)     { cT->id[0]='\0'; cT->token=TOKEN_COND;     }
        if(strcasecmp(cT->id,"while")==0)    { cT->id[0]='\0'; cT->token=TOKEN_WHILE;    }
        if(strcasecmp(cT->id,"input")==0)    { cT->id[0]='\0'; cT->token=TOKEN_INPUT;    }
        if(strcasecmp(cT->id,"return")==0)   { cT->id[0]='\0'; cT->token=TOKEN_RETURN;   }
        if(strcasecmp(cT->id,"break")==0)    { cT->id[0]='\0'; cT->token=TOKEN_BREAK;    }
        if(strcasecmp(cT->id,"quit")==0)     { cT->id[0]='\0'; cT->token=TOKEN_QUIT;     }

        return(TRUE);
    }


    /*
    ** Macro definition
    **
    ** {.*}
    */
    if(iS->buffer[iS->i]=='{') {
        j=0; bracketcount=1; iS->i++;
        while(bracketcount>0 && iS->buffer[iS->i]!='\0') {
            if(iS->buffer[iS->i]=='{') bracketcount++;
            if(iS->buffer[iS->i]=='}') bracketcount--;
            if(j<MAX_MACRO_LEN && bracketcount>0) {
                cT->macrodef[j]=iS->buffer[iS->i];
                if(j<(MAX_MACRO_LEN-1)) j++;
            }
            iS->i++;
        }
        if(bracketcount>0) { lexerror=1; }

        iS->tstop=iS->i-1;
        cT->macrodef[j]='\0';
        cT->token=TOKEN_MDEF;

        if(lexerror) { ecparseerror(iS,"Macro definition missing ending bracket '}'.\n"); }

        return(TRUE);
    }


    /*
    ** Quoted string
    **
    ** \".*\"
    */
    if(iS->buffer[iS->i]=='\"') {
        j=0; iS->i++;
        while(iS->buffer[iS->i]!='\0' && iS->buffer[iS->i]!='\"') {
            if(iS->buffer[iS->i]=='\\') {
                iS->i++;
                if(iS->buffer[iS->i]) {
                    switch(iS->buffer[iS->i]) {
                        case 'b':
                            cT->string[j]='\b';   /* backspace       */
                            break;
                        case 'a':
                            cT->string[j]='\a';   /* bell            */
                            break;
                        case 'f':
                            cT->string[j]='\f';   /* formfeed        */
                            break;
                        case 'n':
                            cT->string[j]='\n';   /* newline         */
                            break;
                        case 'r':
                            cT->string[j]='\r';   /* carriage return */
                            break;
                        case 't':
                            cT->string[j]='\t';   /* tab             */
                            break;
                        case 'v':
                            cT->string[j]='\v';   /* vertical tab    */
                            break;
                        case '\\':
                            cT->string[j]='\\';   /* backslash       */
                            break;
                        default:
                            cT->string[j]=iS->buffer[iS->i];
                            break;
                    }
                    j++;
                }
            } else {
                cT->string[j]=iS->buffer[iS->i];
                j++;
            }
            if(iS->buffer[iS->i]) iS->i++;
        }
        if(iS->buffer[iS->i] && iS->buffer[iS->i]=='\"') { iS->i++; } else { lexerror=1; }

        iS->tstop=iS->i-1;
        cT->string[j]='\0';
        cT->token=TOKEN_STR;

        if(lexerror) { ecparseerror(iS,"Unterminated string.\n"); }

        return(TRUE);
    }



    /*
    ** Return unrecognized token with error-condition
    */
    cT->token=TOKEN_ERR;
    cT->id[0]=iS->buffer[iS->i]; cT->id[1]='\0';
    return(TRUE);
}



#define  MODE_BIN 1
#define  MODE_PI  2
#define  MODE_DEC 3
#define  MODE_HEX 4
#define  MODE_OCT 5

#define ISBINDIGIT(x) ((x)=='0' || (x)=='1')
#define ISPIDIGIT(x)  ((x)>='0' && (x)<='3')
#define ISOCTDIGIT(x) ((x)>='0' && (x)<='7')
#define ISHEXDIGIT(x) (((x)>='0' && (x)<='9') || ((x)>='A' && (x)<='F') \
                                              || ((x)>='a' && (x)<='f'))
#define ISHEXALPHA(x) (((x)>='a' && (x)<='f') || ((x)>='A' && (x)<='F'))

/*****************************************************************************
 ***                                                                       ***
 *** getnumber() - Convert a number from text to double                    ***
 ***               DEC,HEX,OCT,BIN.                                        ***
 ***                                                                       ***
 *****************************************************************************/
int getnumber(double *num,inputStream *iS)
{
double integer=0,fraction=0,exponent=0,expsign=1,decimalcount=1,multiplier=10;
long   mode=MODE_DEC;

    if(iS->buffer[iS->i]=='.' || iS->buffer[iS->i]==':' || ISDIGIT(iS->buffer[iS->i])) {
        mode=MODE_DEC; multiplier=10;

        /*
        ** Determine what base the number is in
        */
        if(iS->buffer[iS->i]=='0') {
            iS->i++;
            if(iS->buffer[iS->i] && ISALPHA(iS->buffer[iS->i])) {
                switch(iS->buffer[iS->i]) {
                    case 'b':
                    case 'B':
                        mode=MODE_BIN; multiplier=2;
                        break;
                    case 'p':
                    case 'P':
                        mode=MODE_PI; multiplier=3.141592653589793238462643383;
                        break;
                    case 'd':
                    case 'D':
                        mode=MODE_DEC; multiplier=10;
                        break;
                    case 'o':
                    case 'O':
                        mode=MODE_OCT; multiplier=8;
                        break;
                    case 'x':
                    case 'X':
                        mode=MODE_HEX; multiplier=16;
                        break;
                    default:
                        ecparseerror(iS,"Unknown number type.\n");
                        break;
                }
                iS->i++;
            }
        }

        /*
        ** Integer part
        */
        integer=0;
        while((mode==MODE_BIN && ISBINDIGIT(iS->buffer[iS->i])) ||
              (mode==MODE_PI  && ISPIDIGIT(iS->buffer[iS->i]))  ||
              (mode==MODE_DEC && ISDIGIT(iS->buffer[iS->i]))    ||
              (mode==MODE_OCT && ISOCTDIGIT(iS->buffer[iS->i])) ||
              (mode==MODE_HEX && ISHEXDIGIT(iS->buffer[iS->i]))) {
            integer*=multiplier;
            if(mode==MODE_HEX && ISHEXALPHA(iS->buffer[iS->i])) {
                integer+=(ISLOWER(iS->buffer[iS->i]) ?
                          (iS->buffer[iS->i]-'a'+10) : (iS->buffer[iS->i]-'A'+10));
            } else {
                integer+=iS->buffer[iS->i]-'0';
            }
            iS->i++;
        }

        /*
        ** Decimal part
        */
        if(iS->buffer[iS->i]=='.') {
            iS->i++;
            fraction=0; decimalcount=1;
            while((mode==MODE_BIN && ISBINDIGIT(iS->buffer[iS->i])) ||
                  (mode==MODE_PI  && ISPIDIGIT(iS->buffer[iS->i]))  ||
                  (mode==MODE_DEC && ISDIGIT(iS->buffer[iS->i]))    ||
                  (mode==MODE_OCT && ISOCTDIGIT(iS->buffer[iS->i])) ||
                  (mode==MODE_HEX && ISHEXDIGIT(iS->buffer[iS->i]))) {
                fraction*=multiplier;
                decimalcount*=multiplier;
                if(mode==MODE_HEX && ISHEXALPHA(iS->buffer[iS->i])) {
                    fraction+=(ISLOWER(iS->buffer[iS->i]) ?
                              (iS->buffer[iS->i]-'a'+10) : (iS->buffer[iS->i]-'A'+10));
                } else {
                    fraction+=iS->buffer[iS->i]-'0';
                }
                iS->i++;
            }
        }

        /*
        ** Exponent (decimal only..)
        */
        if((mode==MODE_DEC && (iS->buffer[iS->i]=='e' || iS->buffer[iS->i]=='E'))) {
            iS->i++;
            exponent=0; expsign=1;
            if(iS->buffer[iS->i]=='+') { expsign=1; iS->i++; }
            if(iS->buffer[iS->i]=='-') { expsign=-1; iS->i++; }
            while((mode==MODE_DEC && ISDIGIT(iS->buffer[iS->i]))) {
                exponent*=multiplier;
                exponent+=iS->buffer[iS->i]-'0';
                iS->i++;
            }
        }

        *num=(integer+(fraction/decimalcount))*pow(multiplier,exponent*expsign);
        return(TRUE);
    }

    *num=0;

return(FALSE);
}
