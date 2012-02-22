#
#   build.mk -- Build It Makefile to build Multithreaded Portable Runtime for macosx on i686
#

CC        := cc
CFLAGS    := -fPIC -Wall -g
DFLAGS    := -DPIC -DCPU=I686
IFLAGS    := -Isrc -Imacosx-i686-debug\inc
LDFLAGS   := -Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -LC:\cygwin\home\mob\mpr\macosx-i686-debug\lib -g
LIBS      := -lpthread -lm

all: \
        macosx-i686-debug\bin\benchMpr \
        macosx-i686-debug\bin\runProgram \
        macosx-i686-debug\bin\testMpr \
        macosx-i686-debug\lib\libmpr.dylib \
        macosx-i686-debug\bin\manager \
        macosx-i686-debug\bin\makerom \
        macosx-i686-debug\bin\chargen

clean:
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\benchMpr
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\runProgram
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\testMpr
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\lib\libmpr.dylib
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\lib\libmprssl.dylib
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\manager
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\makerom
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\chargen
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\benchMpr.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\runProgram.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testArgv.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testBuf.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testCmd.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testCond.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testEvent.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testFile.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testHash.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testList.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testLock.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testMem.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testMpr.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testPath.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testSocket.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testSprintf.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testThread.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testTime.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testUnicode.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\dtoa.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mpr.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprAsync.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprAtomic.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprBuf.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCache.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCmd.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCond.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCrypt.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprDisk.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprDispatcher.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEncode.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEpoll.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEvent.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprFile.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprFileSystem.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprHash.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprJSON.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprKqueue.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprList.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprLock.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprLog.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMem.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMime.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMixed.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprModule.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPath.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPoll.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPrintf.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprRomFile.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSelect.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSignal.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSocket.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprString.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprTest.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprThread.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprTime.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprUnix.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprVxworks.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWait.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWide.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWin.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWince.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprXml.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMatrixssl.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprOpenssl.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSsl.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\manager.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\makerom.o
	rm -f C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\charGen.o

macosx-i686-debug\obj\dtoa.o: \
        src\dtoa.c \
        macosx-i686-debug\inc\bit.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\dtoa.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\dtoa.c

macosx-i686-debug\obj\mpr.o: \
        src\mpr.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mpr.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mpr.c

macosx-i686-debug\obj\mprAsync.o: \
        src\mprAsync.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprAsync.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprAsync.c

macosx-i686-debug\obj\mprAtomic.o: \
        src\mprAtomic.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprAtomic.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprAtomic.c

macosx-i686-debug\obj\mprBuf.o: \
        src\mprBuf.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprBuf.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprBuf.c

macosx-i686-debug\obj\mprCache.o: \
        src\mprCache.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCache.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCache.c

macosx-i686-debug\obj\mprCmd.o: \
        src\mprCmd.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCmd.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCmd.c

macosx-i686-debug\obj\mprCond.o: \
        src\mprCond.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCond.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCond.c

macosx-i686-debug\obj\mprCrypt.o: \
        src\mprCrypt.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCrypt.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCrypt.c

macosx-i686-debug\obj\mprDisk.o: \
        src\mprDisk.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprDisk.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprDisk.c

macosx-i686-debug\obj\mprDispatcher.o: \
        src\mprDispatcher.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprDispatcher.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprDispatcher.c

macosx-i686-debug\obj\mprEncode.o: \
        src\mprEncode.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEncode.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprEncode.c

macosx-i686-debug\obj\mprEpoll.o: \
        src\mprEpoll.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEpoll.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprEpoll.c

macosx-i686-debug\obj\mprEvent.o: \
        src\mprEvent.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEvent.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprEvent.c

macosx-i686-debug\obj\mprFile.o: \
        src\mprFile.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprFile.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprFile.c

macosx-i686-debug\obj\mprFileSystem.o: \
        src\mprFileSystem.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprFileSystem.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprFileSystem.c

macosx-i686-debug\obj\mprHash.o: \
        src\mprHash.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprHash.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprHash.c

