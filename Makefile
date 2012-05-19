#
#	Makefile - Top level Makefile when using "make" to build.
#			   Alternatively, use bit directly.
#	

OS      := $(shell uname | sed 's/CYGWIN.*/windows/;s/Darwin/macosx/' | tr '[A-Z]' '[a-z]')
MAKE	:= make
EXT		:= mk

ifeq ($(OS),windows)
ifeq ($(ARCH),)
ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
	ARCH:=x64
else
	ARCH:=x86
endif
endif
	MAKE:= projects/windows.bat $(ARCH)
	EXT := nmake
endif

all clean clobber compile:
	$(MAKE) -f projects/*-$(OS).$(EXT) $@

build configure generate test package:
	bit $@

version:
	@bit -q version
