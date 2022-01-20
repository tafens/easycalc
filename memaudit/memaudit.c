/*****************************************************************************
 ***                                                                       ***
 *** memaudit.c  -  Controls memory allocation/deallocation and warns      ***
 ***                if memory is corrupted, not freed or freed but not     ***
 ***                allocated                                              ***
 ***                                                                       ***
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#ifdef MEMAUDIT_AUDIT_GNU_READLINE
  #include <readline/readline.h>
  #include <readline/history.h>
#endif


#define  TRUE  1
#define  FALSE 0

#define  TEST_BYTE         ((unsigned char)0xAA)
#define  SAFETY_ZONE_SIZE  16
#define  MAX_STR_LEN       128


typedef struct auditblock {
    unsigned char   *mem;
    size_t           size;

    char             file[MAX_STR_LEN];
    long             line;

    struct auditblock *prev;
    struct auditblock *next;
} auditBlock;

typedef struct auditinfo {
    unsigned long totbytes;
    unsigned long totnumber;

    unsigned long currbytes;
    unsigned long currnumber;
    unsigned long maxcurrbytes;
    unsigned long maxcurrnumber;

    unsigned long largest;
    unsigned long smallest;
} auditInfo;


void *mamalloc(size_t size,char *file,long line);
void *macalloc(size_t nelem,size_t size,char *file,long line);
void *marealloc(void *ptr,size_t size,char *file,long line);
char *mastrdup(const char *s1,char *file,long line);
char *mareadline(const char *s1,char *file,long line);
void  mafree(void *ptr,char *file,long line);
long  macheck(void);




auditBlock *root=NULL;
auditInfo   aI={0,0,0,0,0,0,0,0};



/*****************************************************************************
 ***                                                                       ***
 *** mainsertaudit() -                                                     ***
 ***                                                                       ***
 *****************************************************************************/
long mainsertaudit(unsigned char *newmem,size_t size,char *file,long line)
{
auditBlock    *newAb;
unsigned long  a;

    for(a=0;a<SAFETY_ZONE_SIZE;a++) {
        *(newmem+a)=TEST_BYTE; *(newmem+size+SAFETY_ZONE_SIZE+a)=TEST_BYTE;
    }

    if((newAb=malloc(sizeof(auditBlock)))) {
        newAb->mem=newmem+SAFETY_ZONE_SIZE; newAb->size=size;
        strncpy(newAb->file,file,MAX_STR_LEN); newAb->file[MAX_STR_LEN-1]='\0'; newAb->line=line;
        newAb->prev=NULL; newAb->next=NULL;

        if(root) {
            newAb->prev=NULL; newAb->next=root;
            root->prev=newAb;
        }
        root=newAb;

        /* audit */
        aI.totbytes+=size;
        aI.currbytes+=size; if(aI.currbytes>aI.maxcurrbytes) aI.maxcurrbytes=aI.currbytes;
        aI.totnumber+=1;
        aI.currnumber+=1; if(aI.currnumber>aI.maxcurrnumber) aI.maxcurrnumber=aI.currnumber;

        if(aI.largest<size) aI.largest=size;
        if(aI.smallest>size||aI.smallest==0) aI.smallest=size;

        return(TRUE);
    } else {
        free(newmem); newmem=NULL;
        fprintf(stderr,"MEMAUDIT: Not enough memory for audit-info.\n");
    }

    return(FALSE);
}



/*****************************************************************************
 ***                                                                       ***
 *** madeleteaudit() -                                                     ***
 ***                                                                       ***
 *****************************************************************************/
void madeleteaudit(auditBlock *aB)
{
auditBlock    *tmp;

    if((tmp=aB)) {
        /* unlink */
        if(tmp->prev) tmp->prev->next=tmp->next;
        if(tmp->next) tmp->next->prev=tmp->prev;
        if(root==tmp) root=tmp->next;

        /* audit */
        aI.currbytes-=tmp->size;
        aI.currnumber-=1;

        free(tmp);
    }
}



/*****************************************************************************
 ***                                                                       ***
 *** macheckifcorrupted() -                                                ***
 ***                                                                       ***
 *****************************************************************************/
