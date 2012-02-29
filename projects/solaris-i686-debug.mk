#
#   build.mk -- Build It Makefile to build Multithreaded Portable Runtime for solaris on i686
#

CC        := cc
CFLAGS    := -fPIC -g -mcpu=i686
DFLAGS    := -DPIC
IFLAGS    := -Isrc -Isolaris-i686-debug/inc
LDFLAGS   := -L/Users/mob/git/mpr/solaris-i686-debug/lib -g
LIBS      := -lpthread -lm

all: \
        solaris-i686-debug/bin/benchMpr \
        solaris-i686-debug/bin/runProgram \
        solaris-i686-debug/bin/testMpr \
        solaris-i686-debug/lib/libmpr.so \
        solaris-i686-debug/bin/manager \
        solaris-i686-debug/bin/makerom \
        solaris-i686-debug/bin/chargen

clean:
	rm -f solaris-i686-debug/bin/benchMpr
	rm -f solaris-i686-debug/bin/runProgram
	rm -f solaris-i686-debug/bin/testMpr
	rm -f solaris-i686-debug/lib/libmpr.so
	rm -f solaris-i686-debug/lib/libmprssl.so
	rm -f solaris-i686-debug/bin/manager
	rm -f solaris-i686-debug/bin/makerom
	rm -f solaris-i686-debug/bin/chargen
	rm -f solaris-i686-debug/obj/benchMpr.o
	rm -f solaris-i686-debug/obj/runProgram.o
	rm -f solaris-i686-debug/obj/testArgv.o
	rm -f solaris-i686-debug/obj/testBuf.o
	rm -f solaris-i686-debug/obj/testCmd.o
	rm -f solaris-i686-debug/obj/testCond.o
	rm -f solaris-i686-debug/obj/testEvent.o
	rm -f solaris-i686-debug/obj/testFile.o
	rm -f solaris-i686-debug/obj/testHash.o
	rm -f solaris-i686-debug/obj/testList.o
	rm -f solaris-i686-debug/obj/testLock.o
	rm -f solaris-i686-debug/obj/testMem.o
	rm -f solaris-i686-debug/obj/testMpr.o
	rm -f solaris-i686-debug/obj/testPath.o
	rm -f solaris-i686-debug/obj/testSocket.o
	rm -f solaris-i686-debug/obj/testSprintf.o
	rm -f solaris-i686-debug/obj/testThread.o
	rm -f solaris-i686-debug/obj/testTime.o
	rm -f solaris-i686-debug/obj/testUnicode.o
	rm -f solaris-i686-debug/obj/dtoa.o
	rm -f solaris-i686-debug/obj/mpr.o
	rm -f solaris-i686-debug/obj/mprAsync.o
	rm -f solaris-i686-debug/obj/mprAtomic.o
	rm -f solaris-i686-debug/obj/mprBuf.o
	rm -f solaris-i686-debug/obj/mprCache.o
	rm -f solaris-i686-debug/obj/mprCmd.o
	rm -f solaris-i686-debug/obj/mprCond.o
	rm -f solaris-i686-debug/obj/mprCrypt.o
	rm -f solaris-i686-debug/obj/mprDisk.o
	rm -f solaris-i686-debug/obj/mprDispatcher.o
	rm -f solaris-i686-debug/obj/mprEncode.o
	rm -f solaris-i686-debug/obj/mprEpoll.o
	rm -f solaris-i686-debug/obj/mprEvent.o
	rm -f solaris-i686-debug/obj/mprFile.o
	rm -f solaris-i686-debug/obj/mprFileSystem.o
	rm -f solaris-i686-debug/obj/mprHash.o
	rm -f solaris-i686-debug/obj/mprJSON.o
	rm -f solaris-i686-debug/obj/mprKqueue.o
	rm -f solaris-i686-debug/obj/mprList.o
	rm -f solaris-i686-debug/obj/mprLock.o
	rm -f solaris-i686-debug/obj/mprLog.o
	rm -f solaris-i686-debug/obj/mprMem.o
	rm -f solaris-i686-debug/obj/mprMime.o
	rm -f solaris-i686-debug/obj/mprMixed.o
	rm -f solaris-i686-debug/obj/mprModule.o
	rm -f solaris-i686-debug/obj/mprPath.o
	rm -f solaris-i686-debug/obj/mprPoll.o
	rm -f solaris-i686-debug/obj/mprPrintf.o
	rm -f solaris-i686-debug/obj/mprRomFile.o
	rm -f solaris-i686-debug/obj/mprSelect.o
	rm -f solaris-i686-debug/obj/mprSignal.o
	rm -f solaris-i686-debug/obj/mprSocket.o
	rm -f solaris-i686-debug/obj/mprString.o
	rm -f solaris-i686-debug/obj/mprTest.o
	rm -f solaris-i686-debug/obj/mprThread.o
	rm -f solaris-i686-debug/obj/mprTime.o
	rm -f solaris-i686-debug/obj/mprUnix.o
	rm -f solaris-i686-debug/obj/mprVxworks.o
	rm -f solaris-i686-debug/obj/mprWait.o
	rm -f solaris-i686-debug/obj/mprWide.o
	rm -f solaris-i686-debug/obj/mprWin.o
	rm -f solaris-i686-debug/obj/mprWince.o
	rm -f solaris-i686-debug/obj/mprXml.o
	rm -f solaris-i686-debug/obj/mprMatrixssl.o
	rm -f solaris-i686-debug/obj/mprOpenssl.o
	rm -f solaris-i686-debug/obj/mprSsl.o
	rm -f solaris-i686-debug/obj/manager.o
	rm -f solaris-i686-debug/obj/makerom.o
	rm -f solaris-i686-debug/obj/charGen.o

