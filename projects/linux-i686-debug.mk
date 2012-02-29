#
#   build.mk -- Build It Makefile to build Multithreaded Portable Runtime for linux on i686
#

CC        := cc
CFLAGS    := -fPIC -g -mcpu=i686
DFLAGS    := -DPIC
IFLAGS    := -Isrc -Ilinux-i686-debug/inc
LDFLAGS   := -L/Users/mob/git/mpr/linux-i686-debug/lib -g
LIBS      := -lpthread -lm

all: \
        linux-i686-debug/bin/benchMpr \
        linux-i686-debug/bin/runProgram \
        linux-i686-debug/bin/testMpr \
        linux-i686-debug/lib/libmpr.so \
        linux-i686-debug/bin/manager \
        linux-i686-debug/bin/makerom \
        linux-i686-debug/bin/chargen

clean:
	rm -f linux-i686-debug/bin/benchMpr
	rm -f linux-i686-debug/bin/runProgram
	rm -f linux-i686-debug/bin/testMpr
	rm -f linux-i686-debug/lib/libmpr.so
	rm -f linux-i686-debug/lib/libmprssl.so
	rm -f linux-i686-debug/bin/manager
	rm -f linux-i686-debug/bin/makerom
	rm -f linux-i686-debug/bin/chargen
	rm -f linux-i686-debug/obj/benchMpr.o
	rm -f linux-i686-debug/obj/runProgram.o
	rm -f linux-i686-debug/obj/testArgv.o
	rm -f linux-i686-debug/obj/testBuf.o
	rm -f linux-i686-debug/obj/testCmd.o
	rm -f linux-i686-debug/obj/testCond.o
	rm -f linux-i686-debug/obj/testEvent.o
	rm -f linux-i686-debug/obj/testFile.o
	rm -f linux-i686-debug/obj/testHash.o
	rm -f linux-i686-debug/obj/testList.o
	rm -f linux-i686-debug/obj/testLock.o
	rm -f linux-i686-debug/obj/testMem.o
	rm -f linux-i686-debug/obj/testMpr.o
	rm -f linux-i686-debug/obj/testPath.o
	rm -f linux-i686-debug/obj/testSocket.o
	rm -f linux-i686-debug/obj/testSprintf.o
	rm -f linux-i686-debug/obj/testThread.o
	rm -f linux-i686-debug/obj/testTime.o
	rm -f linux-i686-debug/obj/testUnicode.o
	rm -f linux-i686-debug/obj/dtoa.o
	rm -f linux-i686-debug/obj/mpr.o
	rm -f linux-i686-debug/obj/mprAsync.o
	rm -f linux-i686-debug/obj/mprAtomic.o
	rm -f linux-i686-debug/obj/mprBuf.o
	rm -f linux-i686-debug/obj/mprCache.o
	rm -f linux-i686-debug/obj/mprCmd.o
	rm -f linux-i686-debug/obj/mprCond.o
	rm -f linux-i686-debug/obj/mprCrypt.o
	rm -f linux-i686-debug/obj/mprDisk.o
	rm -f linux-i686-debug/obj/mprDispatcher.o
	rm -f linux-i686-debug/obj/mprEncode.o
	rm -f linux-i686-debug/obj/mprEpoll.o
	rm -f linux-i686-debug/obj/mprEvent.o
	rm -f linux-i686-debug/obj/mprFile.o
	rm -f linux-i686-debug/obj/mprFileSystem.o
	rm -f linux-i686-debug/obj/mprHash.o
	rm -f linux-i686-debug/obj/mprJSON.o
	rm -f linux-i686-debug/obj/mprKqueue.o
	rm -f linux-i686-debug/obj/mprList.o
	rm -f linux-i686-debug/obj/mprLock.o
	rm -f linux-i686-debug/obj/mprLog.o
	rm -f linux-i686-debug/obj/mprMem.o
	rm -f linux-i686-debug/obj/mprMime.o
	rm -f linux-i686-debug/obj/mprMixed.o
	rm -f linux-i686-debug/obj/mprModule.o
	rm -f linux-i686-debug/obj/mprPath.o
	rm -f linux-i686-debug/obj/mprPoll.o
	rm -f linux-i686-debug/obj/mprPrintf.o
	rm -f linux-i686-debug/obj/mprRomFile.o
	rm -f linux-i686-debug/obj/mprSelect.o
	rm -f linux-i686-debug/obj/mprSignal.o
	rm -f linux-i686-debug/obj/mprSocket.o
	rm -f linux-i686-debug/obj/mprString.o
	rm -f linux-i686-debug/obj/mprTest.o
	rm -f linux-i686-debug/obj/mprThread.o
	rm -f linux-i686-debug/obj/mprTime.o
	rm -f linux-i686-debug/obj/mprUnix.o
	rm -f linux-i686-debug/obj/mprVxworks.o
	rm -f linux-i686-debug/obj/mprWait.o
	rm -f linux-i686-debug/obj/mprWide.o
	rm -f linux-i686-debug/obj/mprWin.o
	rm -f linux-i686-debug/obj/mprWince.o
	rm -f linux-i686-debug/obj/mprXml.o
	rm -f linux-i686-debug/obj/mprMatrixssl.o
	rm -f linux-i686-debug/obj/mprOpenssl.o
	rm -f linux-i686-debug/obj/mprSsl.o
	rm -f linux-i686-debug/obj/manager.o
	rm -f linux-i686-debug/obj/makerom.o
	rm -f linux-i686-debug/obj/charGen.o

