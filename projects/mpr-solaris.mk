#
#   mpr-solaris.mk -- Build It Makefile to build Multithreaded Portable Runtime for solaris
#

ARCH     := $(shell uname -m | sed 's/i.86/x86/')
OS       := solaris
PROFILE  := debug
CONFIG   := $(OS)-$(ARCH)-$(PROFILE)
CC       := gcc
LD       := ld
CFLAGS   := -fPIC -g -mcpu=generic -w
DFLAGS   := -D_REENTRANT -DPIC -DBLD_DEBUG
IFLAGS   := -I$(CONFIG)/inc
LDFLAGS  := '-g'
LIBPATHS := -L$(CONFIG)/bin
LIBS     := -llxnet -lrt -lsocket -lpthread -lm

all: prep \
        $(CONFIG)/bin/benchMpr \
        $(CONFIG)/bin/runProgram \
        $(CONFIG)/bin/testMpr \
        $(CONFIG)/bin/libmpr.so \
        $(CONFIG)/bin/libmprssl.so \
        $(CONFIG)/bin/manager \
        $(CONFIG)/bin/makerom \
        $(CONFIG)/bin/chargen

.PHONY: prep

prep:
	@[ ! -x $(CONFIG)/inc ] && mkdir -p $(CONFIG)/inc $(CONFIG)/obj $(CONFIG)/lib $(CONFIG)/bin ; true
	@[ ! -f $(CONFIG)/inc/bit.h ] && cp projects/mpr-$(OS)-bit.h $(CONFIG)/inc/bit.h ; true
	@if ! diff $(CONFIG)/inc/bit.h projects/mpr-$(OS)-bit.h >/dev/null ; then\
		echo cp projects/mpr-$(OS)-bit.h $(CONFIG)/inc/bit.h  ; \
		cp projects/mpr-$(OS)-bit.h $(CONFIG)/inc/bit.h  ; \
	fi; true

