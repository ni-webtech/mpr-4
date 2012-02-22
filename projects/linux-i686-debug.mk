#
#   build.mk -- Build It Makefile to build Multithreaded Portable Runtime for linux on i686
#

CC        := cc
CFLAGS    := -fPIC -g -mcpu=i686
DFLAGS    := -DPIC
IFLAGS    := -Isrc -Ilinux-i686-debug\inc
LDFLAGS   := -LC:\cygwin\home\mob\mpr\linux-i686-debug\lib -g
LIBS      := -lpthread -lm

all: \
        linux-i686-debug\bin\benchMpr \
        linux-i686-debug\bin\runProgram \
        linux-i686-debug\bin\testMpr \
        linux-i686-debug\lib\libmpr.so \
        linux-i686-debug\bin\manager \
        linux-i686-debug\bin\makerom \
        linux-i686-debug\bin\chargen

clean:
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\bin\benchMpr
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\bin\runProgram
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\bin\testMpr
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\lib\libmpr.so
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\lib\libmprssl.so
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\bin\manager
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\bin\makerom
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\bin\chargen
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\benchMpr.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\runProgram.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testArgv.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testBuf.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testCmd.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testCond.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testEvent.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testFile.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testHash.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testList.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testLock.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testMem.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testMpr.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testPath.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testSocket.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testSprintf.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testThread.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testTime.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testUnicode.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\dtoa.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mpr.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprAsync.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprAtomic.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprBuf.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprCache.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprCmd.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprCond.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprCrypt.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprDisk.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprDispatcher.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprEncode.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprEpoll.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprEvent.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprFile.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprFileSystem.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprHash.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprJSON.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprKqueue.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprList.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprLock.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprLog.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprMem.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprMime.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprMixed.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprModule.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprPath.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprPoll.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprPrintf.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprRomFile.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprSelect.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprSignal.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprSocket.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprString.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprTest.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprThread.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprTime.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprUnix.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprVxworks.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprWait.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprWide.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprWin.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprWince.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprXml.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprMatrixssl.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprOpenssl.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprSsl.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\manager.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\makerom.o
	rm -f C:\cygwin\home\mob\mpr\linux-i686-debug\obj\charGen.o

linux-i686-debug\obj\dtoa.o: \
        src\dtoa.c \
        linux-i686-debug\inc\bit.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\dtoa.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\dtoa.c

linux-i686-debug\obj\mpr.o: \
        src\mpr.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mpr.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mpr.c

linux-i686-debug\obj\mprAsync.o: \
        src\mprAsync.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprAsync.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprAsync.c

linux-i686-debug\obj\mprAtomic.o: \
        src\mprAtomic.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprAtomic.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprAtomic.c

linux-i686-debug\obj\mprBuf.o: \
        src\mprBuf.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprBuf.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprBuf.c

linux-i686-debug\obj\mprCache.o: \
        src\mprCache.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprCache.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCache.c

linux-i686-debug\obj\mprCmd.o: \
        src\mprCmd.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprCmd.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCmd.c

linux-i686-debug\obj\mprCond.o: \
        src\mprCond.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprCond.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCond.c

linux-i686-debug\obj\mprCrypt.o: \
        src\mprCrypt.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprCrypt.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCrypt.c

linux-i686-debug\obj\mprDisk.o: \
        src\mprDisk.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprDisk.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprDisk.c

linux-i686-debug\obj\mprDispatcher.o: \
        src\mprDispatcher.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprDispatcher.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprDispatcher.c

linux-i686-debug\obj\mprEncode.o: \
        src\mprEncode.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprEncode.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprEncode.c

linux-i686-debug\obj\mprEpoll.o: \
        src\mprEpoll.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprEpoll.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprEpoll.c

linux-i686-debug\obj\mprEvent.o: \
        src\mprEvent.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprEvent.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprEvent.c

linux-i686-debug\obj\mprFile.o: \
        src\mprFile.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprFile.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprFile.c

linux-i686-debug\obj\mprFileSystem.o: \
        src\mprFileSystem.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprFileSystem.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprFileSystem.c

linux-i686-debug\obj\mprHash.o: \
        src\mprHash.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprHash.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprHash.c

linux-i686-debug\obj\mprJSON.o: \
        src\mprJSON.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprJSON.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprJSON.c