linux-i686-debug/obj/dtoa.o: \
        src/dtoa.c \
        linux-i686-debug/inc/bit.h
	$(CC) -c -o linux-i686-debug/obj/dtoa.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/dtoa.c

linux-i686-debug/obj/mpr.o: \
        src/mpr.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mpr.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mpr.c

linux-i686-debug/obj/mprAsync.o: \
        src/mprAsync.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprAsync.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprAsync.c

linux-i686-debug/obj/mprAtomic.o: \
        src/mprAtomic.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprAtomic.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprAtomic.c

linux-i686-debug/obj/mprBuf.o: \
        src/mprBuf.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprBuf.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprBuf.c

linux-i686-debug/obj/mprCache.o: \
        src/mprCache.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprCache.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprCache.c

linux-i686-debug/obj/mprCmd.o: \
        src/mprCmd.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprCmd.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprCmd.c

linux-i686-debug/obj/mprCond.o: \
        src/mprCond.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprCond.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprCond.c

linux-i686-debug/obj/mprCrypt.o: \
        src/mprCrypt.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprCrypt.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprCrypt.c

linux-i686-debug/obj/mprDisk.o: \
        src/mprDisk.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprDisk.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprDisk.c

linux-i686-debug/obj/mprDispatcher.o: \
        src/mprDispatcher.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprDispatcher.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprDispatcher.c

linux-i686-debug/obj/mprEncode.o: \
        src/mprEncode.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprEncode.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprEncode.c

linux-i686-debug/obj/mprEpoll.o: \
        src/mprEpoll.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprEpoll.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprEpoll.c

linux-i686-debug/obj/mprEvent.o: \
        src/mprEvent.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprEvent.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprEvent.c

linux-i686-debug/obj/mprFile.o: \
        src/mprFile.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprFile.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprFile.c

linux-i686-debug/obj/mprFileSystem.o: \
        src/mprFileSystem.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprFileSystem.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprFileSystem.c

linux-i686-debug/obj/mprHash.o: \
        src/mprHash.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprHash.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprHash.c

linux-i686-debug/obj/mprJSON.o: \
        src/mprJSON.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprJSON.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprJSON.c

linux-i686-debug/obj/mprKqueue.o: \
        src/mprKqueue.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprKqueue.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprKqueue.c

linux-i686-debug/obj/mprList.o: \
        src/mprList.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprList.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprList.c

linux-i686-debug/obj/mprLock.o: \
        src/mprLock.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprLock.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprLock.c

linux-i686-debug/obj/mprLog.o: \
        src/mprLog.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprLog.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprLog.c

linux-i686-debug/obj/mprMem.o: \
        src/mprMem.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprMem.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprMem.c

linux-i686-debug/obj/mprMime.o: \
        src/mprMime.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprMime.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprMime.c

linux-i686-debug/obj/mprMixed.o: \
        src/mprMixed.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprMixed.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprMixed.c

linux-i686-debug/obj/mprModule.o: \
        src/mprModule.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprModule.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprModule.c

linux-i686-debug/obj/mprPath.o: \
        src/mprPath.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprPath.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprPath.c