clean:
	rm -rf $(CONFIG)/bin/benchMpr
	rm -rf $(CONFIG)/bin/runProgram
	rm -rf $(CONFIG)/bin/testMpr
	rm -rf $(CONFIG)/bin/libmpr.so
	rm -rf $(CONFIG)/bin/libmprssl.so
	rm -rf $(CONFIG)/bin/manager
	rm -rf $(CONFIG)/bin/makerom
	rm -rf $(CONFIG)/bin/chargen
	rm -rf $(CONFIG)/obj/benchMpr.o
	rm -rf $(CONFIG)/obj/runProgram.o
	rm -rf $(CONFIG)/obj/testArgv.o
	rm -rf $(CONFIG)/obj/testBuf.o
	rm -rf $(CONFIG)/obj/testCmd.o
	rm -rf $(CONFIG)/obj/testCond.o
	rm -rf $(CONFIG)/obj/testEvent.o
	rm -rf $(CONFIG)/obj/testFile.o
	rm -rf $(CONFIG)/obj/testHash.o
	rm -rf $(CONFIG)/obj/testList.o
	rm -rf $(CONFIG)/obj/testLock.o
	rm -rf $(CONFIG)/obj/testMem.o
	rm -rf $(CONFIG)/obj/testMpr.o
	rm -rf $(CONFIG)/obj/testPath.o
	rm -rf $(CONFIG)/obj/testSocket.o
	rm -rf $(CONFIG)/obj/testSprintf.o
	rm -rf $(CONFIG)/obj/testThread.o
	rm -rf $(CONFIG)/obj/testTime.o
	rm -rf $(CONFIG)/obj/testUnicode.o
	rm -rf $(CONFIG)/obj/dtoa.o
	rm -rf $(CONFIG)/obj/mpr.o
	rm -rf $(CONFIG)/obj/mprAsync.o
	rm -rf $(CONFIG)/obj/mprAtomic.o
	rm -rf $(CONFIG)/obj/mprBuf.o
	rm -rf $(CONFIG)/obj/mprCache.o
	rm -rf $(CONFIG)/obj/mprCmd.o
	rm -rf $(CONFIG)/obj/mprCond.o
	rm -rf $(CONFIG)/obj/mprCrypt.o
	rm -rf $(CONFIG)/obj/mprDisk.o
	rm -rf $(CONFIG)/obj/mprDispatcher.o
	rm -rf $(CONFIG)/obj/mprEncode.o
	rm -rf $(CONFIG)/obj/mprEpoll.o
	rm -rf $(CONFIG)/obj/mprEvent.o
	rm -rf $(CONFIG)/obj/mprFile.o
	rm -rf $(CONFIG)/obj/mprFileSystem.o
	rm -rf $(CONFIG)/obj/mprHash.o
	rm -rf $(CONFIG)/obj/mprJSON.o
	rm -rf $(CONFIG)/obj/mprKqueue.o
	rm -rf $(CONFIG)/obj/mprList.o
	rm -rf $(CONFIG)/obj/mprLock.o
	rm -rf $(CONFIG)/obj/mprLog.o
	rm -rf $(CONFIG)/obj/mprMem.o
	rm -rf $(CONFIG)/obj/mprMime.o
	rm -rf $(CONFIG)/obj/mprMixed.o
	rm -rf $(CONFIG)/obj/mprModule.o
	rm -rf $(CONFIG)/obj/mprPath.o
	rm -rf $(CONFIG)/obj/mprPoll.o
	rm -rf $(CONFIG)/obj/mprPrintf.o
	rm -rf $(CONFIG)/obj/mprRomFile.o
	rm -rf $(CONFIG)/obj/mprSelect.o
	rm -rf $(CONFIG)/obj/mprSignal.o
	rm -rf $(CONFIG)/obj/mprSocket.o
	rm -rf $(CONFIG)/obj/mprString.o
	rm -rf $(CONFIG)/obj/mprTest.o
	rm -rf $(CONFIG)/obj/mprThread.o
	rm -rf $(CONFIG)/obj/mprTime.o
	rm -rf $(CONFIG)/obj/mprUnix.o
	rm -rf $(CONFIG)/obj/mprVxworks.o
	rm -rf $(CONFIG)/obj/mprWait.o
	rm -rf $(CONFIG)/obj/mprWide.o
	rm -rf $(CONFIG)/obj/mprWin.o
	rm -rf $(CONFIG)/obj/mprWince.o
	rm -rf $(CONFIG)/obj/mprXml.o
	rm -rf $(CONFIG)/obj/mprMatrixssl.o
	rm -rf $(CONFIG)/obj/mprOpenssl.o
	rm -rf $(CONFIG)/obj/mprSsl.o
	rm -rf $(CONFIG)/obj/manager.o
	rm -rf $(CONFIG)/obj/makerom.o
	rm -rf $(CONFIG)/obj/charGen.o

clobber: clean
	rm -fr ./$(CONFIG)

$(CONFIG)/inc/mpr.h: 
	rm -fr $(CONFIG)/inc/mpr.h
	cp -r src/mpr.h $(CONFIG)/inc/mpr.h

$(CONFIG)/obj/dtoa.o: \
        src/dtoa.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/dtoa.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/dtoa.c

$(CONFIG)/obj/mpr.o: \
        src/mpr.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mpr.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mpr.c

$(CONFIG)/obj/mprAsync.o: \
        src/mprAsync.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprAsync.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprAsync.c

$(CONFIG)/obj/mprAtomic.o: \
        src/mprAtomic.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprAtomic.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprAtomic.c

$(CONFIG)/obj/mprBuf.o: \
        src/mprBuf.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprBuf.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprBuf.c

$(CONFIG)/obj/mprCache.o: \
        src/mprCache.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprCache.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprCache.c

$(CONFIG)/obj/mprCmd.o: \
        src/mprCmd.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprCmd.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprCmd.c

