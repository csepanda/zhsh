CC      = gcc
CFLAGS  = -g -c -ansi -Wall -pedantic
LFLAGS  = -g -Wall -pedantic
VPATH   = src

VERSION = 0.1
STAGE   = dev
MODULES = util.o io.o env.o error.o builtin.o hashtable.o arraylist.o tty_navigation.o interpreter.o tty_handle.o zhysh.o

all: $(MODULES)
	$(CC) $(LFLAGS) build/* -o zhysh-$(STAGE)-$(VERSION)
init:  ; mkdir build
clean: ; rm -rf build/*

%.o : %.c ; $(CC) -c $(CFLAGS) $< -o build/$@