linux-i686-debug\obj\mprKqueue.o: \
        src\mprKqueue.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprKqueue.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprKqueue.c

linux-i686-debug\obj\mprList.o: \
        src\mprList.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprList.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprList.c

linux-i686-debug\obj\mprLock.o: \
        src\mprLock.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprLock.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprLock.c

linux-i686-debug\obj\mprLog.o: \
        src\mprLog.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprLog.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprLog.c

linux-i686-debug\obj\mprMem.o: \
        src\mprMem.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprMem.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprMem.c

linux-i686-debug\obj\mprMime.o: \
        src\mprMime.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprMime.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprMime.c

linux-i686-debug\obj\mprMixed.o: \
        src\mprMixed.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprMixed.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprMixed.c

linux-i686-debug\obj\mprModule.o: \
        src\mprModule.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprModule.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprModule.c

linux-i686-debug\obj\mprPath.o: \
        src\mprPath.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprPath.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprPath.c

linux-i686-debug\obj\mprPoll.o: \
        src\mprPoll.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprPoll.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprPoll.c

linux-i686-debug\obj\mprPrintf.o: \
        src\mprPrintf.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprPrintf.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprPrintf.c

linux-i686-debug\obj\mprRomFile.o: \
        src\mprRomFile.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprRomFile.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprRomFile.c

linux-i686-debug\obj\mprSelect.o: \
        src\mprSelect.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprSelect.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprSelect.c

linux-i686-debug\obj\mprSignal.o: \
        src\mprSignal.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprSignal.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprSignal.c

linux-i686-debug\obj\mprSocket.o: \
        src\mprSocket.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprSocket.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprSocket.c

linux-i686-debug\obj\mprString.o: \
        src\mprString.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprString.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprString.c

linux-i686-debug\obj\mprTest.o: \
        src\mprTest.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprTest.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprTest.c

linux-i686-debug\obj\mprThread.o: \
        src\mprThread.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprThread.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprThread.c

linux-i686-debug\obj\mprTime.o: \
        src\mprTime.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprTime.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprTime.c

linux-i686-debug\obj\mprUnix.o: \
        src\mprUnix.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprUnix.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprUnix.c

linux-i686-debug\obj\mprVxworks.o: \
        src\mprVxworks.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprVxworks.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprVxworks.c

linux-i686-debug\obj\mprWait.o: \
        src\mprWait.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprWait.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWait.c

linux-i686-debug\obj\mprWide.o: \
        src\mprWide.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprWide.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWide.c

linux-i686-debug\obj\mprWin.o: \
        src\mprWin.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprWin.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWin.c

linux-i686-debug\obj\mprWince.o: \
        src\mprWince.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprWince.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWince.c

linux-i686-debug\obj\mprXml.o: \
        src\mprXml.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprXml.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprXml.c

