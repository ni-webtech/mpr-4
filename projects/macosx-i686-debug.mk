#
#   build.mk -- Build It Makefile to build Multithreaded Portable Runtime for macosx on i686
#

CC        := /usr/bin/cc
CFLAGS    := -fPIC -Wall -g -Wshorten-64-to-32
DFLAGS    := -DPIC -DCPU=I686
IFLAGS    := -Isrc -Imacosx-i686-debug/inc
LDFLAGS   := -Lmacosx-i686-debug/lib -Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -g -ldl
LIBS      := -lpthread -lm

all: \
        macosx-i686-debug/bin/benchMpr \
        macosx-i686-debug/bin/runProgram \
        macosx-i686-debug/bin/testMpr \
        macosx-i686-debug/lib/libmpr.dylib \
        macosx-i686-debug/bin/manager \
        macosx-i686-debug/bin/makerom \
        macosx-i686-debug/bin/chargen

clean:
	rm -f macosx-i686-debug/bin/benchMpr
	rm -f macosx-i686-debug/bin/runProgram
	rm -f macosx-i686-debug/bin/testMpr
	rm -f macosx-i686-debug/lib/libmpr.dylib
	rm -f macosx-i686-debug/lib/libmprssl.dylib
	rm -f macosx-i686-debug/bin/manager
	rm -f macosx-i686-debug/bin/makerom
	rm -f macosx-i686-debug/bin/chargen
	rm -f macosx-i686-debug/obj/benchMpr.o
	rm -f macosx-i686-debug/obj/runProgram.o
	rm -f macosx-i686-debug/obj/testArgv.o
	rm -f macosx-i686-debug/obj/testBuf.o
	rm -f macosx-i686-debug/obj/testCmd.o
	rm -f macosx-i686-debug/obj/testCond.o
	rm -f macosx-i686-debug/obj/testEvent.o
	rm -f macosx-i686-debug/obj/testFile.o
	rm -f macosx-i686-debug/obj/testHash.o
	rm -f macosx-i686-debug/obj/testList.o
	rm -f macosx-i686-debug/obj/testLock.o
	rm -f macosx-i686-debug/obj/testMem.o
	rm -f macosx-i686-debug/obj/testMpr.o
	rm -f macosx-i686-debug/obj/testPath.o
	rm -f macosx-i686-debug/obj/testSocket.o
	rm -f macosx-i686-debug/obj/testSprintf.o
	rm -f macosx-i686-debug/obj/testThread.o
	rm -f macosx-i686-debug/obj/testTime.o
	rm -f macosx-i686-debug/obj/testUnicode.o
	rm -f macosx-i686-debug/obj/dtoa.o
	rm -f macosx-i686-debug/obj/mpr.o
	rm -f macosx-i686-debug/obj/mprAsync.o
	rm -f macosx-i686-debug/obj/mprAtomic.o
	rm -f macosx-i686-debug/obj/mprBuf.o
	rm -f macosx-i686-debug/obj/mprCache.o
	rm -f macosx-i686-debug/obj/mprCmd.o
	rm -f macosx-i686-debug/obj/mprCond.o
	rm -f macosx-i686-debug/obj/mprCrypt.o
	rm -f macosx-i686-debug/obj/mprDisk.o
	rm -f macosx-i686-debug/obj/mprDispatcher.o
	rm -f macosx-i686-debug/obj/mprEncode.o
	rm -f macosx-i686-debug/obj/mprEpoll.o
	rm -f macosx-i686-debug/obj/mprEvent.o
	rm -f macosx-i686-debug/obj/mprFile.o
	rm -f macosx-i686-debug/obj/mprFileSystem.o
	rm -f macosx-i686-debug/obj/mprHash.o
	rm -f macosx-i686-debug/obj/mprJSON.o
	rm -f macosx-i686-debug/obj/mprKqueue.o
	rm -f macosx-i686-debug/obj/mprList.o
	rm -f macosx-i686-debug/obj/mprLock.o
	rm -f macosx-i686-debug/obj/mprLog.o
	rm -f macosx-i686-debug/obj/mprMem.o
	rm -f macosx-i686-debug/obj/mprMime.o
	rm -f macosx-i686-debug/obj/mprMixed.o
	rm -f macosx-i686-debug/obj/mprModule.o
	rm -f macosx-i686-debug/obj/mprPath.o
	rm -f macosx-i686-debug/obj/mprPoll.o
	rm -f macosx-i686-debug/obj/mprPrintf.o
	rm -f macosx-i686-debug/obj/mprRomFile.o
	rm -f macosx-i686-debug/obj/mprSelect.o
	rm -f macosx-i686-debug/obj/mprSignal.o
	rm -f macosx-i686-debug/obj/mprSocket.o
	rm -f macosx-i686-debug/obj/mprString.o
	rm -f macosx-i686-debug/obj/mprTest.o
	rm -f macosx-i686-debug/obj/mprThread.o
	rm -f macosx-i686-debug/obj/mprTime.o
	rm -f macosx-i686-debug/obj/mprUnix.o
	rm -f macosx-i686-debug/obj/mprVxworks.o
	rm -f macosx-i686-debug/obj/mprWait.o
	rm -f macosx-i686-debug/obj/mprWide.o
	rm -f macosx-i686-debug/obj/mprWin.o
	rm -f macosx-i686-debug/obj/mprWince.o
	rm -f macosx-i686-debug/obj/mprXml.o
	rm -f macosx-i686-debug/obj/mprMatrixssl.o
	rm -f macosx-i686-debug/obj/mprOpenssl.o
	rm -f macosx-i686-debug/obj/mprSsl.o
	rm -f macosx-i686-debug/obj/manager.o
	rm -f macosx-i686-debug/obj/makerom.o
	rm -f macosx-i686-debug/obj/charGen.o

