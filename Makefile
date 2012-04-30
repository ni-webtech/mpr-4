#
#	Makefile - Top level Makefile when using "make" to build.
#			   Alternatively, use bit directly.
#	

OS      := $(shell uname | sed 's/CYGWIN.*/win/;s/Darwin/macosx/' | tr '[A-Z]' '[a-z]')
MAKE	:= make
EXT		:= mk

ifeq ($(OS),win)
ifeq ($(ARCH),)
ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
	ARCH:=x64
else
	ARCH:=x86
endif
else
    ARCH:=$(ARCH)
endif
	MAKE:= projects/win.bat $(ARCH)
	EXT := nmake
endif

all clean clobber compile:
	echo $(MAKE) -f projects/*-$(OS).$(EXT) $@
	$(MAKE) -f projects/*-$(OS).$(EXT) $@

build configure generate test package:
	bit $@

version:
	@bit -q version