void macheckifcorrupted(auditBlock *aB)
{
auditBlock    *tmp;
unsigned char *mem;
long           a,cflg;

    if((tmp=aB)) {
        /* Check for corrupted bytes before alloction */
        mem=tmp->mem-SAFETY_ZONE_SIZE; cflg=FALSE;
        for(a=0;a<SAFETY_ZONE_SIZE;a++) { if(mem[a]!=TEST_BYTE) { cflg=TRUE; break; } }
        if(cflg) {
            fprintf(stderr,"MEMAUDIT: Memory before allocation at %p is corrupted!\n"
                           "          (allocated in file \"%s\", line %ld.)\n"
                           "          Showing corrupted bytes within 16 bytes before allocation:\n",tmp->mem,tmp->file,tmp->line);
            fprintf(stderr,"          %p: ",mem);
            for(a=0;a<SAFETY_ZONE_SIZE/2;a++) {
                if(mem[a]!=TEST_BYTE)     { fprintf(stderr,"0x%x,",mem[a]); } else { fprintf(stderr,"    ,"); }
            }
            fprintf(stderr,"\n");
            fprintf(stderr,"          %p: ",mem);
            for(a=SAFETY_ZONE_SIZE/2;a<SAFETY_ZONE_SIZE;a++) {
                if(mem[a]!=TEST_BYTE)     { fprintf(stderr,"0x%x",mem[a]); } else { fprintf(stderr,"    "); }
                if(a!=SAFETY_ZONE_SIZE-1) { fprintf(stderr,","); }
            }
            fprintf(stderr,"\n");
        }

        /* Check for corrupted bytes after alloction */
        mem=tmp->mem+tmp->size; cflg=FALSE;
        for(a=0;a<SAFETY_ZONE_SIZE;a++) { if(mem[a]!=TEST_BYTE) { cflg=TRUE; break; } }
        if(cflg) {
            fprintf(stderr,"MEMAUDIT: Memory after allocation at %p is corrupted!\n"
                           "          (allocated in file \"%s\", line %ld.)\n"
                           "          Showing corrupted bytes within 16 bytes after allocation:\n",tmp->mem,tmp->file,tmp->line);
            fprintf(stderr,"          %p: ",mem);
            for(a=0;a<SAFETY_ZONE_SIZE/2;a++) {
                if(mem[a]!=TEST_BYTE)     { fprintf(stderr,"0x%x,",mem[a]); } else { fprintf(stderr,"    ,"); }
            }
            fprintf(stderr,"\n");
            fprintf(stderr,"          %p: ",mem);
            for(a=SAFETY_ZONE_SIZE/2;a<SAFETY_ZONE_SIZE;a++) {
                if(mem[a]!=TEST_BYTE)     { fprintf(stderr,"0x%x",mem[a]); } else { fprintf(stderr,"    "); }
                if(a!=SAFETY_ZONE_SIZE-1) { fprintf(stderr,","); }
            }
            fprintf(stderr,"\n");
        }
    }
}



/*****************************************************************************
 ***                                                                       ***
 *** mamalloc() -                                                          ***
 ***                                                                       ***
 *****************************************************************************/
void *mamalloc(size_t size,char *file,long line)
{
unsigned char *newmem;

    if((newmem=malloc(size+(SAFETY_ZONE_SIZE*2)))) {
        if(mainsertaudit(newmem,size,file,line)) {
            return(newmem+SAFETY_ZONE_SIZE);
        } else {
            free(newmem); newmem=NULL;
            fprintf(stderr,"MEMAUDIT: Not enough memory for audit-info.\n");
        }
    } else {
        fprintf(stderr,"MEMAUDIT: malloc() failed in mamalloc().\n");
    }

    return(NULL);
}



/*****************************************************************************
 ***                                                                       ***
 *** macalloc() -                                                          ***
 ***                                                                       ***
 *****************************************************************************/
void *macalloc(size_t nelem,size_t size,char *file,long line)
{
unsigned char *newmem;
unsigned long  a;

    size*=nelem;
    if((newmem=malloc(size+(SAFETY_ZONE_SIZE*2)))) {
        for(a=0;a<size;a++) *(newmem+SAFETY_ZONE_SIZE+a)=(unsigned char)0x00;
        if(mainsertaudit(newmem,size,file,line)) {
            return(newmem+SAFETY_ZONE_SIZE);
        } else {
            free(newmem); newmem=NULL;
            fprintf(stderr,"MEMAUDIT: Not enough memory for audit-info.\n");
        }
    } else {
        fprintf(stderr,"MEMAUDIT: malloc() failed in macalloc().\n");
    }

    return(NULL);
}



/*****************************************************************************
 ***                                                                       ***
 *** marealloc() -                                                         ***
 ***                                                                       ***
 *****************************************************************************/