solaris-i686-debug/obj/dtoa.o: \
        src/dtoa.c \
        solaris-i686-debug/inc/bit.h
	$(CC) -c -o solaris-i686-debug/obj/dtoa.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/dtoa.c

solaris-i686-debug/obj/mpr.o: \
        src/mpr.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mpr.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mpr.c

solaris-i686-debug/obj/mprAsync.o: \
        src/mprAsync.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprAsync.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprAsync.c

solaris-i686-debug/obj/mprAtomic.o: \
        src/mprAtomic.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprAtomic.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprAtomic.c

solaris-i686-debug/obj/mprBuf.o: \
        src/mprBuf.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprBuf.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprBuf.c

solaris-i686-debug/obj/mprCache.o: \
        src/mprCache.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprCache.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprCache.c

solaris-i686-debug/obj/mprCmd.o: \
        src/mprCmd.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprCmd.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprCmd.c

solaris-i686-debug/obj/mprCond.o: \
        src/mprCond.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprCond.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprCond.c

solaris-i686-debug/obj/mprCrypt.o: \
        src/mprCrypt.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprCrypt.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprCrypt.c

solaris-i686-debug/obj/mprDisk.o: \
        src/mprDisk.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprDisk.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprDisk.c

solaris-i686-debug/obj/mprDispatcher.o: \
        src/mprDispatcher.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprDispatcher.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprDispatcher.c

solaris-i686-debug/obj/mprEncode.o: \
        src/mprEncode.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprEncode.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprEncode.c

solaris-i686-debug/obj/mprEpoll.o: \
        src/mprEpoll.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprEpoll.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprEpoll.c

solaris-i686-debug/obj/mprEvent.o: \
        src/mprEvent.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprEvent.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprEvent.c

solaris-i686-debug/obj/mprFile.o: \
        src/mprFile.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprFile.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprFile.c

solaris-i686-debug/obj/mprFileSystem.o: \
        src/mprFileSystem.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprFileSystem.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprFileSystem.c

solaris-i686-debug/obj/mprHash.o: \
        src/mprHash.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprHash.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprHash.c

solaris-i686-debug/obj/mprJSON.o: \
        src/mprJSON.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprJSON.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprJSON.c

solaris-i686-debug/obj/mprKqueue.o: \
        src/mprKqueue.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprKqueue.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprKqueue.c

solaris-i686-debug/obj/mprList.o: \
        src/mprList.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprList.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprList.c

solaris-i686-debug/obj/mprLock.o: \
        src/mprLock.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprLock.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprLock.c

solaris-i686-debug/obj/mprLog.o: \
        src/mprLog.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprLog.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprLog.c

solaris-i686-debug/obj/mprMem.o: \
        src/mprMem.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprMem.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprMem.c

solaris-i686-debug/obj/mprMime.o: \
        src/mprMime.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprMime.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprMime.c

solaris-i686-debug/obj/mprMixed.o: \
        src/mprMixed.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprMixed.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprMixed.c

solaris-i686-debug/obj/mprModule.o: \
        src/mprModule.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprModule.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprModule.c

solaris-i686-debug/obj/mprPath.o: \
        src/mprPath.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprPath.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprPath.c

solaris-i686-debug/obj/mprPoll.o: \
        src/mprPoll.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprPoll.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprPoll.c