$(CONFIG)/obj/mprCond.o: \
        src/mprCond.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprCond.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprCond.c

$(CONFIG)/obj/mprCrypt.o: \
        src/mprCrypt.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprCrypt.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprCrypt.c

$(CONFIG)/obj/mprDisk.o: \
        src/mprDisk.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprDisk.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprDisk.c

$(CONFIG)/obj/mprDispatcher.o: \
        src/mprDispatcher.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprDispatcher.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprDispatcher.c

$(CONFIG)/obj/mprEncode.o: \
        src/mprEncode.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprEncode.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprEncode.c

$(CONFIG)/obj/mprEpoll.o: \
        src/mprEpoll.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprEpoll.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprEpoll.c

$(CONFIG)/obj/mprEvent.o: \
        src/mprEvent.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprEvent.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprEvent.c

$(CONFIG)/obj/mprFile.o: \
        src/mprFile.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprFile.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprFile.c

$(CONFIG)/obj/mprFileSystem.o: \
        src/mprFileSystem.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprFileSystem.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprFileSystem.c

$(CONFIG)/obj/mprHash.o: \
        src/mprHash.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprHash.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprHash.c

$(CONFIG)/obj/mprJSON.o: \
        src/mprJSON.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprJSON.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprJSON.c

$(CONFIG)/obj/mprKqueue.o: \
        src/mprKqueue.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprKqueue.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprKqueue.c

$(CONFIG)/obj/mprList.o: \
        src/mprList.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprList.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprList.c

$(CONFIG)/obj/mprLock.o: \
        src/mprLock.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprLock.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprLock.c

$(CONFIG)/obj/mprLog.o: \
        src/mprLog.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprLog.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprLog.c

$(CONFIG)/obj/mprMem.o: \
        src/mprMem.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprMem.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprMem.c

$(CONFIG)/obj/mprMime.o: \
        src/mprMime.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprMime.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprMime.c

$(CONFIG)/obj/mprMixed.o: \
        src/mprMixed.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprMixed.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprMixed.c

$(CONFIG)/obj/mprModule.o: \
        src/mprModule.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprModule.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprModule.c

$(CONFIG)/obj/mprPath.o: \
        src/mprPath.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprPath.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprPath.c

$(CONFIG)/obj/mprPoll.o: \
        src/mprPoll.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprPoll.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprPoll.c

$(CONFIG)/obj/mprPrintf.o: \
        src/mprPrintf.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprPrintf.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprPrintf.c

$(CONFIG)/obj/mprRomFile.o: \
        src/mprRomFile.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprRomFile.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprRomFile.c

$(CONFIG)/obj/mprSelect.o: \
        src/mprSelect.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprSelect.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprSelect.c

$(CONFIG)/obj/mprSignal.o: \
        src/mprSignal.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprSignal.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprSignal.c

$(CONFIG)/obj/mprSocket.o: \
        src/mprSocket.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprSocket.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprSocket.c

$(CONFIG)/obj/mprString.o: \
        src/mprString.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprString.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprString.c

$(CONFIG)/obj/mprTest.o: \
        src/mprTest.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprTest.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprTest.c

$(CONFIG)/obj/mprThread.o: \
        src/mprThread.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprThread.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprThread.c

$(CONFIG)/obj/mprTime.o: \
        src/mprTime.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprTime.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprTime.c

$(CONFIG)/obj/mprUnix.o: \
        src/mprUnix.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprUnix.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprUnix.c

$(CONFIG)/obj/mprVxworks.o: \
        src/mprVxworks.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprVxworks.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprVxworks.c

$(CONFIG)/obj/mprWait.o: \
        src/mprWait.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprWait.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprWait.c

$(CONFIG)/obj/mprWide.o: \
        src/mprWide.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprWide.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprWide.c

$(CONFIG)/obj/mprWin.o: \
        src/mprWin.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprWin.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprWin.c

