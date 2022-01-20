/*****************************************************************************
 ***                                                                       ***
 *** message.h                                                             ***
 ***                                                                       ***
 *****************************************************************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#define VERBOSITYMIN       -3
#define VERBOSITY_NONE     -3  /* Nothing at all                   */
#define VERBOSITY_ANS      -2  /* Results from calculations only   */
#define VERBOSITY_QUIET    -1  /* Error messages                   */
#define VERBOSITY_NORMAL    0  /* Prompt, messages                 */
#define VERBOSITY_VERBOSE   1  /* More messages, etc.              */
#define VERBOSITY_PLUS      2  /* Parse/stack operations           */
#define VERBOSITYMAX        2

#define RESMODEMIN          0
#define RESMODE_UNKNOWN     0  /*                                  */
#define RESMODE_DECIMAL     1  /* Decimal                          */
#define RESMODE_HEXADECIMAL 2  /* Hexadecmal         (ex. 9A7.23E) */
#define RESMODE_OCTAL       3  /* Octal              (ex. 235.765) */
#define RESMODE_BINARY      4  /* Binary           (ex. 1100.1001) */
#define RESMODE_SCIENTIFIC  5  /* Scientific decimal   (ex. 1e+09) */
#define RESMODE_HHMMSS      6  /* Hours:Minutes:Seconds            */
#define RESMODEMAX          6

#endif