linux-i686-debug\lib\libmpr.so:  \
        linux-i686-debug\obj\dtoa.o \
        linux-i686-debug\obj\mpr.o \
        linux-i686-debug\obj\mprAsync.o \
        linux-i686-debug\obj\mprAtomic.o \
        linux-i686-debug\obj\mprBuf.o \
        linux-i686-debug\obj\mprCache.o \
        linux-i686-debug\obj\mprCmd.o \
        linux-i686-debug\obj\mprCond.o \
        linux-i686-debug\obj\mprCrypt.o \
        linux-i686-debug\obj\mprDisk.o \
        linux-i686-debug\obj\mprDispatcher.o \
        linux-i686-debug\obj\mprEncode.o \
        linux-i686-debug\obj\mprEpoll.o \
        linux-i686-debug\obj\mprEvent.o \
        linux-i686-debug\obj\mprFile.o \
        linux-i686-debug\obj\mprFileSystem.o \
        linux-i686-debug\obj\mprHash.o \
        linux-i686-debug\obj\mprJSON.o \
        linux-i686-debug\obj\mprKqueue.o \
        linux-i686-debug\obj\mprList.o \
        linux-i686-debug\obj\mprLock.o \
        linux-i686-debug\obj\mprLog.o \
        linux-i686-debug\obj\mprMem.o \
        linux-i686-debug\obj\mprMime.o \
        linux-i686-debug\obj\mprMixed.o \
        linux-i686-debug\obj\mprModule.o \
        linux-i686-debug\obj\mprPath.o \
        linux-i686-debug\obj\mprPoll.o \
        linux-i686-debug\obj\mprPrintf.o \
        linux-i686-debug\obj\mprRomFile.o \
        linux-i686-debug\obj\mprSelect.o \
        linux-i686-debug\obj\mprSignal.o \
        linux-i686-debug\obj\mprSocket.o \
        linux-i686-debug\obj\mprString.o \
        linux-i686-debug\obj\mprTest.o \
        linux-i686-debug\obj\mprThread.o \
        linux-i686-debug\obj\mprTime.o \
        linux-i686-debug\obj\mprUnix.o \
        linux-i686-debug\obj\mprVxworks.o \
        linux-i686-debug\obj\mprWait.o \
        linux-i686-debug\obj\mprWide.o \
        linux-i686-debug\obj\mprWin.o \
        linux-i686-debug\obj\mprWince.o \
        linux-i686-debug\obj\mprXml.o
	$(CC) -shared -o C:\cygwin\home\mob\mpr\linux-i686-debug\lib\libmpr.so $(LDFLAGS) C:\cygwin\home\mob\mpr\linux-i686-debug\obj\dtoa.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mpr.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprAsync.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprAtomic.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprBuf.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprCache.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprCmd.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprCond.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprCrypt.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprDisk.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprDispatcher.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprEncode.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprEpoll.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprEvent.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprFile.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprFileSystem.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprHash.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprJSON.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprKqueue.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprList.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprLock.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprLog.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprMem.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprMime.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprMixed.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprModule.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprPath.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprPoll.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprPrintf.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprRomFile.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprSelect.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprSignal.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprSocket.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprString.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprTest.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprThread.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprTime.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprUnix.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprVxworks.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprWait.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprWide.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprWin.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprWince.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\mprXml.o $(LIBS)

linux-i686-debug\obj\benchMpr.o: \
        test\benchMpr.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\benchMpr.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\benchMpr.c

linux-i686-debug\bin\benchMpr:  \
        linux-i686-debug\lib\libmpr.so \
        linux-i686-debug\obj\benchMpr.o
	$(CC) -o C:\cygwin\home\mob\mpr\linux-i686-debug\bin\benchMpr $(LDFLAGS) -Llinux-i686-debug/lib C:\cygwin\home\mob\mpr\linux-i686-debug\obj\benchMpr.o $(LIBS) -lmpr -LC:\cygwin\home\mob\mpr\linux-i686-debug\lib -g

linux-i686-debug\obj\runProgram.o: \
        test\runProgram.c \
        linux-i686-debug\inc\bit.h \
        linux-i686-debug\inc\buildConfig.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\runProgram.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\runProgram.c

linux-i686-debug\bin\runProgram:  \
        linux-i686-debug\obj\runProgram.o
	$(CC) -o C:\cygwin\home\mob\mpr\linux-i686-debug\bin\runProgram $(LDFLAGS) -Llinux-i686-debug/lib C:\cygwin\home\mob\mpr\linux-i686-debug\obj\runProgram.o $(LIBS) -LC:\cygwin\home\mob\mpr\linux-i686-debug\lib -g

linux-i686-debug\obj\testArgv.o: \
        test\testArgv.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testArgv.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testArgv.c

linux-i686-debug\obj\testBuf.o: \
        test\testBuf.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testBuf.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testBuf.c

linux-i686-debug\obj\testCmd.o: \
        test\testCmd.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testCmd.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testCmd.c

linux-i686-debug\obj\testCond.o: \
        test\testCond.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testCond.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testCond.c

linux-i686-debug\obj\testEvent.o: \
        test\testEvent.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testEvent.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testEvent.c

linux-i686-debug\obj\testFile.o: \
        test\testFile.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testFile.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testFile.c

linux-i686-debug\obj\testHash.o: \
        test\testHash.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testHash.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testHash.c

linux-i686-debug\obj\testList.o: \
        test\testList.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testList.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testList.c

linux-i686-debug\obj\testLock.o: \
        test\testLock.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testLock.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testLock.c

linux-i686-debug\obj\testMem.o: \
        test\testMem.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testMem.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testMem.c

linux-i686-debug\obj\testMpr.o: \
        test\testMpr.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testMpr.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testMpr.c