linux-i686-debug/obj/mprPoll.o: \
        src/mprPoll.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprPoll.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprPoll.c

linux-i686-debug/obj/mprPrintf.o: \
        src/mprPrintf.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprPrintf.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprPrintf.c

linux-i686-debug/obj/mprRomFile.o: \
        src/mprRomFile.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprRomFile.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprRomFile.c

linux-i686-debug/obj/mprSelect.o: \
        src/mprSelect.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprSelect.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprSelect.c

linux-i686-debug/obj/mprSignal.o: \
        src/mprSignal.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprSignal.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprSignal.c

linux-i686-debug/obj/mprSocket.o: \
        src/mprSocket.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprSocket.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprSocket.c

linux-i686-debug/obj/mprString.o: \
        src/mprString.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprString.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprString.c

linux-i686-debug/obj/mprTest.o: \
        src/mprTest.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprTest.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprTest.c

linux-i686-debug/obj/mprThread.o: \
        src/mprThread.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprThread.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprThread.c

linux-i686-debug/obj/mprTime.o: \
        src/mprTime.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprTime.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprTime.c

linux-i686-debug/obj/mprUnix.o: \
        src/mprUnix.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprUnix.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprUnix.c

linux-i686-debug/obj/mprVxworks.o: \
        src/mprVxworks.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprVxworks.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprVxworks.c

linux-i686-debug/obj/mprWait.o: \
        src/mprWait.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprWait.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprWait.c

linux-i686-debug/obj/mprWide.o: \
        src/mprWide.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprWide.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprWide.c

linux-i686-debug/obj/mprWin.o: \
        src/mprWin.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprWin.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprWin.c

linux-i686-debug/obj/mprWince.o: \
        src/mprWince.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprWince.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprWince.c

linux-i686-debug/obj/mprXml.o: \
        src/mprXml.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/mprXml.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/mprXml.c

linux-i686-debug/lib/libmpr.so:  \
        linux-i686-debug/obj/dtoa.o \
        linux-i686-debug/obj/mpr.o \
        linux-i686-debug/obj/mprAsync.o \
        linux-i686-debug/obj/mprAtomic.o \
        linux-i686-debug/obj/mprBuf.o \
        linux-i686-debug/obj/mprCache.o \
        linux-i686-debug/obj/mprCmd.o \
        linux-i686-debug/obj/mprCond.o \
        linux-i686-debug/obj/mprCrypt.o \
        linux-i686-debug/obj/mprDisk.o \
        linux-i686-debug/obj/mprDispatcher.o \
        linux-i686-debug/obj/mprEncode.o \
        linux-i686-debug/obj/mprEpoll.o \
        linux-i686-debug/obj/mprEvent.o \
        linux-i686-debug/obj/mprFile.o \
        linux-i686-debug/obj/mprFileSystem.o \
        linux-i686-debug/obj/mprHash.o \
        linux-i686-debug/obj/mprJSON.o \
        linux-i686-debug/obj/mprKqueue.o \
        linux-i686-debug/obj/mprList.o \
        linux-i686-debug/obj/mprLock.o \
        linux-i686-debug/obj/mprLog.o \
        linux-i686-debug/obj/mprMem.o \
        linux-i686-debug/obj/mprMime.o \
        linux-i686-debug/obj/mprMixed.o \
        linux-i686-debug/obj/mprModule.o \
        linux-i686-debug/obj/mprPath.o \
        linux-i686-debug/obj/mprPoll.o \
        linux-i686-debug/obj/mprPrintf.o \
        linux-i686-debug/obj/mprRomFile.o \
        linux-i686-debug/obj/mprSelect.o \
        linux-i686-debug/obj/mprSignal.o \
        linux-i686-debug/obj/mprSocket.o \
        linux-i686-debug/obj/mprString.o \
        linux-i686-debug/obj/mprTest.o \
        linux-i686-debug/obj/mprThread.o \
        linux-i686-debug/obj/mprTime.o \
        linux-i686-debug/obj/mprUnix.o \
        linux-i686-debug/obj/mprVxworks.o \
        linux-i686-debug/obj/mprWait.o \
        linux-i686-debug/obj/mprWide.o \
        linux-i686-debug/obj/mprWin.o \
        linux-i686-debug/obj/mprWince.o \
        linux-i686-debug/obj/mprXml.o
	$(CC) -shared -o linux-i686-debug/lib/libmpr.so $(LDFLAGS) linux-i686-debug/obj/dtoa.o linux-i686-debug/obj/mpr.o linux-i686-debug/obj/mprAsync.o linux-i686-debug/obj/mprAtomic.o linux-i686-debug/obj/mprBuf.o linux-i686-debug/obj/mprCache.o linux-i686-debug/obj/mprCmd.o linux-i686-debug/obj/mprCond.o linux-i686-debug/obj/mprCrypt.o linux-i686-debug/obj/mprDisk.o linux-i686-debug/obj/mprDispatcher.o linux-i686-debug/obj/mprEncode.o linux-i686-debug/obj/mprEpoll.o linux-i686-debug/obj/mprEvent.o linux-i686-debug/obj/mprFile.o linux-i686-debug/obj/mprFileSystem.o linux-i686-debug/obj/mprHash.o linux-i686-debug/obj/mprJSON.o linux-i686-debug/obj/mprKqueue.o linux-i686-debug/obj/mprList.o linux-i686-debug/obj/mprLock.o linux-i686-debug/obj/mprLog.o linux-i686-debug/obj/mprMem.o linux-i686-debug/obj/mprMime.o linux-i686-debug/obj/mprMixed.o linux-i686-debug/obj/mprModule.o linux-i686-debug/obj/mprPath.o linux-i686-debug/obj/mprPoll.o linux-i686-debug/obj/mprPrintf.o linux-i686-debug/obj/mprRomFile.o linux-i686-debug/obj/mprSelect.o linux-i686-debug/obj/mprSignal.o linux-i686-debug/obj/mprSocket.o linux-i686-debug/obj/mprString.o linux-i686-debug/obj/mprTest.o linux-i686-debug/obj/mprThread.o linux-i686-debug/obj/mprTime.o linux-i686-debug/obj/mprUnix.o linux-i686-debug/obj/mprVxworks.o linux-i686-debug/obj/mprWait.o linux-i686-debug/obj/mprWide.o linux-i686-debug/obj/mprWin.o linux-i686-debug/obj/mprWince.o linux-i686-debug/obj/mprXml.o $(LIBS)

