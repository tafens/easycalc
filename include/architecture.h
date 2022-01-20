/*****************************************************************************
 ***                                                                       ***
 *** architecture.h  -  Implements functions needed by certain OS:es       ***
 ***                                                                       ***
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>



#ifdef EC_ARCH_AMIGA_DICE

int snprintf(char *str, size_t size, const  char  *format, ...);
#define strcasecmp stricmp
#define strncasecmp strnicmp

#endif
