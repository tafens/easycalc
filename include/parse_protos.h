/*****************************************************************************
 ***                                                                       ***
 *** parser_protos.h                                                       ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef PARSER_PROTOS_H
#define PARSER_PROTOS_H

void ecenabledryparse(void);
void ecdisbledryparse(void);
void eccleardryparse(void);
long eccheckdryparse(void);

long ecchkquit(void);

void   parser_init(void);
double parse(inputStream *iS, symTable *sT,int parselevel);

#endif