void *marealloc(void *ptr,size_t size,char *file,long line)
{
auditBlock    *tmp=NULL;
unsigned char *oldmem,*newmem;

    if((oldmem=ptr)) {
        for(tmp=root;tmp;tmp=tmp->next) { if(tmp->mem==oldmem) break; }
        if(tmp) {
            macheckifcorrupted(tmp);
        } else {
            fprintf(stderr,"MEMAUDIT: realloc(); Trying to free memory at %p not allocated. (file \"%s\", line %ld).\n",ptr,file,line);
        }
    }

    if(ptr) ptr=(unsigned char*)ptr-SAFETY_ZONE_SIZE;

    if((newmem=realloc(ptr,size+(SAFETY_ZONE_SIZE*2)))) {
        if(tmp) madeleteaudit(tmp);
        if(mainsertaudit(newmem,size,file,line)) {
            return(newmem+SAFETY_ZONE_SIZE);
        } else {
            free(newmem); newmem=NULL;
            fprintf(stderr,"MEMAUDIT: Not enough memory for audit-info.\n");
        }
    } else {
        fprintf(stderr,"MEMAUDIT: realloc() failed in marealloc().\n");
    }

    return(NULL);
}



/*****************************************************************************
 ***                                                                       ***
 *** mastrdup() -                                                          ***
 ***                                                                       ***
 *****************************************************************************/
char *mastrdup(const char *s1,char *file,long line)
{
unsigned char *newmem;
unsigned long size;

    size=strlen(s1)+1;

    if((newmem=malloc(size+(SAFETY_ZONE_SIZE*2)))) {
        strncpy((char*)(newmem+SAFETY_ZONE_SIZE),s1,size); *(newmem+SAFETY_ZONE_SIZE+size-1)='\0';
        if(mainsertaudit(newmem,size,file,line)) {
            return((char *)(newmem+SAFETY_ZONE_SIZE));
        } else {
            free(newmem); newmem=NULL;
            fprintf(stderr,"MEMAUDIT: Not enough memory for audit-info.\n");
        }
    } else {
        fprintf(stderr,"MEMAUDIT: malloc() failed in mastrdup().\n");
    }

    return(NULL);
}



#ifdef MEMAUDIT_AUDIT_GNU_READLINE
/*****************************************************************************
 ***                                                                       ***
 *** mareadline() -                                                        ***
 ***                                                                       ***
 *****************************************************************************/
char *mareadline(const char *prompt,char *file,long line)
{
unsigned char *buf,*newmem;
unsigned long size;

    buf=(unsigned char*)readline(prompt); size=strlen((char*)buf)+1;
    if((newmem=malloc(size+(SAFETY_ZONE_SIZE*2)))) {
        strncpy((char*)(newmem+SAFETY_ZONE_SIZE),(char*)buf,size); *(newmem+SAFETY_ZONE_SIZE+size-1)='\0';
        if(mainsertaudit(newmem,size,file,line)) {
            return((char *)(newmem+SAFETY_ZONE_SIZE));
        } else {
            free(newmem); newmem=NULL;
            fprintf(stderr,"MEMAUDIT: Not enough memory for audit-info.\n");
        }
    } else {
        fprintf(stderr,"MEMAUDIT: malloc() failed in mareadline().\n");
    }
    if(buf) free(buf);

    return(NULL);
}
#endif



/*****************************************************************************
 ***                                                                       ***
 *** mafree() -                                                            ***
 ***                                                                       ***
 *****************************************************************************/
void mafree(void *ptr,char *file,long line)
{
auditBlock *tmp;

    if(ptr) {
        for(tmp=root;tmp;tmp=tmp->next) { if(tmp->mem==ptr) break; }
        if(tmp) {
            macheckifcorrupted(tmp);
            free((unsigned char*)ptr-SAFETY_ZONE_SIZE);
            madeleteaudit(tmp);
        } else {
            fprintf(stderr,"MEMAUDIT: free(); Trying to free memory at %p not allocated. (file \"%s\", line %ld).\n",ptr,file,line);
        }
    } else {
        fprintf(stderr,"MEMAUDIT: free(); Called with a NULL pointer. (file \"%s\", line %ld).\n",file,line);
    }

    return;
}



/*****************************************************************************
 ***                                                                       ***
 *** macheck() -                                                           ***
 ***                                                                       ***
 *****************************************************************************/