linux-i686-debug/obj/benchMpr.o: \
        test/benchMpr.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/benchMpr.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/benchMpr.c

linux-i686-debug/bin/benchMpr:  \
        linux-i686-debug/lib/libmpr.so \
        linux-i686-debug/obj/benchMpr.o
	$(CC) -o linux-i686-debug/bin/benchMpr $(LDFLAGS) -Llinux-i686-debug/lib linux-i686-debug/obj/benchMpr.o $(LIBS) -lmpr -Llinux-i686-debug/lib -g

linux-i686-debug/obj/runProgram.o: \
        test/runProgram.c \
        linux-i686-debug/inc/bit.h \
        linux-i686-debug/inc/buildConfig.h
	$(CC) -c -o linux-i686-debug/obj/runProgram.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/runProgram.c

linux-i686-debug/bin/runProgram:  \
        linux-i686-debug/obj/runProgram.o
	$(CC) -o linux-i686-debug/bin/runProgram $(LDFLAGS) -Llinux-i686-debug/lib linux-i686-debug/obj/runProgram.o $(LIBS) -Llinux-i686-debug/lib -g

linux-i686-debug/obj/testArgv.o: \
        test/testArgv.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testArgv.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testArgv.c

linux-i686-debug/obj/testBuf.o: \
        test/testBuf.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testBuf.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testBuf.c

linux-i686-debug/obj/testCmd.o: \
        test/testCmd.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testCmd.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testCmd.c

linux-i686-debug/obj/testCond.o: \
        test/testCond.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testCond.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testCond.c

linux-i686-debug/obj/testEvent.o: \
        test/testEvent.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testEvent.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testEvent.c

linux-i686-debug/obj/testFile.o: \
        test/testFile.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testFile.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testFile.c

linux-i686-debug/obj/testHash.o: \
        test/testHash.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testHash.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testHash.c

linux-i686-debug/obj/testList.o: \
        test/testList.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testList.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testList.c

linux-i686-debug/obj/testLock.o: \
        test/testLock.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testLock.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testLock.c

linux-i686-debug/obj/testMem.o: \
        test/testMem.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testMem.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testMem.c