macosx-i686-debug/obj/dtoa.o: \
        src/dtoa.c \
        macosx-i686-debug/inc/bit.h
	$(CC) -c -o macosx-i686-debug/obj/dtoa.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/dtoa.c

macosx-i686-debug/obj/mpr.o: \
        src/mpr.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mpr.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mpr.c

macosx-i686-debug/obj/mprAsync.o: \
        src/mprAsync.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprAsync.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprAsync.c

macosx-i686-debug/obj/mprAtomic.o: \
        src/mprAtomic.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprAtomic.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprAtomic.c

macosx-i686-debug/obj/mprBuf.o: \
        src/mprBuf.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprBuf.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprBuf.c

macosx-i686-debug/obj/mprCache.o: \
        src/mprCache.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprCache.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprCache.c

macosx-i686-debug/obj/mprCmd.o: \
        src/mprCmd.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprCmd.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprCmd.c

macosx-i686-debug/obj/mprCond.o: \
        src/mprCond.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprCond.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprCond.c

macosx-i686-debug/obj/mprCrypt.o: \
        src/mprCrypt.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprCrypt.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprCrypt.c

macosx-i686-debug/obj/mprDisk.o: \
        src/mprDisk.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprDisk.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprDisk.c

macosx-i686-debug/obj/mprDispatcher.o: \
        src/mprDispatcher.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprDispatcher.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprDispatcher.c

macosx-i686-debug/obj/mprEncode.o: \
        src/mprEncode.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprEncode.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprEncode.c

macosx-i686-debug/obj/mprEpoll.o: \
        src/mprEpoll.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprEpoll.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprEpoll.c

macosx-i686-debug/obj/mprEvent.o: \
        src/mprEvent.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprEvent.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprEvent.c

macosx-i686-debug/obj/mprFile.o: \
        src/mprFile.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprFile.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprFile.c

macosx-i686-debug/obj/mprFileSystem.o: \
        src/mprFileSystem.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprFileSystem.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprFileSystem.c

macosx-i686-debug/obj/mprHash.o: \
        src/mprHash.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprHash.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprHash.c

macosx-i686-debug/obj/mprJSON.o: \
        src/mprJSON.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprJSON.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprJSON.c

macosx-i686-debug/obj/mprKqueue.o: \
        src/mprKqueue.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprKqueue.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprKqueue.c

macosx-i686-debug/obj/mprList.o: \
        src/mprList.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprList.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprList.c

macosx-i686-debug/obj/mprLock.o: \
        src/mprLock.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprLock.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprLock.c

macosx-i686-debug/obj/mprLog.o: \
        src/mprLog.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprLog.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprLog.c

macosx-i686-debug/obj/mprMem.o: \
        src/mprMem.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprMem.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprMem.c

macosx-i686-debug/obj/mprMime.o: \
        src/mprMime.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprMime.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprMime.c

macosx-i686-debug/obj/mprMixed.o: \
        src/mprMixed.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprMixed.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprMixed.c

macosx-i686-debug/obj/mprModule.o: \
        src/mprModule.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprModule.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprModule.c

macosx-i686-debug/obj/mprPath.o: \
        src/mprPath.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprPath.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprPath.c

