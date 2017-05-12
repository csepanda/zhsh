CC      = clang
CFLAGS  = -ansi -Wall -pedantic
VPATH   = src

VERSION = 0.1
STAGE   = dev
MODULES = util.o io.o error.o hashtable.o tty_navigation.o zhysh.o

all: $(MODULES)
	$(CC) $(CFLAGS) build/* -o zhysh-$(STAGE)-$(VERSION)

init:
	mkdir build

clean: 
	rm -rf build/*
	


%.o : %.c ; $(CC) -c $(CFLAGS) $< -o build/$@