linux-i686-debug/obj/testMpr.o: \
        test/testMpr.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testMpr.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testMpr.c

linux-i686-debug/obj/testPath.o: \
        test/testPath.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testPath.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testPath.c

linux-i686-debug/obj/testSocket.o: \
        test/testSocket.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testSocket.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testSocket.c

linux-i686-debug/obj/testSprintf.o: \
        test/testSprintf.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testSprintf.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testSprintf.c

linux-i686-debug/obj/testThread.o: \
        test/testThread.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testThread.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testThread.c

linux-i686-debug/obj/testTime.o: \
        test/testTime.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testTime.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testTime.c

linux-i686-debug/obj/testUnicode.o: \
        test/testUnicode.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/testUnicode.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc test/testUnicode.c

linux-i686-debug/bin/testMpr:  \
        linux-i686-debug/lib/libmpr.so \
        linux-i686-debug/bin/runProgram \
        linux-i686-debug/obj/testArgv.o \
        linux-i686-debug/obj/testBuf.o \
        linux-i686-debug/obj/testCmd.o \
        linux-i686-debug/obj/testCond.o \
        linux-i686-debug/obj/testEvent.o \
        linux-i686-debug/obj/testFile.o \
        linux-i686-debug/obj/testHash.o \
        linux-i686-debug/obj/testList.o \
        linux-i686-debug/obj/testLock.o \
        linux-i686-debug/obj/testMem.o \
        linux-i686-debug/obj/testMpr.o \
        linux-i686-debug/obj/testPath.o \
        linux-i686-debug/obj/testSocket.o \
        linux-i686-debug/obj/testSprintf.o \
        linux-i686-debug/obj/testThread.o \
        linux-i686-debug/obj/testTime.o \
        linux-i686-debug/obj/testUnicode.o
	$(CC) -o linux-i686-debug/bin/testMpr $(LDFLAGS) -Llinux-i686-debug/lib linux-i686-debug/obj/testArgv.o linux-i686-debug/obj/testBuf.o linux-i686-debug/obj/testCmd.o linux-i686-debug/obj/testCond.o linux-i686-debug/obj/testEvent.o linux-i686-debug/obj/testFile.o linux-i686-debug/obj/testHash.o linux-i686-debug/obj/testList.o linux-i686-debug/obj/testLock.o linux-i686-debug/obj/testMem.o linux-i686-debug/obj/testMpr.o linux-i686-debug/obj/testPath.o linux-i686-debug/obj/testSocket.o linux-i686-debug/obj/testSprintf.o linux-i686-debug/obj/testThread.o linux-i686-debug/obj/testTime.o linux-i686-debug/obj/testUnicode.o $(LIBS) -lmpr -Llinux-i686-debug/lib -g

linux-i686-debug/obj/manager.o: \
        src/manager.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/manager.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/manager.c

linux-i686-debug/bin/manager:  \
        linux-i686-debug/lib/libmpr.so \
        linux-i686-debug/obj/manager.o
	$(CC) -o linux-i686-debug/bin/manager $(LDFLAGS) -Llinux-i686-debug/lib linux-i686-debug/obj/manager.o $(LIBS) -lmpr -Llinux-i686-debug/lib -g

linux-i686-debug/obj/makerom.o: \
        src/utils/makerom.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/makerom.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/utils/makerom.c

linux-i686-debug/bin/makerom:  \
        linux-i686-debug/lib/libmpr.so \
        linux-i686-debug/obj/makerom.o
	$(CC) -o linux-i686-debug/bin/makerom $(LDFLAGS) -Llinux-i686-debug/lib linux-i686-debug/obj/makerom.o $(LIBS) -lmpr -Llinux-i686-debug/lib -g

linux-i686-debug/obj/charGen.o: \
        src/utils/charGen.c \
        linux-i686-debug/inc/bit.h \
        src/mpr.h
	$(CC) -c -o linux-i686-debug/obj/charGen.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug/inc src/utils/charGen.c

linux-i686-debug/bin/chargen:  \
        linux-i686-debug/lib/libmpr.so \
        linux-i686-debug/obj/charGen.o
	$(CC) -o linux-i686-debug/bin/chargen $(LDFLAGS) -Llinux-i686-debug/lib linux-i686-debug/obj/charGen.o $(LIBS) -lmpr -Llinux-i686-debug/lib -g