macosx-i686-debug/obj/mprPoll.o: \
        src/mprPoll.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprPoll.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprPoll.c

macosx-i686-debug/obj/mprPrintf.o: \
        src/mprPrintf.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprPrintf.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprPrintf.c

macosx-i686-debug/obj/mprRomFile.o: \
        src/mprRomFile.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprRomFile.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprRomFile.c

macosx-i686-debug/obj/mprSelect.o: \
        src/mprSelect.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprSelect.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprSelect.c

macosx-i686-debug/obj/mprSignal.o: \
        src/mprSignal.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprSignal.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprSignal.c

macosx-i686-debug/obj/mprSocket.o: \
        src/mprSocket.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprSocket.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprSocket.c

macosx-i686-debug/obj/mprString.o: \
        src/mprString.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprString.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprString.c

macosx-i686-debug/obj/mprTest.o: \
        src/mprTest.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprTest.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprTest.c

macosx-i686-debug/obj/mprThread.o: \
        src/mprThread.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprThread.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprThread.c

macosx-i686-debug/obj/mprTime.o: \
        src/mprTime.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprTime.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprTime.c

macosx-i686-debug/obj/mprUnix.o: \
        src/mprUnix.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprUnix.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprUnix.c

macosx-i686-debug/obj/mprVxworks.o: \
        src/mprVxworks.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprVxworks.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprVxworks.c

macosx-i686-debug/obj/mprWait.o: \
        src/mprWait.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprWait.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprWait.c

macosx-i686-debug/obj/mprWide.o: \
        src/mprWide.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprWide.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprWide.c

macosx-i686-debug/obj/mprWin.o: \
        src/mprWin.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprWin.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprWin.c

macosx-i686-debug/obj/mprWince.o: \
        src/mprWince.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprWince.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprWince.c

macosx-i686-debug/obj/mprXml.o: \
        src/mprXml.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/mprXml.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/mprXml.c

macosx-i686-debug/lib/libmpr.dylib:  \
        macosx-i686-debug/obj/dtoa.o \
        macosx-i686-debug/obj/mpr.o \
        macosx-i686-debug/obj/mprAsync.o \
        macosx-i686-debug/obj/mprAtomic.o \
        macosx-i686-debug/obj/mprBuf.o \
        macosx-i686-debug/obj/mprCache.o \
        macosx-i686-debug/obj/mprCmd.o \
        macosx-i686-debug/obj/mprCond.o \
        macosx-i686-debug/obj/mprCrypt.o \
        macosx-i686-debug/obj/mprDisk.o \
        macosx-i686-debug/obj/mprDispatcher.o \
        macosx-i686-debug/obj/mprEncode.o \
        macosx-i686-debug/obj/mprEpoll.o \
        macosx-i686-debug/obj/mprEvent.o \
        macosx-i686-debug/obj/mprFile.o \
        macosx-i686-debug/obj/mprFileSystem.o \
        macosx-i686-debug/obj/mprHash.o \
        macosx-i686-debug/obj/mprJSON.o \
        macosx-i686-debug/obj/mprKqueue.o \
        macosx-i686-debug/obj/mprList.o \
        macosx-i686-debug/obj/mprLock.o \
        macosx-i686-debug/obj/mprLog.o \
        macosx-i686-debug/obj/mprMem.o \
        macosx-i686-debug/obj/mprMime.o \
        macosx-i686-debug/obj/mprMixed.o \
        macosx-i686-debug/obj/mprModule.o \
        macosx-i686-debug/obj/mprPath.o \
        macosx-i686-debug/obj/mprPoll.o \
        macosx-i686-debug/obj/mprPrintf.o \
        macosx-i686-debug/obj/mprRomFile.o \
        macosx-i686-debug/obj/mprSelect.o \
        macosx-i686-debug/obj/mprSignal.o \
        macosx-i686-debug/obj/mprSocket.o \
        macosx-i686-debug/obj/mprString.o \
        macosx-i686-debug/obj/mprTest.o \
        macosx-i686-debug/obj/mprThread.o \
        macosx-i686-debug/obj/mprTime.o \
        macosx-i686-debug/obj/mprUnix.o \
        macosx-i686-debug/obj/mprVxworks.o \
        macosx-i686-debug/obj/mprWait.o \
        macosx-i686-debug/obj/mprWide.o \
        macosx-i686-debug/obj/mprWin.o \
        macosx-i686-debug/obj/mprWince.o \
        macosx-i686-debug/obj/mprXml.o
	$(CC) -dynamiclib -o macosx-i686-debug/lib/libmpr.dylib -arch i686 $(LDFLAGS) -install_name @rpath/libmpr.dylib macosx-i686-debug/obj/dtoa.o macosx-i686-debug/obj/mpr.o macosx-i686-debug/obj/mprAsync.o macosx-i686-debug/obj/mprAtomic.o macosx-i686-debug/obj/mprBuf.o macosx-i686-debug/obj/mprCache.o macosx-i686-debug/obj/mprCmd.o macosx-i686-debug/obj/mprCond.o macosx-i686-debug/obj/mprCrypt.o macosx-i686-debug/obj/mprDisk.o macosx-i686-debug/obj/mprDispatcher.o macosx-i686-debug/obj/mprEncode.o macosx-i686-debug/obj/mprEpoll.o macosx-i686-debug/obj/mprEvent.o macosx-i686-debug/obj/mprFile.o macosx-i686-debug/obj/mprFileSystem.o macosx-i686-debug/obj/mprHash.o macosx-i686-debug/obj/mprJSON.o macosx-i686-debug/obj/mprKqueue.o macosx-i686-debug/obj/mprList.o macosx-i686-debug/obj/mprLock.o macosx-i686-debug/obj/mprLog.o macosx-i686-debug/obj/mprMem.o macosx-i686-debug/obj/mprMime.o macosx-i686-debug/obj/mprMixed.o macosx-i686-debug/obj/mprModule.o macosx-i686-debug/obj/mprPath.o macosx-i686-debug/obj/mprPoll.o macosx-i686-debug/obj/mprPrintf.o macosx-i686-debug/obj/mprRomFile.o macosx-i686-debug/obj/mprSelect.o macosx-i686-debug/obj/mprSignal.o macosx-i686-debug/obj/mprSocket.o macosx-i686-debug/obj/mprString.o macosx-i686-debug/obj/mprTest.o macosx-i686-debug/obj/mprThread.o macosx-i686-debug/obj/mprTime.o macosx-i686-debug/obj/mprUnix.o macosx-i686-debug/obj/mprVxworks.o macosx-i686-debug/obj/mprWait.o macosx-i686-debug/obj/mprWide.o macosx-i686-debug/obj/mprWin.o macosx-i686-debug/obj/mprWince.o macosx-i686-debug/obj/mprXml.o $(LIBS)