long  macheck(void)
{
auditBlock    *tmp;
unsigned long  status=TRUE,left=0;

    if(root) {
        for(tmp=root;tmp;tmp=tmp->next) {
            fprintf(stderr,"MEMAUDIT: Memory left allocated at %p, allocated in file \"%s\", line %ld.\n",tmp->mem,tmp->file,tmp->line);
            left+=tmp->size;
/*
            free((unsigned char*)(tmp->mem)-SAFETY_ZONE_SIZE);
            free(tmp);
*/
        }
        fprintf(stderr,"MEMAUDIT: Total memory left allocated: %ld bytes.\n",left);
        status=FALSE;
    } else {
        status=TRUE;
    }

    fprintf(stderr,"\n");
    fprintf(stderr,"MEMAUDIT STATISTICS:\n");
    fprintf(stderr,"Total bytes allocated: %ld in %ld allocations (avg. %ld bytes).\n",aI.totbytes,aI.totnumber,aI.totbytes/(aI.totnumber>0 ? aI.totnumber : 1));
    fprintf(stderr,"Max simultaneous bytes: %ld.\n",aI.maxcurrbytes);
    fprintf(stderr,"Max simultaneous allocations: %ld.\n",aI.maxcurrnumber);
    fprintf(stderr,"Largest block: %ld, smallest block: %ld.\n",aI.largest,aI.smallest);
    fprintf(stderr,"\n");

    return(status);
}



#if 0
/*
** Test Code
*/

#define malloc(x)    mamalloc(x,__FILE__,__LINE__)
#define calloc(x,y)  mamalloc(x,y,__FILE__,__LINE__)
#define realloc(x,y) marealloc(x,y,__FILE__,__LINE__)
#define strdup(x)    mastrdup(x,__FILE__,__LINE__)
#define free(x)      mafree(x,__FILE__,__LINE__)

int main(int argc, char *argv[])
{
char *mem1,*mem2,*mem3;

    printf("MemAudit test: START.\n");



    puts("Test0");
    free(NULL);

    puts("Test0.1");
    free((void*)0x5A6344);

    puts("Test1");
    mem1=NULL; mem2=NULL; mem3=NULL;
    mem1=malloc(128);
    printf("Test1: Memory allocated (128 bytes): %p.\n",mem1);
    free(mem1);

    puts("Test2");
    mem1=NULL; mem2=NULL; mem3=NULL;
    mem1=malloc(128);
    mem2=malloc(128);
    printf("Test2: Memory allocated (2x128 bytes): %p, %p.\n",mem1,mem2);
    free(mem1);
    free(mem2);

    puts("Test3");
    mem1=NULL; mem2=NULL; mem3=NULL;
    mem1=malloc(1024);
    mem2=malloc(128);
    mem3=malloc(128);
    printf("Test3: Memory allocated (3x128 bytes): %p, %p, %p.\n",mem1,mem2,mem3);
    free(mem1);
    free(mem2);
    free(mem3);

    puts("Test4");
    mem1=NULL; mem2=NULL; mem3=NULL;
    mem1=malloc(128);
    printf("Test4: Memory allocated (128 bytes): %p.\n",mem1);
    *(mem1-1)      = 'a';
    *(mem1-3)      = 'b';
    *(mem1-4)      = 'c';
    *(mem1+128+0)  = 'x';
    *(mem1+128+1)  = 'y';
    *(mem1+128+15) = 'z';
    free(mem1);
    free(mem1);

    puts("Test5");
    mem1=NULL; mem2=NULL; mem3=NULL;
    mem1=strdup("TESTTESTTEST");
    puts(mem1);
    *(mem1+10) = 'q';
    *(mem1+11) = 'w';
    *(mem1+12) = 'e';
    *(mem1+13) = 'r';
    puts(mem1);
    free(mem1);

    puts("Test6");
    mem1=NULL; mem2=NULL; mem3=NULL;
    mem1=realloc(NULL,128);
    printf("Test6: Memory allocated (128 bytes): %p.\n",mem1);
    mem1=realloc(mem1,256);
    printf("Test6: Memory reallocated (256 bytes): %p.\n",mem1);
    *(mem1+256+5) = 'q';
    mem1=realloc(mem1,64);
    printf("Test6: Memory reallocated (64 bytes): %p.\n",mem1);
    free(mem1);

    macheck();

    printf("MemAudit test: END.\n");

return(EXIT_SUCCESS);
}
/**/
#endif