macosx-i686-debug\obj\mprJSON.o: \
        src\mprJSON.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprJSON.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprJSON.c

macosx-i686-debug\obj\mprKqueue.o: \
        src\mprKqueue.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprKqueue.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprKqueue.c

macosx-i686-debug\obj\mprList.o: \
        src\mprList.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprList.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprList.c

macosx-i686-debug\obj\mprLock.o: \
        src\mprLock.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprLock.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprLock.c

macosx-i686-debug\obj\mprLog.o: \
        src\mprLog.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprLog.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprLog.c

macosx-i686-debug\obj\mprMem.o: \
        src\mprMem.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMem.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprMem.c

macosx-i686-debug\obj\mprMime.o: \
        src\mprMime.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMime.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprMime.c

macosx-i686-debug\obj\mprMixed.o: \
        src\mprMixed.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMixed.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprMixed.c

macosx-i686-debug\obj\mprModule.o: \
        src\mprModule.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprModule.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprModule.c

macosx-i686-debug\obj\mprPath.o: \
        src\mprPath.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPath.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprPath.c

macosx-i686-debug\obj\mprPoll.o: \
        src\mprPoll.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPoll.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprPoll.c

macosx-i686-debug\obj\mprPrintf.o: \
        src\mprPrintf.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPrintf.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprPrintf.c

macosx-i686-debug\obj\mprRomFile.o: \
        src\mprRomFile.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprRomFile.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprRomFile.c

macosx-i686-debug\obj\mprSelect.o: \
        src\mprSelect.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSelect.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprSelect.c

macosx-i686-debug\obj\mprSignal.o: \
        src\mprSignal.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSignal.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprSignal.c

macosx-i686-debug\obj\mprSocket.o: \
        src\mprSocket.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSocket.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprSocket.c

macosx-i686-debug\obj\mprString.o: \
        src\mprString.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprString.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprString.c

macosx-i686-debug\obj\mprTest.o: \
        src\mprTest.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprTest.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprTest.c

macosx-i686-debug\obj\mprThread.o: \
        src\mprThread.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprThread.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprThread.c

macosx-i686-debug\obj\mprTime.o: \
        src\mprTime.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprTime.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprTime.c

macosx-i686-debug\obj\mprUnix.o: \
        src\mprUnix.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprUnix.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprUnix.c

macosx-i686-debug\obj\mprVxworks.o: \
        src\mprVxworks.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprVxworks.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprVxworks.c

macosx-i686-debug\obj\mprWait.o: \
        src\mprWait.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWait.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWait.c

macosx-i686-debug\obj\mprWide.o: \
        src\mprWide.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWide.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWide.c

macosx-i686-debug\obj\mprWin.o: \
        src\mprWin.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWin.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWin.c

macosx-i686-debug\obj\mprWince.o: \
        src\mprWince.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWince.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWince.c

macosx-i686-debug\obj\mprXml.o: \
        src\mprXml.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprXml.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprXml.c