linux-i686-debug\obj\testPath.o: \
        test\testPath.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testPath.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testPath.c

linux-i686-debug\obj\testSocket.o: \
        test\testSocket.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testSocket.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testSocket.c

linux-i686-debug\obj\testSprintf.o: \
        test\testSprintf.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testSprintf.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testSprintf.c

linux-i686-debug\obj\testThread.o: \
        test\testThread.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testThread.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testThread.c

linux-i686-debug\obj\testTime.o: \
        test\testTime.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testTime.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testTime.c

linux-i686-debug\obj\testUnicode.o: \
        test\testUnicode.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testUnicode.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\test\testUnicode.c

linux-i686-debug\bin\testMpr:  \
        linux-i686-debug\lib\libmpr.so \
        linux-i686-debug\bin\runProgram \
        linux-i686-debug\obj\testArgv.o \
        linux-i686-debug\obj\testBuf.o \
        linux-i686-debug\obj\testCmd.o \
        linux-i686-debug\obj\testCond.o \
        linux-i686-debug\obj\testEvent.o \
        linux-i686-debug\obj\testFile.o \
        linux-i686-debug\obj\testHash.o \
        linux-i686-debug\obj\testList.o \
        linux-i686-debug\obj\testLock.o \
        linux-i686-debug\obj\testMem.o \
        linux-i686-debug\obj\testMpr.o \
        linux-i686-debug\obj\testPath.o \
        linux-i686-debug\obj\testSocket.o \
        linux-i686-debug\obj\testSprintf.o \
        linux-i686-debug\obj\testThread.o \
        linux-i686-debug\obj\testTime.o \
        linux-i686-debug\obj\testUnicode.o
	$(CC) -o C:\cygwin\home\mob\mpr\linux-i686-debug\bin\testMpr $(LDFLAGS) -Llinux-i686-debug/lib C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testArgv.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testBuf.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testCmd.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testCond.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testEvent.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testFile.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testHash.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testList.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testLock.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testMem.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testMpr.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testPath.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testSocket.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testSprintf.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testThread.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testTime.o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\testUnicode.o $(LIBS) -lmpr -LC:\cygwin\home\mob\mpr\linux-i686-debug\lib -g

linux-i686-debug\obj\manager.o: \
        src\manager.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\manager.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\manager.c

linux-i686-debug\bin\manager:  \
        linux-i686-debug\lib\libmpr.so \
        linux-i686-debug\obj\manager.o
	$(CC) -o C:\cygwin\home\mob\mpr\linux-i686-debug\bin\manager $(LDFLAGS) -Llinux-i686-debug/lib C:\cygwin\home\mob\mpr\linux-i686-debug\obj\manager.o $(LIBS) -lmpr -LC:\cygwin\home\mob\mpr\linux-i686-debug\lib -g

linux-i686-debug\obj\makerom.o: \
        src\utils\makerom.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\makerom.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\utils\makerom.c

linux-i686-debug\bin\makerom:  \
        linux-i686-debug\lib\libmpr.so \
        linux-i686-debug\obj\makerom.o
	$(CC) -o C:\cygwin\home\mob\mpr\linux-i686-debug\bin\makerom $(LDFLAGS) -Llinux-i686-debug/lib C:\cygwin\home\mob\mpr\linux-i686-debug\obj\makerom.o $(LIBS) -lmpr -LC:\cygwin\home\mob\mpr\linux-i686-debug\lib -g

linux-i686-debug\obj\charGen.o: \
        src\utils\charGen.c \
        linux-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\linux-i686-debug\obj\charGen.o $(CFLAGS) $(DFLAGS) -Isrc -Ilinux-i686-debug\inc C:\cygwin\home\mob\mpr\src\utils\charGen.c

linux-i686-debug\bin\chargen:  \
        linux-i686-debug\lib\libmpr.so \
        linux-i686-debug\obj\charGen.o
	$(CC) -o C:\cygwin\home\mob\mpr\linux-i686-debug\bin\chargen $(LDFLAGS) -Llinux-i686-debug/lib C:\cygwin\home\mob\mpr\linux-i686-debug\obj\charGen.o $(LIBS) -lmpr -LC:\cygwin\home\mob\mpr\linux-i686-debug\lib -g