macosx-i686-debug/obj/benchMpr.o: \
        src/test/benchMpr.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/benchMpr.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/benchMpr.c

macosx-i686-debug/bin/benchMpr:  \
        macosx-i686-debug/lib/libmpr.dylib \
        macosx-i686-debug/obj/benchMpr.o
	$(CC) -o macosx-i686-debug/bin/benchMpr -arch i686 $(LDFLAGS) -Lmacosx-i686-debug/lib macosx-i686-debug/obj/benchMpr.o $(LIBS) -lmpr

macosx-i686-debug/obj/runProgram.o: \
        src/test/runProgram.c \
        macosx-i686-debug/inc/bit.h \
        macosx-i686-debug/inc/buildConfig.h
	$(CC) -c -o macosx-i686-debug/obj/runProgram.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/runProgram.c

macosx-i686-debug/bin/runProgram:  \
        macosx-i686-debug/obj/runProgram.o
	$(CC) -o macosx-i686-debug/bin/runProgram -arch i686 $(LDFLAGS) -Lmacosx-i686-debug/lib macosx-i686-debug/obj/runProgram.o $(LIBS)

macosx-i686-debug/obj/testArgv.o: \
        src/test/testArgv.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testArgv.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testArgv.c

macosx-i686-debug/obj/testBuf.o: \
        src/test/testBuf.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testBuf.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testBuf.c

macosx-i686-debug/obj/testCmd.o: \
        src/test/testCmd.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testCmd.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testCmd.c

macosx-i686-debug/obj/testCond.o: \
        src/test/testCond.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testCond.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testCond.c

macosx-i686-debug/obj/testEvent.o: \
        src/test/testEvent.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testEvent.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testEvent.c

macosx-i686-debug/obj/testFile.o: \
        src/test/testFile.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testFile.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testFile.c

macosx-i686-debug/obj/testHash.o: \
        src/test/testHash.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testHash.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testHash.c

macosx-i686-debug/obj/testList.o: \
        src/test/testList.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testList.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testList.c

macosx-i686-debug/obj/testLock.o: \
        src/test/testLock.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testLock.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testLock.c