macosx-i686-debug\lib\libmpr.dylib:  \
        macosx-i686-debug\obj\dtoa.o \
        macosx-i686-debug\obj\mpr.o \
        macosx-i686-debug\obj\mprAsync.o \
        macosx-i686-debug\obj\mprAtomic.o \
        macosx-i686-debug\obj\mprBuf.o \
        macosx-i686-debug\obj\mprCache.o \
        macosx-i686-debug\obj\mprCmd.o \
        macosx-i686-debug\obj\mprCond.o \
        macosx-i686-debug\obj\mprCrypt.o \
        macosx-i686-debug\obj\mprDisk.o \
        macosx-i686-debug\obj\mprDispatcher.o \
        macosx-i686-debug\obj\mprEncode.o \
        macosx-i686-debug\obj\mprEpoll.o \
        macosx-i686-debug\obj\mprEvent.o \
        macosx-i686-debug\obj\mprFile.o \
        macosx-i686-debug\obj\mprFileSystem.o \
        macosx-i686-debug\obj\mprHash.o \
        macosx-i686-debug\obj\mprJSON.o \
        macosx-i686-debug\obj\mprKqueue.o \
        macosx-i686-debug\obj\mprList.o \
        macosx-i686-debug\obj\mprLock.o \
        macosx-i686-debug\obj\mprLog.o \
        macosx-i686-debug\obj\mprMem.o \
        macosx-i686-debug\obj\mprMime.o \
        macosx-i686-debug\obj\mprMixed.o \
        macosx-i686-debug\obj\mprModule.o \
        macosx-i686-debug\obj\mprPath.o \
        macosx-i686-debug\obj\mprPoll.o \
        macosx-i686-debug\obj\mprPrintf.o \
        macosx-i686-debug\obj\mprRomFile.o \
        macosx-i686-debug\obj\mprSelect.o \
        macosx-i686-debug\obj\mprSignal.o \
        macosx-i686-debug\obj\mprSocket.o \
        macosx-i686-debug\obj\mprString.o \
        macosx-i686-debug\obj\mprTest.o \
        macosx-i686-debug\obj\mprThread.o \
        macosx-i686-debug\obj\mprTime.o \
        macosx-i686-debug\obj\mprUnix.o \
        macosx-i686-debug\obj\mprVxworks.o \
        macosx-i686-debug\obj\mprWait.o \
        macosx-i686-debug\obj\mprWide.o \
        macosx-i686-debug\obj\mprWin.o \
        macosx-i686-debug\obj\mprWince.o \
        macosx-i686-debug\obj\mprXml.o
	$(CC) -dynamiclib -o C:\cygwin\home\mob\mpr\macosx-i686-debug\lib\libmpr.dylib -arch i686 $(LDFLAGS) -install_name @rpath/libmpr.dylib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\dtoa.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mpr.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprAsync.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprAtomic.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprBuf.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCache.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCmd.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCond.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCrypt.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprDisk.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprDispatcher.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEncode.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEpoll.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEvent.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprFile.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprFileSystem.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprHash.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprJSON.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprKqueue.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprList.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprLock.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprLog.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMem.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMime.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMixed.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprModule.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPath.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPoll.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPrintf.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprRomFile.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSelect.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSignal.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSocket.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprString.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprTest.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprThread.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprTime.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprUnix.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprVxworks.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWait.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWide.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWin.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWince.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprXml.o $(LIBS)

macosx-i686-debug\obj\benchMpr.o: \
        test\benchMpr.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\benchMpr.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\benchMpr.c

macosx-i686-debug\bin\benchMpr:  \
        macosx-i686-debug\lib\libmpr.dylib \
        macosx-i686-debug\obj\benchMpr.o
	$(CC) -o C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\benchMpr -arch i686 $(LDFLAGS) -Lmacosx-i686-debug/lib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\benchMpr.o $(LIBS) -lmpr

macosx-i686-debug\obj\runProgram.o: \
        test\runProgram.c \
        macosx-i686-debug\inc\bit.h \
        macosx-i686-debug\inc\buildConfig.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\runProgram.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\runProgram.c

macosx-i686-debug\bin\runProgram:  \
        macosx-i686-debug\obj\runProgram.o
	$(CC) -o C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\runProgram -arch i686 $(LDFLAGS) -Lmacosx-i686-debug/lib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\runProgram.o $(LIBS)

macosx-i686-debug\obj\testArgv.o: \
        test\testArgv.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testArgv.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testArgv.c

macosx-i686-debug\obj\testBuf.o: \
        test\testBuf.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testBuf.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testBuf.c

macosx-i686-debug\obj\testCmd.o: \
        test\testCmd.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testCmd.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testCmd.c

macosx-i686-debug\obj\testCond.o: \
        test\testCond.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testCond.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testCond.c

macosx-i686-debug\obj\testEvent.o: \
        test\testEvent.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testEvent.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testEvent.c

macosx-i686-debug\obj\testFile.o: \
        test\testFile.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testFile.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testFile.c

macosx-i686-debug\obj\testHash.o: \
        test\testHash.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testHash.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testHash.c

