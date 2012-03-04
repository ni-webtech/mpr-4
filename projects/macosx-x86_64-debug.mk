#
#   build.mk -- Build It Makefile to build Multithreaded Portable Runtime for macosx on x86_64
#

CC        := /usr/bin/cc
CFLAGS    := -fPIC -Wall -g -Wshorten-64-to-32
DFLAGS    := -DPIC -DCPU=X86_64
IFLAGS    := -Imacosx-x86_64-debug/inc -Isrc
LDFLAGS   := -Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -L/Users/mob/git/mpr/macosx-x86_64-debug/lib -g -ldl
LIBS      := -lpthread -lm

all: \
        macosx-x86_64-debug/bin/benchMpr \
        macosx-x86_64-debug/bin/runProgram \
        macosx-x86_64-debug/bin/testMpr \
        macosx-x86_64-debug/lib/libmpr.dylib \
        macosx-x86_64-debug/lib/libmprssl.dylib \
        macosx-x86_64-debug/bin/manager \
        macosx-x86_64-debug/bin/makerom \
        macosx-x86_64-debug/bin/chargen

clean:
	rm -rf macosx-x86_64-debug/bin/benchMpr
	rm -rf macosx-x86_64-debug/bin/runProgram
	rm -rf macosx-x86_64-debug/bin/testMpr
	rm -rf macosx-x86_64-debug/lib/libmpr.dylib
	rm -rf macosx-x86_64-debug/lib/libmprssl.dylib
	rm -rf macosx-x86_64-debug/bin/manager
	rm -rf macosx-x86_64-debug/bin/makerom
	rm -rf macosx-x86_64-debug/bin/chargen
	rm -rf macosx-x86_64-debug/obj/benchMpr.o
	rm -rf macosx-x86_64-debug/obj/runProgram.o
	rm -rf macosx-x86_64-debug/obj/testArgv.o
	rm -rf macosx-x86_64-debug/obj/testBuf.o
	rm -rf macosx-x86_64-debug/obj/testCmd.o
	rm -rf macosx-x86_64-debug/obj/testCond.o
	rm -rf macosx-x86_64-debug/obj/testEvent.o
	rm -rf macosx-x86_64-debug/obj/testFile.o
	rm -rf macosx-x86_64-debug/obj/testHash.o
	rm -rf macosx-x86_64-debug/obj/testList.o
	rm -rf macosx-x86_64-debug/obj/testLock.o
	rm -rf macosx-x86_64-debug/obj/testMem.o
	rm -rf macosx-x86_64-debug/obj/testMpr.o
	rm -rf macosx-x86_64-debug/obj/testPath.o
	rm -rf macosx-x86_64-debug/obj/testSocket.o
	rm -rf macosx-x86_64-debug/obj/testSprintf.o
	rm -rf macosx-x86_64-debug/obj/testThread.o
	rm -rf macosx-x86_64-debug/obj/testTime.o
	rm -rf macosx-x86_64-debug/obj/testUnicode.o
	rm -rf macosx-x86_64-debug/obj/dtoa.o
	rm -rf macosx-x86_64-debug/obj/mpr.o
	rm -rf macosx-x86_64-debug/obj/mprAsync.o
	rm -rf macosx-x86_64-debug/obj/mprAtomic.o
	rm -rf macosx-x86_64-debug/obj/mprBuf.o
	rm -rf macosx-x86_64-debug/obj/mprCache.o
	rm -rf macosx-x86_64-debug/obj/mprCmd.o
	rm -rf macosx-x86_64-debug/obj/mprCond.o
	rm -rf macosx-x86_64-debug/obj/mprCrypt.o
	rm -rf macosx-x86_64-debug/obj/mprDisk.o
	rm -rf macosx-x86_64-debug/obj/mprDispatcher.o
	rm -rf macosx-x86_64-debug/obj/mprEncode.o
	rm -rf macosx-x86_64-debug/obj/mprEpoll.o
	rm -rf macosx-x86_64-debug/obj/mprEvent.o
	rm -rf macosx-x86_64-debug/obj/mprFile.o
	rm -rf macosx-x86_64-debug/obj/mprFileSystem.o
	rm -rf macosx-x86_64-debug/obj/mprHash.o
	rm -rf macosx-x86_64-debug/obj/mprJSON.o
	rm -rf macosx-x86_64-debug/obj/mprKqueue.o
	rm -rf macosx-x86_64-debug/obj/mprList.o
	rm -rf macosx-x86_64-debug/obj/mprLock.o
	rm -rf macosx-x86_64-debug/obj/mprLog.o
	rm -rf macosx-x86_64-debug/obj/mprMem.o
	rm -rf macosx-x86_64-debug/obj/mprMime.o
	rm -rf macosx-x86_64-debug/obj/mprMixed.o
	rm -rf macosx-x86_64-debug/obj/mprModule.o
	rm -rf macosx-x86_64-debug/obj/mprPath.o
	rm -rf macosx-x86_64-debug/obj/mprPoll.o
	rm -rf macosx-x86_64-debug/obj/mprPrintf.o
	rm -rf macosx-x86_64-debug/obj/mprRomFile.o
	rm -rf macosx-x86_64-debug/obj/mprSelect.o
	rm -rf macosx-x86_64-debug/obj/mprSignal.o
	rm -rf macosx-x86_64-debug/obj/mprSocket.o
	rm -rf macosx-x86_64-debug/obj/mprString.o
	rm -rf macosx-x86_64-debug/obj/mprTest.o
	rm -rf macosx-x86_64-debug/obj/mprThread.o
	rm -rf macosx-x86_64-debug/obj/mprTime.o
	rm -rf macosx-x86_64-debug/obj/mprUnix.o
	rm -rf macosx-x86_64-debug/obj/mprVxworks.o
	rm -rf macosx-x86_64-debug/obj/mprWait.o
	rm -rf macosx-x86_64-debug/obj/mprWide.o
	rm -rf macosx-x86_64-debug/obj/mprWin.o
	rm -rf macosx-x86_64-debug/obj/mprWince.o
	rm -rf macosx-x86_64-debug/obj/mprXml.o
	rm -rf macosx-x86_64-debug/obj/mprMatrixssl.o
	rm -rf macosx-x86_64-debug/obj/mprOpenssl.o
	rm -rf macosx-x86_64-debug/obj/mprSsl.o
	rm -rf macosx-x86_64-debug/obj/manager.o
	rm -rf macosx-x86_64-debug/obj/makerom.o
	rm -rf macosx-x86_64-debug/obj/charGen.o