macosx-i686-debug/obj/testMem.o: \
        src/test/testMem.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testMem.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testMem.c

macosx-i686-debug/obj/testMpr.o: \
        src/test/testMpr.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testMpr.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testMpr.c

macosx-i686-debug/obj/testPath.o: \
        src/test/testPath.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testPath.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testPath.c

macosx-i686-debug/obj/testSocket.o: \
        src/test/testSocket.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testSocket.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testSocket.c

macosx-i686-debug/obj/testSprintf.o: \
        src/test/testSprintf.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testSprintf.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testSprintf.c

macosx-i686-debug/obj/testThread.o: \
        src/test/testThread.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testThread.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testThread.c

macosx-i686-debug/obj/testTime.o: \
        src/test/testTime.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testTime.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testTime.c

macosx-i686-debug/obj/testUnicode.o: \
        src/test/testUnicode.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/testUnicode.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/test/testUnicode.c

macosx-i686-debug/bin/testMpr:  \
        macosx-i686-debug/lib/libmpr.dylib \
        macosx-i686-debug/bin/runProgram \
        macosx-i686-debug/obj/testArgv.o \
        macosx-i686-debug/obj/testBuf.o \
        macosx-i686-debug/obj/testCmd.o \
        macosx-i686-debug/obj/testCond.o \
        macosx-i686-debug/obj/testEvent.o \
        macosx-i686-debug/obj/testFile.o \
        macosx-i686-debug/obj/testHash.o \
        macosx-i686-debug/obj/testList.o \
        macosx-i686-debug/obj/testLock.o \
        macosx-i686-debug/obj/testMem.o \
        macosx-i686-debug/obj/testMpr.o \
        macosx-i686-debug/obj/testPath.o \
        macosx-i686-debug/obj/testSocket.o \
        macosx-i686-debug/obj/testSprintf.o \
        macosx-i686-debug/obj/testThread.o \
        macosx-i686-debug/obj/testTime.o \
        macosx-i686-debug/obj/testUnicode.o
	$(CC) -o macosx-i686-debug/bin/testMpr -arch i686 $(LDFLAGS) -Lmacosx-i686-debug/lib macosx-i686-debug/obj/testArgv.o macosx-i686-debug/obj/testBuf.o macosx-i686-debug/obj/testCmd.o macosx-i686-debug/obj/testCond.o macosx-i686-debug/obj/testEvent.o macosx-i686-debug/obj/testFile.o macosx-i686-debug/obj/testHash.o macosx-i686-debug/obj/testList.o macosx-i686-debug/obj/testLock.o macosx-i686-debug/obj/testMem.o macosx-i686-debug/obj/testMpr.o macosx-i686-debug/obj/testPath.o macosx-i686-debug/obj/testSocket.o macosx-i686-debug/obj/testSprintf.o macosx-i686-debug/obj/testThread.o macosx-i686-debug/obj/testTime.o macosx-i686-debug/obj/testUnicode.o $(LIBS) -lmpr

macosx-i686-debug/obj/manager.o: \
        src/manager.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/manager.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/manager.c

macosx-i686-debug/bin/manager:  \
        macosx-i686-debug/lib/libmpr.dylib \
        macosx-i686-debug/obj/manager.o
	$(CC) -o macosx-i686-debug/bin/manager -arch i686 $(LDFLAGS) -Lmacosx-i686-debug/lib macosx-i686-debug/obj/manager.o $(LIBS) -lmpr

macosx-i686-debug/obj/makerom.o: \
        src/utils/makerom.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/makerom.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/utils/makerom.c

macosx-i686-debug/bin/makerom:  \
        macosx-i686-debug/lib/libmpr.dylib \
        macosx-i686-debug/obj/makerom.o
	$(CC) -o macosx-i686-debug/bin/makerom -arch i686 $(LDFLAGS) -Lmacosx-i686-debug/lib macosx-i686-debug/obj/makerom.o $(LIBS) -lmpr

macosx-i686-debug/obj/charGen.o: \
        src/utils/charGen.c \
        macosx-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o macosx-i686-debug/obj/charGen.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug/inc src/utils/charGen.c

macosx-i686-debug/bin/chargen:  \
        macosx-i686-debug/lib/libmpr.dylib \
        macosx-i686-debug/obj/charGen.o
	$(CC) -o macosx-i686-debug/bin/chargen -arch i686 $(LDFLAGS) -Lmacosx-i686-debug/lib macosx-i686-debug/obj/charGen.o $(LIBS) -lmpr

