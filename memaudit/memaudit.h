/*****************************************************************************
 ***                                                                       ***
 *** memaudit.h                                                            ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef MEMAUDIT_H
#define MEMAUDIT_H

void *mamalloc(size_t size,char *file,long line);
void *macalloc(size_t nelem,size_t size,char *file,long line);
void *marealloc(void *ptr,size_t size,char *file,long line);
char *mastrdup(const char *s1,char *file,long line);
#ifdef MEMAUDIT_AUDIT_GNU_READLINE
char *mareadline(const char *s1,char *file,long line);
#endif
void  mafree(void *ptr,char *file,long line);
long  macheck(void);

#ifdef MEMAUDIT
  #define malloc(x)     mamalloc(x,__FILE__,__LINE__)
  #define calloc(x,y)   mamalloc(x,y,__FILE__,__LINE__)
  #define realloc(x,y)  marealloc(x,y,__FILE__,__LINE__)
  #define strdup(x)     mastrdup(x,__FILE__,__LINE__)
  #ifdef MEMAUDIT_AUDIT_GNU_READLINE
    #define readline(x) mareadline(x,__FILE__,__LINE__)
  #endif
  #define free(x)       mafree(x,__FILE__,__LINE__)
#endif

#endif