solaris-i686-debug/obj/mprPrintf.o: \
        src/mprPrintf.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprPrintf.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprPrintf.c

solaris-i686-debug/obj/mprRomFile.o: \
        src/mprRomFile.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprRomFile.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprRomFile.c

solaris-i686-debug/obj/mprSelect.o: \
        src/mprSelect.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprSelect.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprSelect.c

solaris-i686-debug/obj/mprSignal.o: \
        src/mprSignal.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprSignal.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprSignal.c

solaris-i686-debug/obj/mprSocket.o: \
        src/mprSocket.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprSocket.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprSocket.c

solaris-i686-debug/obj/mprString.o: \
        src/mprString.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprString.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprString.c

solaris-i686-debug/obj/mprTest.o: \
        src/mprTest.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprTest.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprTest.c

solaris-i686-debug/obj/mprThread.o: \
        src/mprThread.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprThread.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprThread.c

solaris-i686-debug/obj/mprTime.o: \
        src/mprTime.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprTime.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprTime.c

solaris-i686-debug/obj/mprUnix.o: \
        src/mprUnix.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprUnix.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprUnix.c

solaris-i686-debug/obj/mprVxworks.o: \
        src/mprVxworks.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprVxworks.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprVxworks.c

solaris-i686-debug/obj/mprWait.o: \
        src/mprWait.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprWait.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprWait.c

solaris-i686-debug/obj/mprWide.o: \
        src/mprWide.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprWide.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprWide.c

solaris-i686-debug/obj/mprWin.o: \
        src/mprWin.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprWin.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprWin.c

solaris-i686-debug/obj/mprWince.o: \
        src/mprWince.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprWince.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprWince.c

solaris-i686-debug/obj/mprXml.o: \
        src/mprXml.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/mprXml.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/mprXml.c

solaris-i686-debug/lib/libmpr.so:  \
        solaris-i686-debug/obj/dtoa.o \
        solaris-i686-debug/obj/mpr.o \
        solaris-i686-debug/obj/mprAsync.o \
        solaris-i686-debug/obj/mprAtomic.o \
        solaris-i686-debug/obj/mprBuf.o \
        solaris-i686-debug/obj/mprCache.o \
        solaris-i686-debug/obj/mprCmd.o \
        solaris-i686-debug/obj/mprCond.o \
        solaris-i686-debug/obj/mprCrypt.o \
        solaris-i686-debug/obj/mprDisk.o \
        solaris-i686-debug/obj/mprDispatcher.o \
        solaris-i686-debug/obj/mprEncode.o \
        solaris-i686-debug/obj/mprEpoll.o \
        solaris-i686-debug/obj/mprEvent.o \
        solaris-i686-debug/obj/mprFile.o \
        solaris-i686-debug/obj/mprFileSystem.o \
        solaris-i686-debug/obj/mprHash.o \
        solaris-i686-debug/obj/mprJSON.o \
        solaris-i686-debug/obj/mprKqueue.o \
        solaris-i686-debug/obj/mprList.o \
        solaris-i686-debug/obj/mprLock.o \
        solaris-i686-debug/obj/mprLog.o \
        solaris-i686-debug/obj/mprMem.o \
        solaris-i686-debug/obj/mprMime.o \
        solaris-i686-debug/obj/mprMixed.o \
        solaris-i686-debug/obj/mprModule.o \
        solaris-i686-debug/obj/mprPath.o \
        solaris-i686-debug/obj/mprPoll.o \
        solaris-i686-debug/obj/mprPrintf.o \
        solaris-i686-debug/obj/mprRomFile.o \
        solaris-i686-debug/obj/mprSelect.o \
        solaris-i686-debug/obj/mprSignal.o \
        solaris-i686-debug/obj/mprSocket.o \
        solaris-i686-debug/obj/mprString.o \
        solaris-i686-debug/obj/mprTest.o \
        solaris-i686-debug/obj/mprThread.o \
        solaris-i686-debug/obj/mprTime.o \
        solaris-i686-debug/obj/mprUnix.o \
        solaris-i686-debug/obj/mprVxworks.o \
        solaris-i686-debug/obj/mprWait.o \
        solaris-i686-debug/obj/mprWide.o \
        solaris-i686-debug/obj/mprWin.o \
        solaris-i686-debug/obj/mprWince.o \
        solaris-i686-debug/obj/mprXml.o
	$(CC) -shared -o solaris-i686-debug/lib/libmpr.so $(LDFLAGS) solaris-i686-debug/obj/dtoa.o solaris-i686-debug/obj/mpr.o solaris-i686-debug/obj/mprAsync.o solaris-i686-debug/obj/mprAtomic.o solaris-i686-debug/obj/mprBuf.o solaris-i686-debug/obj/mprCache.o solaris-i686-debug/obj/mprCmd.o solaris-i686-debug/obj/mprCond.o solaris-i686-debug/obj/mprCrypt.o solaris-i686-debug/obj/mprDisk.o solaris-i686-debug/obj/mprDispatcher.o solaris-i686-debug/obj/mprEncode.o solaris-i686-debug/obj/mprEpoll.o solaris-i686-debug/obj/mprEvent.o solaris-i686-debug/obj/mprFile.o solaris-i686-debug/obj/mprFileSystem.o solaris-i686-debug/obj/mprHash.o solaris-i686-debug/obj/mprJSON.o solaris-i686-debug/obj/mprKqueue.o solaris-i686-debug/obj/mprList.o solaris-i686-debug/obj/mprLock.o solaris-i686-debug/obj/mprLog.o solaris-i686-debug/obj/mprMem.o solaris-i686-debug/obj/mprMime.o solaris-i686-debug/obj/mprMixed.o solaris-i686-debug/obj/mprModule.o solaris-i686-debug/obj/mprPath.o solaris-i686-debug/obj/mprPoll.o solaris-i686-debug/obj/mprPrintf.o solaris-i686-debug/obj/mprRomFile.o solaris-i686-debug/obj/mprSelect.o solaris-i686-debug/obj/mprSignal.o solaris-i686-debug/obj/mprSocket.o solaris-i686-debug/obj/mprString.o solaris-i686-debug/obj/mprTest.o solaris-i686-debug/obj/mprThread.o solaris-i686-debug/obj/mprTime.o solaris-i686-debug/obj/mprUnix.o solaris-i686-debug/obj/mprVxworks.o solaris-i686-debug/obj/mprWait.o solaris-i686-debug/obj/mprWide.o solaris-i686-debug/obj/mprWin.o solaris-i686-debug/obj/mprWince.o solaris-i686-debug/obj/mprXml.o $(LIBS)

