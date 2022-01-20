/*****************************************************************************
 ***                                                                       ***
 *** safe_strncpy() - Implements a safe version of strncpy() that always   ***
 ***                  puts a '\0' at the end of the destination.           ***
 ***                                                                       ***
 *****************************************************************************/

#include <string.h>

char *safe_strncpy(char *a, char *b, long l)
{
    strncpy(a,b,l);
    if(l>0) a[l-1]='\0';

    return(a);
}