$(CONFIG)/obj/mprWince.o: \
        src/mprWince.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprWince.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprWince.c

$(CONFIG)/obj/mprXml.o: \
        src/mprXml.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprXml.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprXml.c

$(CONFIG)/bin/libmpr.so:  \
        $(CONFIG)/inc/mpr.h \
        $(CONFIG)/obj/dtoa.o \
        $(CONFIG)/obj/mpr.o \
        $(CONFIG)/obj/mprAsync.o \
        $(CONFIG)/obj/mprAtomic.o \
        $(CONFIG)/obj/mprBuf.o \
        $(CONFIG)/obj/mprCache.o \
        $(CONFIG)/obj/mprCmd.o \
        $(CONFIG)/obj/mprCond.o \
        $(CONFIG)/obj/mprCrypt.o \
        $(CONFIG)/obj/mprDisk.o \
        $(CONFIG)/obj/mprDispatcher.o \
        $(CONFIG)/obj/mprEncode.o \
        $(CONFIG)/obj/mprEpoll.o \
        $(CONFIG)/obj/mprEvent.o \
        $(CONFIG)/obj/mprFile.o \
        $(CONFIG)/obj/mprFileSystem.o \
        $(CONFIG)/obj/mprHash.o \
        $(CONFIG)/obj/mprJSON.o \
        $(CONFIG)/obj/mprKqueue.o \
        $(CONFIG)/obj/mprList.o \
        $(CONFIG)/obj/mprLock.o \
        $(CONFIG)/obj/mprLog.o \
        $(CONFIG)/obj/mprMem.o \
        $(CONFIG)/obj/mprMime.o \
        $(CONFIG)/obj/mprMixed.o \
        $(CONFIG)/obj/mprModule.o \
        $(CONFIG)/obj/mprPath.o \
        $(CONFIG)/obj/mprPoll.o \
        $(CONFIG)/obj/mprPrintf.o \
        $(CONFIG)/obj/mprRomFile.o \
        $(CONFIG)/obj/mprSelect.o \
        $(CONFIG)/obj/mprSignal.o \
        $(CONFIG)/obj/mprSocket.o \
        $(CONFIG)/obj/mprString.o \
        $(CONFIG)/obj/mprTest.o \
        $(CONFIG)/obj/mprThread.o \
        $(CONFIG)/obj/mprTime.o \
        $(CONFIG)/obj/mprUnix.o \
        $(CONFIG)/obj/mprVxworks.o \
        $(CONFIG)/obj/mprWait.o \
        $(CONFIG)/obj/mprWide.o \
        $(CONFIG)/obj/mprWin.o \
        $(CONFIG)/obj/mprWince.o \
        $(CONFIG)/obj/mprXml.o
	$(CC) -shared -o $(CONFIG)/bin/libmpr.so $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/dtoa.o $(CONFIG)/obj/mpr.o $(CONFIG)/obj/mprAsync.o $(CONFIG)/obj/mprAtomic.o $(CONFIG)/obj/mprBuf.o $(CONFIG)/obj/mprCache.o $(CONFIG)/obj/mprCmd.o $(CONFIG)/obj/mprCond.o $(CONFIG)/obj/mprCrypt.o $(CONFIG)/obj/mprDisk.o $(CONFIG)/obj/mprDispatcher.o $(CONFIG)/obj/mprEncode.o $(CONFIG)/obj/mprEpoll.o $(CONFIG)/obj/mprEvent.o $(CONFIG)/obj/mprFile.o $(CONFIG)/obj/mprFileSystem.o $(CONFIG)/obj/mprHash.o $(CONFIG)/obj/mprJSON.o $(CONFIG)/obj/mprKqueue.o $(CONFIG)/obj/mprList.o $(CONFIG)/obj/mprLock.o $(CONFIG)/obj/mprLog.o $(CONFIG)/obj/mprMem.o $(CONFIG)/obj/mprMime.o $(CONFIG)/obj/mprMixed.o $(CONFIG)/obj/mprModule.o $(CONFIG)/obj/mprPath.o $(CONFIG)/obj/mprPoll.o $(CONFIG)/obj/mprPrintf.o $(CONFIG)/obj/mprRomFile.o $(CONFIG)/obj/mprSelect.o $(CONFIG)/obj/mprSignal.o $(CONFIG)/obj/mprSocket.o $(CONFIG)/obj/mprString.o $(CONFIG)/obj/mprTest.o $(CONFIG)/obj/mprThread.o $(CONFIG)/obj/mprTime.o $(CONFIG)/obj/mprUnix.o $(CONFIG)/obj/mprVxworks.o $(CONFIG)/obj/mprWait.o $(CONFIG)/obj/mprWide.o $(CONFIG)/obj/mprWin.o $(CONFIG)/obj/mprWince.o $(CONFIG)/obj/mprXml.o $(LIBS)