macosx-x86_64-debug/obj/dtoa.o: \
        src/dtoa.c \
        macosx-x86_64-debug/inc/bit.h
	$(CC) -c -o macosx-x86_64-debug/obj/dtoa.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/dtoa.c

macosx-x86_64-debug/obj/mpr.o: \
        src/mpr.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mpr.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mpr.c

macosx-x86_64-debug/obj/mprAsync.o: \
        src/mprAsync.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprAsync.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprAsync.c

macosx-x86_64-debug/obj/mprAtomic.o: \
        src/mprAtomic.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprAtomic.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprAtomic.c

macosx-x86_64-debug/obj/mprBuf.o: \
        src/mprBuf.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprBuf.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprBuf.c

macosx-x86_64-debug/obj/mprCache.o: \
        src/mprCache.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprCache.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprCache.c

macosx-x86_64-debug/obj/mprCmd.o: \
        src/mprCmd.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprCmd.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprCmd.c

macosx-x86_64-debug/obj/mprCond.o: \
        src/mprCond.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprCond.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprCond.c

macosx-x86_64-debug/obj/mprCrypt.o: \
        src/mprCrypt.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprCrypt.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprCrypt.c

macosx-x86_64-debug/obj/mprDisk.o: \
        src/mprDisk.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprDisk.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprDisk.c

macosx-x86_64-debug/obj/mprDispatcher.o: \
        src/mprDispatcher.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprDispatcher.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprDispatcher.c

macosx-x86_64-debug/obj/mprEncode.o: \
        src/mprEncode.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprEncode.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprEncode.c

macosx-x86_64-debug/obj/mprEpoll.o: \
        src/mprEpoll.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprEpoll.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprEpoll.c

macosx-x86_64-debug/obj/mprEvent.o: \
        src/mprEvent.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprEvent.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprEvent.c

macosx-x86_64-debug/obj/mprFile.o: \
        src/mprFile.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprFile.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprFile.c

macosx-x86_64-debug/obj/mprFileSystem.o: \
        src/mprFileSystem.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprFileSystem.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprFileSystem.c

macosx-x86_64-debug/obj/mprHash.o: \
        src/mprHash.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprHash.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprHash.c

macosx-x86_64-debug/obj/mprJSON.o: \
        src/mprJSON.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprJSON.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprJSON.c