macosx-i686-debug\obj\testList.o: \
        test\testList.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testList.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testList.c

macosx-i686-debug\obj\testLock.o: \
        test\testLock.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testLock.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testLock.c

macosx-i686-debug\obj\testMem.o: \
        test\testMem.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testMem.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testMem.c

macosx-i686-debug\obj\testMpr.o: \
        test\testMpr.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testMpr.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testMpr.c

macosx-i686-debug\obj\testPath.o: \
        test\testPath.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testPath.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testPath.c

macosx-i686-debug\obj\testSocket.o: \
        test\testSocket.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testSocket.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testSocket.c

macosx-i686-debug\obj\testSprintf.o: \
        test\testSprintf.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testSprintf.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testSprintf.c

macosx-i686-debug\obj\testThread.o: \
        test\testThread.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testThread.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testThread.c

macosx-i686-debug\obj\testTime.o: \
        test\testTime.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testTime.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testTime.c

macosx-i686-debug\obj\testUnicode.o: \
        test\testUnicode.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testUnicode.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testUnicode.c

macosx-i686-debug\bin\testMpr:  \
        macosx-i686-debug\lib\libmpr.dylib \
        macosx-i686-debug\bin\runProgram \
        macosx-i686-debug\obj\testArgv.o \
        macosx-i686-debug\obj\testBuf.o \
        macosx-i686-debug\obj\testCmd.o \
        macosx-i686-debug\obj\testCond.o \
        macosx-i686-debug\obj\testEvent.o \
        macosx-i686-debug\obj\testFile.o \
        macosx-i686-debug\obj\testHash.o \
        macosx-i686-debug\obj\testList.o \
        macosx-i686-debug\obj\testLock.o \
        macosx-i686-debug\obj\testMem.o \
        macosx-i686-debug\obj\testMpr.o \
        macosx-i686-debug\obj\testPath.o \
        macosx-i686-debug\obj\testSocket.o \
        macosx-i686-debug\obj\testSprintf.o \
        macosx-i686-debug\obj\testThread.o \
        macosx-i686-debug\obj\testTime.o \
        macosx-i686-debug\obj\testUnicode.o
	$(CC) -o C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\testMpr -arch i686 $(LDFLAGS) -Lmacosx-i686-debug/lib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testArgv.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testBuf.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testCmd.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testCond.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testEvent.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testFile.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testHash.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testList.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testLock.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testMem.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testMpr.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testPath.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testSocket.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testSprintf.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testThread.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testTime.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testUnicode.o $(LIBS) -lmpr

macosx-i686-debug\obj\manager.o: \
        src\manager.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\manager.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\manager.c

macosx-i686-debug\bin\manager:  \
        macosx-i686-debug\lib\libmpr.dylib \
        macosx-i686-debug\obj\manager.o
	$(CC) -o C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\manager -arch i686 $(LDFLAGS) -Lmacosx-i686-debug/lib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\manager.o $(LIBS) -lmpr

macosx-i686-debug\obj\makerom.o: \
        src\utils\makerom.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\makerom.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\utils\makerom.c

macosx-i686-debug\bin\makerom:  \
        macosx-i686-debug\lib\libmpr.dylib \
        macosx-i686-debug\obj\makerom.o
	$(CC) -o C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\makerom -arch i686 $(LDFLAGS) -Lmacosx-i686-debug/lib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\makerom.o $(LIBS) -lmpr

macosx-i686-debug\obj\charGen.o: \
        src\utils\charGen.c \
        macosx-i686-debug\inc\bit.h \
        src\mpr.h
	$(CC) -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\charGen.o -arch i686 $(CFLAGS) $(DFLAGS) -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\utils\charGen.c

macosx-i686-debug\bin\chargen:  \
        macosx-i686-debug\lib\libmpr.dylib \
        macosx-i686-debug\obj\charGen.o
	$(CC) -o C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\chargen -arch i686 $(LDFLAGS) -Lmacosx-i686-debug/lib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\charGen.o $(LIBS) -lmpr