$(CONFIG)/obj/benchMpr.o: \
        test/benchMpr.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/benchMpr.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/benchMpr.c

$(CONFIG)/bin/benchMpr:  \
        $(CONFIG)/bin/libmpr.so \
        $(CONFIG)/obj/benchMpr.o
	$(CC) -o $(CONFIG)/bin/benchMpr $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/benchMpr.o $(LIBS) -lmpr $(LDFLAGS)

$(CONFIG)/obj/runProgram.o: \
        test/runProgram.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/runProgram.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/runProgram.c

$(CONFIG)/bin/runProgram:  \
        $(CONFIG)/obj/runProgram.o
	$(CC) -o $(CONFIG)/bin/runProgram $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/runProgram.o $(LIBS) $(LDFLAGS)

$(CONFIG)/obj/mprMatrixssl.o: \
        src/mprMatrixssl.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprMatrixssl.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprMatrixssl.c

$(CONFIG)/obj/mprOpenssl.o: \
        src/mprOpenssl.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprOpenssl.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprOpenssl.c

$(CONFIG)/obj/mprSsl.o: \
        src/mprSsl.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/mprSsl.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/mprSsl.c

$(CONFIG)/bin/libmprssl.so:  \
        $(CONFIG)/bin/libmpr.so \
        $(CONFIG)/obj/mprMatrixssl.o \
        $(CONFIG)/obj/mprOpenssl.o \
        $(CONFIG)/obj/mprSsl.o
	$(CC) -shared -o $(CONFIG)/bin/libmprssl.so $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/mprMatrixssl.o $(CONFIG)/obj/mprOpenssl.o $(CONFIG)/obj/mprSsl.o $(LIBS) -lmpr

$(CONFIG)/obj/testArgv.o: \
        test/testArgv.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testArgv.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testArgv.c

$(CONFIG)/obj/testBuf.o: \
        test/testBuf.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testBuf.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testBuf.c

$(CONFIG)/obj/testCmd.o: \
        test/testCmd.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testCmd.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testCmd.c

$(CONFIG)/obj/testCond.o: \
        test/testCond.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testCond.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testCond.c

$(CONFIG)/obj/testEvent.o: \
        test/testEvent.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testEvent.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testEvent.c

$(CONFIG)/obj/testFile.o: \
        test/testFile.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testFile.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testFile.c

$(CONFIG)/obj/testHash.o: \
        test/testHash.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testHash.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testHash.c

$(CONFIG)/obj/testList.o: \
        test/testList.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testList.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testList.c

$(CONFIG)/obj/testLock.o: \
        test/testLock.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testLock.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testLock.c

$(CONFIG)/obj/testMem.o: \
        test/testMem.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testMem.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testMem.c

$(CONFIG)/obj/testMpr.o: \
        test/testMpr.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testMpr.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testMpr.c

$(CONFIG)/obj/testPath.o: \
        test/testPath.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testPath.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testPath.c