macosx-x86_64-debug/obj/mprKqueue.o: \
        src/mprKqueue.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprKqueue.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprKqueue.c

macosx-x86_64-debug/obj/mprList.o: \
        src/mprList.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprList.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprList.c

macosx-x86_64-debug/obj/mprLock.o: \
        src/mprLock.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprLock.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprLock.c

macosx-x86_64-debug/obj/mprLog.o: \
        src/mprLog.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprLog.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprLog.c

macosx-x86_64-debug/obj/mprMem.o: \
        src/mprMem.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprMem.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprMem.c

macosx-x86_64-debug/obj/mprMime.o: \
        src/mprMime.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprMime.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprMime.c

macosx-x86_64-debug/obj/mprMixed.o: \
        src/mprMixed.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprMixed.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprMixed.c

macosx-x86_64-debug/obj/mprModule.o: \
        src/mprModule.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprModule.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprModule.c

macosx-x86_64-debug/obj/mprPath.o: \
        src/mprPath.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprPath.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprPath.c

macosx-x86_64-debug/obj/mprPoll.o: \
        src/mprPoll.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprPoll.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprPoll.c

macosx-x86_64-debug/obj/mprPrintf.o: \
        src/mprPrintf.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprPrintf.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprPrintf.c

macosx-x86_64-debug/obj/mprRomFile.o: \
        src/mprRomFile.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprRomFile.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprRomFile.c

macosx-x86_64-debug/obj/mprSelect.o: \
        src/mprSelect.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprSelect.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprSelect.c

macosx-x86_64-debug/obj/mprSignal.o: \
        src/mprSignal.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprSignal.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprSignal.c

macosx-x86_64-debug/obj/mprSocket.o: \
        src/mprSocket.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprSocket.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprSocket.c

macosx-x86_64-debug/obj/mprString.o: \
        src/mprString.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprString.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprString.c

macosx-x86_64-debug/obj/mprTest.o: \
        src/mprTest.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprTest.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprTest.c

macosx-x86_64-debug/obj/mprThread.o: \
        src/mprThread.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprThread.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprThread.c

macosx-x86_64-debug/obj/mprTime.o: \
        src/mprTime.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprTime.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprTime.c

macosx-x86_64-debug/obj/mprUnix.o: \
        src/mprUnix.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprUnix.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprUnix.c

macosx-x86_64-debug/obj/mprVxworks.o: \
        src/mprVxworks.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprVxworks.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprVxworks.c

macosx-x86_64-debug/obj/mprWait.o: \
        src/mprWait.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprWait.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprWait.c

macosx-x86_64-debug/obj/mprWide.o: \
        src/mprWide.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprWide.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprWide.c

macosx-x86_64-debug/obj/mprWin.o: \
        src/mprWin.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprWin.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprWin.c

macosx-x86_64-debug/obj/mprWince.o: \
        src/mprWince.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprWince.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprWince.c

macosx-x86_64-debug/obj/mprXml.o: \
        src/mprXml.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprXml.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/mprXml.c

