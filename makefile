###############################################################################
#                                                                             #
# EasyCalc - MakeFile                                                         #
#                                                                             #
###############################################################################

##
## For OSX/clang
##
CC=clang
#DEFS=-DEC_ARCH_OSX -DEC_USE_EDITLINE -MEMAUDIT
DEFS=-DEC_ARCH_OSX -DMEMAUDIT
FLAGS=-Wall -Wpointer-arith -Wstrict-prototypes -pedantic -g
OFLAGS=-O2
LIBS=-lm -ledit -lcurses  -ltermcap

##
## For Linux/GCC
##
#CC=gcc
#DEFS=-DEC_ARCH_LINUX -DEC_USE_GNU_READLINE -DMEMAUDIT -DMEMAUDIT_AUDIT_GNU_READLINE
#FLAGS=-Wall -Wpointer-arith -Wstrict-prototypes -pedantic -g
##OFLAGS=-O3 -fschedule-insns2 -fomit-frame-pointer -fstrength-reduce -fexpensive-optimizations
#OFLAGS=-O2
##LIBS=-lm -lreadline -lefence
#LIBS=-lm -lreadline

##
## For Amiga/DICE
##
#CC=dcc
#DEFS=-DEC_ARCH_AMIGA_DICE
#FLAGS=-Iinclude -Ireadargs
#OFLAGS=
#LIBS=-lm


OBJS=obj/global.o obj/safe_strncpy.o obj/symbol.o obj/macro.o obj/emit.o obj/lexer.o obj/parser.o obj/main.o obj/command.o obj/file.o obj/message.o obj/input.o obj/readargs.o obj/architecture.o obj/memaudit.o


ec : $(OBJS)
	$(CC) $(OBJS) -o ec $(LIBS)

obj/safe_strncpy.o : safe_strncpy.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) safe_strncpy.c -o obj/safe_strncpy.o

obj/global.o : global.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) global.c -o obj/global.o

obj/main.o : main.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) main.c -o obj/main.o

obj/command.o : command.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) command.c -o obj/command.o

obj/file.o : file.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) file.c -o obj/file.o

obj/message.o : message.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) message.c -o obj/message.o

obj/input.o : input.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) input.c -o obj/input.o

obj/lexer.o : lexer.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) lexer.c -o obj/lexer.o

obj/parser.o : parser.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) parser.c -o obj/parser.o

obj/emit.o : emit.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) emit.c -o obj/emit.o

obj/symbol.o : symbol.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) symbol.c -o obj/symbol.o

obj/macro.o : macro.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) macro.c -o obj/macro.o

obj/readargs.o : readargs/readargs.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) readargs/readargs.c -o obj/readargs.o

obj/architecture.o : architecture.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) architecture.c -o obj/architecture.o

obj/memaudit.o : memaudit/memaudit.c
	$(CC) -c $(FLAGS) $(OFLAGS) $(DEFS) memaudit/memaudit.c -o obj/memaudit.o

###############################################################################