solaris-i686-debug/obj/benchMpr.o: \
        test/benchMpr.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/benchMpr.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/benchMpr.c

solaris-i686-debug/bin/benchMpr:  \
        solaris-i686-debug/lib/libmpr.so \
        solaris-i686-debug/obj/benchMpr.o
	$(CC) -o solaris-i686-debug/bin/benchMpr $(LDFLAGS) -Lsolaris-i686-debug/lib solaris-i686-debug/obj/benchMpr.o $(LIBS) -lmpr -Lsolaris-i686-debug/lib -g

solaris-i686-debug/obj/runProgram.o: \
        test/runProgram.c \
        solaris-i686-debug/inc/bit.h \
        solaris-i686-debug/inc/buildConfig.h
	$(CC) -c -o solaris-i686-debug/obj/runProgram.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/runProgram.c

solaris-i686-debug/bin/runProgram:  \
        solaris-i686-debug/obj/runProgram.o
	$(CC) -o solaris-i686-debug/bin/runProgram $(LDFLAGS) -Lsolaris-i686-debug/lib solaris-i686-debug/obj/runProgram.o $(LIBS) -Lsolaris-i686-debug/lib -g

solaris-i686-debug/obj/testArgv.o: \
        test/testArgv.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testArgv.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testArgv.c

solaris-i686-debug/obj/testBuf.o: \
        test/testBuf.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testBuf.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testBuf.c

solaris-i686-debug/obj/testCmd.o: \
        test/testCmd.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testCmd.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testCmd.c

solaris-i686-debug/obj/testCond.o: \
        test/testCond.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testCond.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testCond.c

solaris-i686-debug/obj/testEvent.o: \
        test/testEvent.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testEvent.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testEvent.c

solaris-i686-debug/obj/testFile.o: \
        test/testFile.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testFile.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testFile.c

solaris-i686-debug/obj/testHash.o: \
        test/testHash.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testHash.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testHash.c

solaris-i686-debug/obj/testList.o: \
        test/testList.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testList.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testList.c

solaris-i686-debug/obj/testLock.o: \
        test/testLock.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testLock.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testLock.c

solaris-i686-debug/obj/testMem.o: \
        test/testMem.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testMem.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testMem.c