macosx-x86_64-debug/lib/libmpr.dylib:  \
        macosx-x86_64-debug/obj/dtoa.o \
        macosx-x86_64-debug/obj/mpr.o \
        macosx-x86_64-debug/obj/mprAsync.o \
        macosx-x86_64-debug/obj/mprAtomic.o \
        macosx-x86_64-debug/obj/mprBuf.o \
        macosx-x86_64-debug/obj/mprCache.o \
        macosx-x86_64-debug/obj/mprCmd.o \
        macosx-x86_64-debug/obj/mprCond.o \
        macosx-x86_64-debug/obj/mprCrypt.o \
        macosx-x86_64-debug/obj/mprDisk.o \
        macosx-x86_64-debug/obj/mprDispatcher.o \
        macosx-x86_64-debug/obj/mprEncode.o \
        macosx-x86_64-debug/obj/mprEpoll.o \
        macosx-x86_64-debug/obj/mprEvent.o \
        macosx-x86_64-debug/obj/mprFile.o \
        macosx-x86_64-debug/obj/mprFileSystem.o \
        macosx-x86_64-debug/obj/mprHash.o \
        macosx-x86_64-debug/obj/mprJSON.o \
        macosx-x86_64-debug/obj/mprKqueue.o \
        macosx-x86_64-debug/obj/mprList.o \
        macosx-x86_64-debug/obj/mprLock.o \
        macosx-x86_64-debug/obj/mprLog.o \
        macosx-x86_64-debug/obj/mprMem.o \
        macosx-x86_64-debug/obj/mprMime.o \
        macosx-x86_64-debug/obj/mprMixed.o \
        macosx-x86_64-debug/obj/mprModule.o \
        macosx-x86_64-debug/obj/mprPath.o \
        macosx-x86_64-debug/obj/mprPoll.o \
        macosx-x86_64-debug/obj/mprPrintf.o \
        macosx-x86_64-debug/obj/mprRomFile.o \
        macosx-x86_64-debug/obj/mprSelect.o \
        macosx-x86_64-debug/obj/mprSignal.o \
        macosx-x86_64-debug/obj/mprSocket.o \
        macosx-x86_64-debug/obj/mprString.o \
        macosx-x86_64-debug/obj/mprTest.o \
        macosx-x86_64-debug/obj/mprThread.o \
        macosx-x86_64-debug/obj/mprTime.o \
        macosx-x86_64-debug/obj/mprUnix.o \
        macosx-x86_64-debug/obj/mprVxworks.o \
        macosx-x86_64-debug/obj/mprWait.o \
        macosx-x86_64-debug/obj/mprWide.o \
        macosx-x86_64-debug/obj/mprWin.o \
        macosx-x86_64-debug/obj/mprWince.o \
        macosx-x86_64-debug/obj/mprXml.o
	$(CC) -dynamiclib -o macosx-x86_64-debug/lib/libmpr.dylib -arch x86_64 $(LDFLAGS) -install_name @rpath/libmpr.dylib macosx-x86_64-debug/obj/dtoa.o macosx-x86_64-debug/obj/mpr.o macosx-x86_64-debug/obj/mprAsync.o macosx-x86_64-debug/obj/mprAtomic.o macosx-x86_64-debug/obj/mprBuf.o macosx-x86_64-debug/obj/mprCache.o macosx-x86_64-debug/obj/mprCmd.o macosx-x86_64-debug/obj/mprCond.o macosx-x86_64-debug/obj/mprCrypt.o macosx-x86_64-debug/obj/mprDisk.o macosx-x86_64-debug/obj/mprDispatcher.o macosx-x86_64-debug/obj/mprEncode.o macosx-x86_64-debug/obj/mprEpoll.o macosx-x86_64-debug/obj/mprEvent.o macosx-x86_64-debug/obj/mprFile.o macosx-x86_64-debug/obj/mprFileSystem.o macosx-x86_64-debug/obj/mprHash.o macosx-x86_64-debug/obj/mprJSON.o macosx-x86_64-debug/obj/mprKqueue.o macosx-x86_64-debug/obj/mprList.o macosx-x86_64-debug/obj/mprLock.o macosx-x86_64-debug/obj/mprLog.o macosx-x86_64-debug/obj/mprMem.o macosx-x86_64-debug/obj/mprMime.o macosx-x86_64-debug/obj/mprMixed.o macosx-x86_64-debug/obj/mprModule.o macosx-x86_64-debug/obj/mprPath.o macosx-x86_64-debug/obj/mprPoll.o macosx-x86_64-debug/obj/mprPrintf.o macosx-x86_64-debug/obj/mprRomFile.o macosx-x86_64-debug/obj/mprSelect.o macosx-x86_64-debug/obj/mprSignal.o macosx-x86_64-debug/obj/mprSocket.o macosx-x86_64-debug/obj/mprString.o macosx-x86_64-debug/obj/mprTest.o macosx-x86_64-debug/obj/mprThread.o macosx-x86_64-debug/obj/mprTime.o macosx-x86_64-debug/obj/mprUnix.o macosx-x86_64-debug/obj/mprVxworks.o macosx-x86_64-debug/obj/mprWait.o macosx-x86_64-debug/obj/mprWide.o macosx-x86_64-debug/obj/mprWin.o macosx-x86_64-debug/obj/mprWince.o macosx-x86_64-debug/obj/mprXml.o $(LIBS)

macosx-x86_64-debug/obj/benchMpr.o: \
        test/benchMpr.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/benchMpr.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/benchMpr.c

macosx-x86_64-debug/bin/benchMpr:  \
        macosx-x86_64-debug/lib/libmpr.dylib \
        macosx-x86_64-debug/obj/benchMpr.o
	$(CC) -o macosx-x86_64-debug/bin/benchMpr -arch x86_64 $(LDFLAGS) -Lmacosx-x86_64-debug/lib macosx-x86_64-debug/obj/benchMpr.o $(LIBS) -lmpr

