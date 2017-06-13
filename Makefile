CC      = gcc
CFLAGS  = -g -c -ansi -Wall -pedantic
LFLAGS  = -g -Wall -pedantic
VPATH   = src
VERSION = 0.0.1
STAGE   = dev

PARSER_MODULES  = token.o recognizer.o lexer.o parser.o preprocessor.o interpreter.o
RUNTIME_MODULES = jobs.o cmd.o builtin.o 
UTIL_MODULES    = util.o hashtable.o arraylist.o stringbuilder.o
UX_MODULES      = history.o complete.o
MODULES = $(PARSER_MODULES) $(UX_MODULES) $(UTIL_MODULES) $(RUNTIME_MODULES) io.o env.o error.o tty_navigation.o tty_handle.o sig.o zhsh.o

all: $(MODULES)
	$(CC) $(LFLAGS) build/* -o zhsh-$(STAGE)-$(VERSION)

init:  ; mkdir build build/parser
clean: ; rm -rf build/*

%.o : %.c ; $(CC) -c $(CFLAGS) $< -o build/$@
%.o : parser/%.c ; $(CC) -c $(CFLAGS) $< -o build/$@
%.o : ux/%.c ; $(CC) -c $(CFLAGS) $< -o build/$@