solaris-i686-debug/obj/testMpr.o: \
        test/testMpr.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testMpr.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testMpr.c

solaris-i686-debug/obj/testPath.o: \
        test/testPath.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testPath.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testPath.c

solaris-i686-debug/obj/testSocket.o: \
        test/testSocket.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testSocket.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testSocket.c

solaris-i686-debug/obj/testSprintf.o: \
        test/testSprintf.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testSprintf.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testSprintf.c

solaris-i686-debug/obj/testThread.o: \
        test/testThread.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testThread.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testThread.c

solaris-i686-debug/obj/testTime.o: \
        test/testTime.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testTime.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testTime.c

solaris-i686-debug/obj/testUnicode.o: \
        test/testUnicode.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/testUnicode.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc test/testUnicode.c

solaris-i686-debug/bin/testMpr:  \
        solaris-i686-debug/lib/libmpr.so \
        solaris-i686-debug/bin/runProgram \
        solaris-i686-debug/obj/testArgv.o \
        solaris-i686-debug/obj/testBuf.o \
        solaris-i686-debug/obj/testCmd.o \
        solaris-i686-debug/obj/testCond.o \
        solaris-i686-debug/obj/testEvent.o \
        solaris-i686-debug/obj/testFile.o \
        solaris-i686-debug/obj/testHash.o \
        solaris-i686-debug/obj/testList.o \
        solaris-i686-debug/obj/testLock.o \
        solaris-i686-debug/obj/testMem.o \
        solaris-i686-debug/obj/testMpr.o \
        solaris-i686-debug/obj/testPath.o \
        solaris-i686-debug/obj/testSocket.o \
        solaris-i686-debug/obj/testSprintf.o \
        solaris-i686-debug/obj/testThread.o \
        solaris-i686-debug/obj/testTime.o \
        solaris-i686-debug/obj/testUnicode.o
	$(CC) -o solaris-i686-debug/bin/testMpr $(LDFLAGS) -Lsolaris-i686-debug/lib solaris-i686-debug/obj/testArgv.o solaris-i686-debug/obj/testBuf.o solaris-i686-debug/obj/testCmd.o solaris-i686-debug/obj/testCond.o solaris-i686-debug/obj/testEvent.o solaris-i686-debug/obj/testFile.o solaris-i686-debug/obj/testHash.o solaris-i686-debug/obj/testList.o solaris-i686-debug/obj/testLock.o solaris-i686-debug/obj/testMem.o solaris-i686-debug/obj/testMpr.o solaris-i686-debug/obj/testPath.o solaris-i686-debug/obj/testSocket.o solaris-i686-debug/obj/testSprintf.o solaris-i686-debug/obj/testThread.o solaris-i686-debug/obj/testTime.o solaris-i686-debug/obj/testUnicode.o $(LIBS) -lmpr -Lsolaris-i686-debug/lib -g

solaris-i686-debug/obj/manager.o: \
        src/manager.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/manager.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/manager.c

solaris-i686-debug/bin/manager:  \
        solaris-i686-debug/lib/libmpr.so \
        solaris-i686-debug/obj/manager.o
	$(CC) -o solaris-i686-debug/bin/manager $(LDFLAGS) -Lsolaris-i686-debug/lib solaris-i686-debug/obj/manager.o $(LIBS) -lmpr -Lsolaris-i686-debug/lib -g

solaris-i686-debug/obj/makerom.o: \
        src/utils/makerom.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/makerom.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/utils/makerom.c

solaris-i686-debug/bin/makerom:  \
        solaris-i686-debug/lib/libmpr.so \
        solaris-i686-debug/obj/makerom.o
	$(CC) -o solaris-i686-debug/bin/makerom $(LDFLAGS) -Lsolaris-i686-debug/lib solaris-i686-debug/obj/makerom.o $(LIBS) -lmpr -Lsolaris-i686-debug/lib -g

solaris-i686-debug/obj/charGen.o: \
        src/utils/charGen.c \
        solaris-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o solaris-i686-debug/obj/charGen.o $(CFLAGS) $(DFLAGS) -Isrc -Isolaris-i686-debug/inc src/utils/charGen.c

solaris-i686-debug/bin/chargen:  \
        solaris-i686-debug/lib/libmpr.so \
        solaris-i686-debug/obj/charGen.o
	$(CC) -o solaris-i686-debug/bin/chargen $(LDFLAGS) -Lsolaris-i686-debug/lib solaris-i686-debug/obj/charGen.o $(LIBS) -lmpr -Lsolaris-i686-debug/lib -g