macosx-x86_64-debug/obj/runProgram.o: \
        test/runProgram.c \
        macosx-x86_64-debug/inc/bit.h \
        macosx-x86_64-debug/inc/buildConfig.h
	$(CC) -c -o macosx-x86_64-debug/obj/runProgram.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/runProgram.c

macosx-x86_64-debug/bin/runProgram:  \
        macosx-x86_64-debug/obj/runProgram.o
	$(CC) -o macosx-x86_64-debug/bin/runProgram -arch x86_64 $(LDFLAGS) -Lmacosx-x86_64-debug/lib macosx-x86_64-debug/obj/runProgram.o $(LIBS)

macosx-x86_64-debug/obj/mprMatrixssl.o: \
        src/mprMatrixssl.c \
        macosx-x86_64-debug/inc/bit.h \
        macosx-x86_64-debug/inc/buildConfig.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprMatrixssl.o -arch x86_64 $(CFLAGS) $(DFLAGS) -DPOSIX -DMATRIX_USE_FILE_SYSTEM -Imacosx-x86_64-debug/inc -Isrc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprMatrixssl.c

macosx-x86_64-debug/obj/mprOpenssl.o: \
        src/mprOpenssl.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprOpenssl.o -arch x86_64 $(CFLAGS) $(DFLAGS) -DPOSIX -DMATRIX_USE_FILE_SYSTEM -Imacosx-x86_64-debug/inc -Isrc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprOpenssl.c

macosx-x86_64-debug/obj/mprSsl.o: \
        src/mprSsl.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/mprSsl.o -arch x86_64 $(CFLAGS) $(DFLAGS) -DPOSIX -DMATRIX_USE_FILE_SYSTEM -Imacosx-x86_64-debug/inc -Isrc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprSsl.c

macosx-x86_64-debug/lib/libmprssl.dylib:  \
        macosx-x86_64-debug/lib/libmpr.dylib \
        macosx-x86_64-debug/obj/mprMatrixssl.o \
        macosx-x86_64-debug/obj/mprOpenssl.o \
        macosx-x86_64-debug/obj/mprSsl.o
	$(CC) -dynamiclib -o macosx-x86_64-debug/lib/libmprssl.dylib -arch x86_64 $(LDFLAGS) -L/Users/mob/git/packages-macosx-x86_64/openssl/openssl-1.0.0d -L/Users/mob/git/packages-macosx-x86_64/matrixssl/matrixssl-3-3-open -install_name @rpath/libmprssl.dylib macosx-x86_64-debug/obj/mprMatrixssl.o macosx-x86_64-debug/obj/mprOpenssl.o macosx-x86_64-debug/obj/mprSsl.o $(LIBS) -lmpr -lssl -lcrypto -lmatrixssl

macosx-x86_64-debug/obj/testArgv.o: \
        test/testArgv.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testArgv.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testArgv.c

macosx-x86_64-debug/obj/testBuf.o: \
        test/testBuf.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testBuf.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testBuf.c

macosx-x86_64-debug/obj/testCmd.o: \
        test/testCmd.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testCmd.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testCmd.c

macosx-x86_64-debug/obj/testCond.o: \
        test/testCond.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testCond.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testCond.c

macosx-x86_64-debug/obj/testEvent.o: \
        test/testEvent.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testEvent.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testEvent.c

macosx-x86_64-debug/obj/testFile.o: \
        test/testFile.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testFile.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testFile.c

macosx-x86_64-debug/obj/testHash.o: \
        test/testHash.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testHash.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testHash.c

macosx-x86_64-debug/obj/testList.o: \
        test/testList.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testList.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testList.c

macosx-x86_64-debug/obj/testLock.o: \
        test/testLock.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testLock.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testLock.c

macosx-x86_64-debug/obj/testMem.o: \
        test/testMem.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testMem.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testMem.c

macosx-x86_64-debug/obj/testMpr.o: \
        test/testMpr.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testMpr.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testMpr.c

macosx-x86_64-debug/obj/testPath.o: \
        test/testPath.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testPath.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testPath.c

macosx-x86_64-debug/obj/testSocket.o: \
        test/testSocket.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testSocket.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testSocket.c

macosx-x86_64-debug/obj/testSprintf.o: \
        test/testSprintf.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testSprintf.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testSprintf.c

macosx-x86_64-debug/obj/testThread.o: \
        test/testThread.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testThread.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testThread.c

macosx-x86_64-debug/obj/testTime.o: \
        test/testTime.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testTime.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testTime.c