$(CONFIG)/obj/testSocket.o: \
        test/testSocket.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testSocket.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testSocket.c

$(CONFIG)/obj/testSprintf.o: \
        test/testSprintf.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testSprintf.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testSprintf.c

$(CONFIG)/obj/testThread.o: \
        test/testThread.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testThread.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testThread.c

$(CONFIG)/obj/testTime.o: \
        test/testTime.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testTime.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testTime.c

$(CONFIG)/obj/testUnicode.o: \
        test/testUnicode.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/testUnicode.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc test/testUnicode.c

$(CONFIG)/bin/testMpr:  \
        $(CONFIG)/bin/libmpr.so \
        $(CONFIG)/bin/libmprssl.so \
        $(CONFIG)/bin/runProgram \
        $(CONFIG)/obj/testArgv.o \
        $(CONFIG)/obj/testBuf.o \
        $(CONFIG)/obj/testCmd.o \
        $(CONFIG)/obj/testCond.o \
        $(CONFIG)/obj/testEvent.o \
        $(CONFIG)/obj/testFile.o \
        $(CONFIG)/obj/testHash.o \
        $(CONFIG)/obj/testList.o \
        $(CONFIG)/obj/testLock.o \
        $(CONFIG)/obj/testMem.o \
        $(CONFIG)/obj/testMpr.o \
        $(CONFIG)/obj/testPath.o \
        $(CONFIG)/obj/testSocket.o \
        $(CONFIG)/obj/testSprintf.o \
        $(CONFIG)/obj/testThread.o \
        $(CONFIG)/obj/testTime.o \
        $(CONFIG)/obj/testUnicode.o
	$(CC) -o $(CONFIG)/bin/testMpr $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/testArgv.o $(CONFIG)/obj/testBuf.o $(CONFIG)/obj/testCmd.o $(CONFIG)/obj/testCond.o $(CONFIG)/obj/testEvent.o $(CONFIG)/obj/testFile.o $(CONFIG)/obj/testHash.o $(CONFIG)/obj/testList.o $(CONFIG)/obj/testLock.o $(CONFIG)/obj/testMem.o $(CONFIG)/obj/testMpr.o $(CONFIG)/obj/testPath.o $(CONFIG)/obj/testSocket.o $(CONFIG)/obj/testSprintf.o $(CONFIG)/obj/testThread.o $(CONFIG)/obj/testTime.o $(CONFIG)/obj/testUnicode.o $(LIBS) -lmpr -lmprssl $(LDFLAGS)

$(CONFIG)/obj/manager.o: \
        src/manager.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/manager.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/manager.c

$(CONFIG)/bin/manager:  \
        $(CONFIG)/bin/libmpr.so \
        $(CONFIG)/obj/manager.o
	$(CC) -o $(CONFIG)/bin/manager $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/manager.o $(LIBS) -lmpr $(LDFLAGS)

$(CONFIG)/obj/makerom.o: \
        src/utils/makerom.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/makerom.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/utils/makerom.c

$(CONFIG)/bin/makerom:  \
        $(CONFIG)/bin/libmpr.so \
        $(CONFIG)/obj/makerom.o
	$(CC) -o $(CONFIG)/bin/makerom $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/makerom.o $(LIBS) -lmpr $(LDFLAGS)

$(CONFIG)/obj/charGen.o: \
        src/utils/charGen.c \
        $(CONFIG)/inc/bit.h
	$(CC) -c -o $(CONFIG)/obj/charGen.o -Wall -fPIC $(LDFLAGS) -mcpu=generic $(DFLAGS) -I$(CONFIG)/inc src/utils/charGen.c

$(CONFIG)/bin/chargen:  \
        $(CONFIG)/bin/libmpr.so \
        $(CONFIG)/obj/charGen.o
	$(CC) -o $(CONFIG)/bin/chargen $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/charGen.o $(LIBS) -lmpr $(LDFLAGS)