macosx-x86_64-debug/obj/testUnicode.o: \
        test/testUnicode.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/testUnicode.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc test/testUnicode.c

macosx-x86_64-debug/bin/testMpr:  \
        macosx-x86_64-debug/lib/libmpr.dylib \
        macosx-x86_64-debug/lib/libmprssl.dylib \
        macosx-x86_64-debug/bin/runProgram \
        macosx-x86_64-debug/obj/testArgv.o \
        macosx-x86_64-debug/obj/testBuf.o \
        macosx-x86_64-debug/obj/testCmd.o \
        macosx-x86_64-debug/obj/testCond.o \
        macosx-x86_64-debug/obj/testEvent.o \
        macosx-x86_64-debug/obj/testFile.o \
        macosx-x86_64-debug/obj/testHash.o \
        macosx-x86_64-debug/obj/testList.o \
        macosx-x86_64-debug/obj/testLock.o \
        macosx-x86_64-debug/obj/testMem.o \
        macosx-x86_64-debug/obj/testMpr.o \
        macosx-x86_64-debug/obj/testPath.o \
        macosx-x86_64-debug/obj/testSocket.o \
        macosx-x86_64-debug/obj/testSprintf.o \
        macosx-x86_64-debug/obj/testThread.o \
        macosx-x86_64-debug/obj/testTime.o \
        macosx-x86_64-debug/obj/testUnicode.o
	$(CC) -o macosx-x86_64-debug/bin/testMpr -arch x86_64 $(LDFLAGS) -L/Users/mob/git/packages-macosx-x86_64/openssl/openssl-1.0.0d -L/Users/mob/git/packages-macosx-x86_64/matrixssl/matrixssl-3-3-open -Lmacosx-x86_64-debug/lib macosx-x86_64-debug/obj/testArgv.o macosx-x86_64-debug/obj/testBuf.o macosx-x86_64-debug/obj/testCmd.o macosx-x86_64-debug/obj/testCond.o macosx-x86_64-debug/obj/testEvent.o macosx-x86_64-debug/obj/testFile.o macosx-x86_64-debug/obj/testHash.o macosx-x86_64-debug/obj/testList.o macosx-x86_64-debug/obj/testLock.o macosx-x86_64-debug/obj/testMem.o macosx-x86_64-debug/obj/testMpr.o macosx-x86_64-debug/obj/testPath.o macosx-x86_64-debug/obj/testSocket.o macosx-x86_64-debug/obj/testSprintf.o macosx-x86_64-debug/obj/testThread.o macosx-x86_64-debug/obj/testTime.o macosx-x86_64-debug/obj/testUnicode.o $(LIBS) -lmpr -lmprssl -lssl -lcrypto -lmatrixssl

macosx-x86_64-debug/obj/manager.o: \
        src/manager.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/manager.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/manager.c

macosx-x86_64-debug/bin/manager:  \
        macosx-x86_64-debug/lib/libmpr.dylib \
        macosx-x86_64-debug/obj/manager.o
	$(CC) -o macosx-x86_64-debug/bin/manager -arch x86_64 $(LDFLAGS) -Lmacosx-x86_64-debug/lib macosx-x86_64-debug/obj/manager.o $(LIBS) -lmpr

macosx-x86_64-debug/obj/makerom.o: \
        src/utils/makerom.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/makerom.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/utils/makerom.c

macosx-x86_64-debug/bin/makerom:  \
        macosx-x86_64-debug/lib/libmpr.dylib \
        macosx-x86_64-debug/obj/makerom.o
	$(CC) -o macosx-x86_64-debug/bin/makerom -arch x86_64 $(LDFLAGS) -Lmacosx-x86_64-debug/lib macosx-x86_64-debug/obj/makerom.o $(LIBS) -lmpr

macosx-x86_64-debug/obj/charGen.o: \
        src/utils/charGen.c \
        macosx-x86_64-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-x86_64-debug/obj/charGen.o -arch x86_64 $(CFLAGS) $(DFLAGS) -Imacosx-x86_64-debug/inc -Isrc src/utils/charGen.c

macosx-x86_64-debug/bin/chargen:  \
        macosx-x86_64-debug/lib/libmpr.dylib \
        macosx-x86_64-debug/obj/charGen.o
	$(CC) -o macosx-x86_64-debug/bin/chargen -arch x86_64 $(LDFLAGS) -Lmacosx-x86_64-debug/lib macosx-x86_64-debug/obj/charGen.o $(LIBS) -lmpr

