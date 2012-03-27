#
#   build.mk -- Build It Makefile to build Multithreaded Portable Runtime for macosx on x86_64
#

PLATFORM  := macosx-x86_64-debug
CC        := cc
CFLAGS    := -fPIC -Wall -g
DFLAGS    := -DPIC -DCPU=X86_64
IFLAGS    := -I$(PLATFORM)/inc
LDFLAGS   := -Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -L$(PLATFORM)/lib -g -ldl
LIBS      := -lpthread -lm


all: prep \
        $(PLATFORM)/bin/benchMpr \
        $(PLATFORM)/bin/runProgram \
        $(PLATFORM)/bin/testMpr \
        $(PLATFORM)/lib/libmpr.dylib \
        $(PLATFORM)/lib/libmprssl.dylib \
        $(PLATFORM)/bin/manager \
        $(PLATFORM)/bin/makerom \
        $(PLATFORM)/bin/chargen

.PHONY: prep

prep:
	@[ ! -x $(PLATFORM)/inc ] && mkdir -p $(PLATFORM)/inc $(PLATFORM)/obj $(PLATFORM)/lib $(PLATFORM)/bin ; true
	@[ ! -f $(PLATFORM)/inc/buildConfig.h ] && cp projects/buildConfig.$(PLATFORM) $(PLATFORM)/inc/buildConfig.h ; true

clean:
	rm -rf $(PLATFORM)/bin/benchMpr
	rm -rf $(PLATFORM)/bin/runProgram
	rm -rf $(PLATFORM)/bin/testMpr
	rm -rf $(PLATFORM)/lib/libmpr.dylib
	rm -rf $(PLATFORM)/lib/libmprssl.dylib
	rm -rf $(PLATFORM)/bin/manager
	rm -rf $(PLATFORM)/bin/makerom
	rm -rf $(PLATFORM)/bin/chargen
	rm -rf $(PLATFORM)/obj/benchMpr.o
	rm -rf $(PLATFORM)/obj/runProgram.o
	rm -rf $(PLATFORM)/obj/testArgv.o
	rm -rf $(PLATFORM)/obj/testBuf.o
	rm -rf $(PLATFORM)/obj/testCmd.o
	rm -rf $(PLATFORM)/obj/testCond.o
	rm -rf $(PLATFORM)/obj/testEvent.o
	rm -rf $(PLATFORM)/obj/testFile.o
	rm -rf $(PLATFORM)/obj/testHash.o
	rm -rf $(PLATFORM)/obj/testList.o
	rm -rf $(PLATFORM)/obj/testLock.o
	rm -rf $(PLATFORM)/obj/testMem.o
	rm -rf $(PLATFORM)/obj/testMpr.o
	rm -rf $(PLATFORM)/obj/testPath.o
	rm -rf $(PLATFORM)/obj/testSocket.o
	rm -rf $(PLATFORM)/obj/testSprintf.o
	rm -rf $(PLATFORM)/obj/testThread.o
	rm -rf $(PLATFORM)/obj/testTime.o
	rm -rf $(PLATFORM)/obj/testUnicode.o
	rm -rf $(PLATFORM)/obj/dtoa.o
	rm -rf $(PLATFORM)/obj/mpr.o
	rm -rf $(PLATFORM)/obj/mprAsync.o
	rm -rf $(PLATFORM)/obj/mprAtomic.o
	rm -rf $(PLATFORM)/obj/mprBuf.o
	rm -rf $(PLATFORM)/obj/mprCache.o
	rm -rf $(PLATFORM)/obj/mprCmd.o
	rm -rf $(PLATFORM)/obj/mprCond.o
	rm -rf $(PLATFORM)/obj/mprCrypt.o
	rm -rf $(PLATFORM)/obj/mprDisk.o
	rm -rf $(PLATFORM)/obj/mprDispatcher.o
	rm -rf $(PLATFORM)/obj/mprEncode.o
	rm -rf $(PLATFORM)/obj/mprEpoll.o
	rm -rf $(PLATFORM)/obj/mprEvent.o
	rm -rf $(PLATFORM)/obj/mprFile.o
	rm -rf $(PLATFORM)/obj/mprFileSystem.o
	rm -rf $(PLATFORM)/obj/mprHash.o
	rm -rf $(PLATFORM)/obj/mprJSON.o
	rm -rf $(PLATFORM)/obj/mprKqueue.o
	rm -rf $(PLATFORM)/obj/mprList.o
	rm -rf $(PLATFORM)/obj/mprLock.o
	rm -rf $(PLATFORM)/obj/mprLog.o
	rm -rf $(PLATFORM)/obj/mprMem.o
	rm -rf $(PLATFORM)/obj/mprMime.o
	rm -rf $(PLATFORM)/obj/mprMixed.o
	rm -rf $(PLATFORM)/obj/mprModule.o
	rm -rf $(PLATFORM)/obj/mprPath.o
	rm -rf $(PLATFORM)/obj/mprPoll.o
	rm -rf $(PLATFORM)/obj/mprPrintf.o
	rm -rf $(PLATFORM)/obj/mprRomFile.o
	rm -rf $(PLATFORM)/obj/mprSelect.o
	rm -rf $(PLATFORM)/obj/mprSignal.o
	rm -rf $(PLATFORM)/obj/mprSocket.o
	rm -rf $(PLATFORM)/obj/mprString.o
	rm -rf $(PLATFORM)/obj/mprTest.o
	rm -rf $(PLATFORM)/obj/mprThread.o
	rm -rf $(PLATFORM)/obj/mprTime.o
	rm -rf $(PLATFORM)/obj/mprUnix.o
	rm -rf $(PLATFORM)/obj/mprVxworks.o
	rm -rf $(PLATFORM)/obj/mprWait.o
	rm -rf $(PLATFORM)/obj/mprWide.o
	rm -rf $(PLATFORM)/obj/mprWin.o
	rm -rf $(PLATFORM)/obj/mprWince.o
	rm -rf $(PLATFORM)/obj/mprXml.o
	rm -rf $(PLATFORM)/obj/mprMatrixssl.o
	rm -rf $(PLATFORM)/obj/mprOpenssl.o
	rm -rf $(PLATFORM)/obj/mprSsl.o
	rm -rf $(PLATFORM)/obj/manager.o
	rm -rf $(PLATFORM)/obj/makerom.o
	rm -rf $(PLATFORM)/obj/charGen.o

clobber: clean
	rm -fr ./$(PLATFORM)

$(PLATFORM)/obj/dtoa.o: \
        src/dtoa.c \
        $(PLATFORM)/inc/buildConfig.h
	$(CC) -c -o $(PLATFORM)/obj/dtoa.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/dtoa.c

$(PLATFORM)/obj/mpr.o: \
        src/mpr.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mpr.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mpr.c

$(PLATFORM)/obj/mprAsync.o: \
        src/mprAsync.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprAsync.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprAsync.c

$(PLATFORM)/obj/mprAtomic.o: \
        src/mprAtomic.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprAtomic.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprAtomic.c

$(PLATFORM)/obj/mprBuf.o: \
        src/mprBuf.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprBuf.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprBuf.c

$(PLATFORM)/obj/mprCache.o: \
        src/mprCache.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprCache.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprCache.c

$(PLATFORM)/obj/mprCmd.o: \
        src/mprCmd.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprCmd.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprCmd.c

$(PLATFORM)/obj/mprCond.o: \
        src/mprCond.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprCond.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprCond.c

$(PLATFORM)/obj/mprCrypt.o: \
        src/mprCrypt.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprCrypt.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprCrypt.c

$(PLATFORM)/obj/mprDisk.o: \
        src/mprDisk.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprDisk.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprDisk.c

$(PLATFORM)/obj/mprDispatcher.o: \
        src/mprDispatcher.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprDispatcher.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprDispatcher.c

$(PLATFORM)/obj/mprEncode.o: \
        src/mprEncode.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprEncode.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprEncode.c

$(PLATFORM)/obj/mprEpoll.o: \
        src/mprEpoll.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprEpoll.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprEpoll.c

$(PLATFORM)/obj/mprEvent.o: \
        src/mprEvent.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprEvent.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprEvent.c

$(PLATFORM)/obj/mprFile.o: \
        src/mprFile.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprFile.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprFile.c

$(PLATFORM)/obj/mprFileSystem.o: \
        src/mprFileSystem.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprFileSystem.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprFileSystem.c

$(PLATFORM)/obj/mprHash.o: \
        src/mprHash.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprHash.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprHash.c

$(PLATFORM)/obj/mprJSON.o: \
        src/mprJSON.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprJSON.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprJSON.c

$(PLATFORM)/obj/mprKqueue.o: \
        src/mprKqueue.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprKqueue.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprKqueue.c

$(PLATFORM)/obj/mprList.o: \
        src/mprList.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprList.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprList.c

$(PLATFORM)/obj/mprLock.o: \
        src/mprLock.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprLock.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprLock.c

$(PLATFORM)/obj/mprLog.o: \
        src/mprLog.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprLog.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprLog.c

$(PLATFORM)/obj/mprMem.o: \
        src/mprMem.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprMem.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprMem.c

$(PLATFORM)/obj/mprMime.o: \
        src/mprMime.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprMime.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprMime.c

$(PLATFORM)/obj/mprMixed.o: \
        src/mprMixed.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprMixed.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprMixed.c

$(PLATFORM)/obj/mprModule.o: \
        src/mprModule.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprModule.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprModule.c

$(PLATFORM)/obj/mprPath.o: \
        src/mprPath.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprPath.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprPath.c

$(PLATFORM)/obj/mprPoll.o: \
        src/mprPoll.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprPoll.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprPoll.c

$(PLATFORM)/obj/mprPrintf.o: \
        src/mprPrintf.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprPrintf.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprPrintf.c

$(PLATFORM)/obj/mprRomFile.o: \
        src/mprRomFile.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprRomFile.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprRomFile.c

$(PLATFORM)/obj/mprSelect.o: \
        src/mprSelect.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprSelect.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprSelect.c

$(PLATFORM)/obj/mprSignal.o: \
        src/mprSignal.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprSignal.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprSignal.c

$(PLATFORM)/obj/mprSocket.o: \
        src/mprSocket.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprSocket.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprSocket.c

$(PLATFORM)/obj/mprString.o: \
        src/mprString.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprString.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprString.c

$(PLATFORM)/obj/mprTest.o: \
        src/mprTest.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprTest.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprTest.c

$(PLATFORM)/obj/mprThread.o: \
        src/mprThread.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprThread.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprThread.c

$(PLATFORM)/obj/mprTime.o: \
        src/mprTime.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprTime.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprTime.c

$(PLATFORM)/obj/mprUnix.o: \
        src/mprUnix.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprUnix.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprUnix.c

$(PLATFORM)/obj/mprVxworks.o: \
        src/mprVxworks.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprVxworks.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprVxworks.c

$(PLATFORM)/obj/mprWait.o: \
        src/mprWait.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprWait.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprWait.c

$(PLATFORM)/obj/mprWide.o: \
        src/mprWide.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprWide.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprWide.c

$(PLATFORM)/obj/mprWin.o: \
        src/mprWin.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprWin.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprWin.c

$(PLATFORM)/obj/mprWince.o: \
        src/mprWince.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprWince.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprWince.c

$(PLATFORM)/obj/mprXml.o: \
        src/mprXml.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprXml.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/mprXml.c

$(PLATFORM)/lib/libmpr.dylib:  \
        $(PLATFORM)/obj/dtoa.o \
        $(PLATFORM)/obj/mpr.o \
        $(PLATFORM)/obj/mprAsync.o \
        $(PLATFORM)/obj/mprAtomic.o \
        $(PLATFORM)/obj/mprBuf.o \
        $(PLATFORM)/obj/mprCache.o \
        $(PLATFORM)/obj/mprCmd.o \
        $(PLATFORM)/obj/mprCond.o \
        $(PLATFORM)/obj/mprCrypt.o \
        $(PLATFORM)/obj/mprDisk.o \
        $(PLATFORM)/obj/mprDispatcher.o \
        $(PLATFORM)/obj/mprEncode.o \
        $(PLATFORM)/obj/mprEpoll.o \
        $(PLATFORM)/obj/mprEvent.o \
        $(PLATFORM)/obj/mprFile.o \
        $(PLATFORM)/obj/mprFileSystem.o \
        $(PLATFORM)/obj/mprHash.o \
        $(PLATFORM)/obj/mprJSON.o \
        $(PLATFORM)/obj/mprKqueue.o \
        $(PLATFORM)/obj/mprList.o \
        $(PLATFORM)/obj/mprLock.o \
        $(PLATFORM)/obj/mprLog.o \
        $(PLATFORM)/obj/mprMem.o \
        $(PLATFORM)/obj/mprMime.o \
        $(PLATFORM)/obj/mprMixed.o \
        $(PLATFORM)/obj/mprModule.o \
        $(PLATFORM)/obj/mprPath.o \
        $(PLATFORM)/obj/mprPoll.o \
        $(PLATFORM)/obj/mprPrintf.o \
        $(PLATFORM)/obj/mprRomFile.o \
        $(PLATFORM)/obj/mprSelect.o \
        $(PLATFORM)/obj/mprSignal.o \
        $(PLATFORM)/obj/mprSocket.o \
        $(PLATFORM)/obj/mprString.o \
        $(PLATFORM)/obj/mprTest.o \
        $(PLATFORM)/obj/mprThread.o \
        $(PLATFORM)/obj/mprTime.o \
        $(PLATFORM)/obj/mprUnix.o \
        $(PLATFORM)/obj/mprVxworks.o \
        $(PLATFORM)/obj/mprWait.o \
        $(PLATFORM)/obj/mprWide.o \
        $(PLATFORM)/obj/mprWin.o \
        $(PLATFORM)/obj/mprWince.o \
        $(PLATFORM)/obj/mprXml.o
	$(CC) -dynamiclib -o $(PLATFORM)/lib/libmpr.dylib -arch x86_64 $(LDFLAGS) -install_name @rpath/libmpr.dylib $(PLATFORM)/obj/dtoa.o $(PLATFORM)/obj/mpr.o $(PLATFORM)/obj/mprAsync.o $(PLATFORM)/obj/mprAtomic.o $(PLATFORM)/obj/mprBuf.o $(PLATFORM)/obj/mprCache.o $(PLATFORM)/obj/mprCmd.o $(PLATFORM)/obj/mprCond.o $(PLATFORM)/obj/mprCrypt.o $(PLATFORM)/obj/mprDisk.o $(PLATFORM)/obj/mprDispatcher.o $(PLATFORM)/obj/mprEncode.o $(PLATFORM)/obj/mprEpoll.o $(PLATFORM)/obj/mprEvent.o $(PLATFORM)/obj/mprFile.o $(PLATFORM)/obj/mprFileSystem.o $(PLATFORM)/obj/mprHash.o $(PLATFORM)/obj/mprJSON.o $(PLATFORM)/obj/mprKqueue.o $(PLATFORM)/obj/mprList.o $(PLATFORM)/obj/mprLock.o $(PLATFORM)/obj/mprLog.o $(PLATFORM)/obj/mprMem.o $(PLATFORM)/obj/mprMime.o $(PLATFORM)/obj/mprMixed.o $(PLATFORM)/obj/mprModule.o $(PLATFORM)/obj/mprPath.o $(PLATFORM)/obj/mprPoll.o $(PLATFORM)/obj/mprPrintf.o $(PLATFORM)/obj/mprRomFile.o $(PLATFORM)/obj/mprSelect.o $(PLATFORM)/obj/mprSignal.o $(PLATFORM)/obj/mprSocket.o $(PLATFORM)/obj/mprString.o $(PLATFORM)/obj/mprTest.o $(PLATFORM)/obj/mprThread.o $(PLATFORM)/obj/mprTime.o $(PLATFORM)/obj/mprUnix.o $(PLATFORM)/obj/mprVxworks.o $(PLATFORM)/obj/mprWait.o $(PLATFORM)/obj/mprWide.o $(PLATFORM)/obj/mprWin.o $(PLATFORM)/obj/mprWince.o $(PLATFORM)/obj/mprXml.o $(LIBS)

$(PLATFORM)/obj/benchMpr.o: \
        test/benchMpr.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/benchMpr.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/benchMpr.c

$(PLATFORM)/bin/benchMpr:  \
        $(PLATFORM)/lib/libmpr.dylib \
        $(PLATFORM)/obj/benchMpr.o
	$(CC) -o $(PLATFORM)/bin/benchMpr -arch x86_64 $(LDFLAGS) -L$(PLATFORM)/lib $(PLATFORM)/obj/benchMpr.o $(LIBS) -lmpr

$(PLATFORM)/obj/runProgram.o: \
        test/runProgram.c \
        $(PLATFORM)/inc/buildConfig.h
	$(CC) -c -o $(PLATFORM)/obj/runProgram.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/runProgram.c

$(PLATFORM)/bin/runProgram:  \
        $(PLATFORM)/obj/runProgram.o
	$(CC) -o $(PLATFORM)/bin/runProgram -arch x86_64 $(LDFLAGS) -L$(PLATFORM)/lib $(PLATFORM)/obj/runProgram.o $(LIBS)

$(PLATFORM)/obj/mprMatrixssl.o: \
        src/mprMatrixssl.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprMatrixssl.o -arch x86_64 $(CFLAGS) $(DFLAGS) -DPOSIX -DMATRIX_USE_FILE_SYSTEM -I$(PLATFORM)/inc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprMatrixssl.c

$(PLATFORM)/obj/mprOpenssl.o: \
        src/mprOpenssl.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprOpenssl.o -arch x86_64 $(CFLAGS) $(DFLAGS) -DPOSIX -DMATRIX_USE_FILE_SYSTEM -I$(PLATFORM)/inc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprOpenssl.c

$(PLATFORM)/obj/mprSsl.o: \
        src/mprSsl.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/mprSsl.o -arch x86_64 $(CFLAGS) $(DFLAGS) -DPOSIX -DMATRIX_USE_FILE_SYSTEM -I$(PLATFORM)/inc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprSsl.c

$(PLATFORM)/lib/libmprssl.dylib:  \
        $(PLATFORM)/lib/libmpr.dylib \
        $(PLATFORM)/obj/mprMatrixssl.o \
        $(PLATFORM)/obj/mprOpenssl.o \
        $(PLATFORM)/obj/mprSsl.o
	$(CC) -dynamiclib -o $(PLATFORM)/lib/libmprssl.dylib -arch x86_64 $(LDFLAGS) -L/Users/mob/git/packages-macosx-x86_64/openssl/openssl-1.0.0d -L/Users/mob/git/packages-macosx-x86_64/matrixssl/matrixssl-3-3-open -install_name @rpath/libmprssl.dylib $(PLATFORM)/obj/mprMatrixssl.o $(PLATFORM)/obj/mprOpenssl.o $(PLATFORM)/obj/mprSsl.o $(LIBS) -lmpr -lssl -lcrypto -lmatrixssl

$(PLATFORM)/obj/testArgv.o: \
        test/testArgv.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testArgv.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testArgv.c

$(PLATFORM)/obj/testBuf.o: \
        test/testBuf.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testBuf.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testBuf.c

$(PLATFORM)/obj/testCmd.o: \
        test/testCmd.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testCmd.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testCmd.c

$(PLATFORM)/obj/testCond.o: \
        test/testCond.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testCond.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testCond.c

$(PLATFORM)/obj/testEvent.o: \
        test/testEvent.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testEvent.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testEvent.c

$(PLATFORM)/obj/testFile.o: \
        test/testFile.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testFile.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testFile.c

$(PLATFORM)/obj/testHash.o: \
        test/testHash.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testHash.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testHash.c

$(PLATFORM)/obj/testList.o: \
        test/testList.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testList.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testList.c

$(PLATFORM)/obj/testLock.o: \
        test/testLock.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testLock.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testLock.c

$(PLATFORM)/obj/testMem.o: \
        test/testMem.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testMem.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testMem.c

$(PLATFORM)/obj/testMpr.o: \
        test/testMpr.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testMpr.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testMpr.c

$(PLATFORM)/obj/testPath.o: \
        test/testPath.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testPath.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testPath.c

$(PLATFORM)/obj/testSocket.o: \
        test/testSocket.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testSocket.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testSocket.c

$(PLATFORM)/obj/testSprintf.o: \
        test/testSprintf.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testSprintf.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testSprintf.c

$(PLATFORM)/obj/testThread.o: \
        test/testThread.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testThread.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testThread.c

$(PLATFORM)/obj/testTime.o: \
        test/testTime.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testTime.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testTime.c

$(PLATFORM)/obj/testUnicode.o: \
        test/testUnicode.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/testUnicode.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc test/testUnicode.c

$(PLATFORM)/bin/testMpr:  \
        $(PLATFORM)/lib/libmpr.dylib \
        $(PLATFORM)/lib/libmprssl.dylib \
        $(PLATFORM)/bin/runProgram \
        $(PLATFORM)/obj/testArgv.o \
        $(PLATFORM)/obj/testBuf.o \
        $(PLATFORM)/obj/testCmd.o \
        $(PLATFORM)/obj/testCond.o \
        $(PLATFORM)/obj/testEvent.o \
        $(PLATFORM)/obj/testFile.o \
        $(PLATFORM)/obj/testHash.o \
        $(PLATFORM)/obj/testList.o \
        $(PLATFORM)/obj/testLock.o \
        $(PLATFORM)/obj/testMem.o \
        $(PLATFORM)/obj/testMpr.o \
        $(PLATFORM)/obj/testPath.o \
        $(PLATFORM)/obj/testSocket.o \
        $(PLATFORM)/obj/testSprintf.o \
        $(PLATFORM)/obj/testThread.o \
        $(PLATFORM)/obj/testTime.o \
        $(PLATFORM)/obj/testUnicode.o
	$(CC) -o $(PLATFORM)/bin/testMpr -arch x86_64 $(LDFLAGS) -L/Users/mob/git/packages-macosx-x86_64/openssl/openssl-1.0.0d -L/Users/mob/git/packages-macosx-x86_64/matrixssl/matrixssl-3-3-open -L$(PLATFORM)/lib $(PLATFORM)/obj/testArgv.o $(PLATFORM)/obj/testBuf.o $(PLATFORM)/obj/testCmd.o $(PLATFORM)/obj/testCond.o $(PLATFORM)/obj/testEvent.o $(PLATFORM)/obj/testFile.o $(PLATFORM)/obj/testHash.o $(PLATFORM)/obj/testList.o $(PLATFORM)/obj/testLock.o $(PLATFORM)/obj/testMem.o $(PLATFORM)/obj/testMpr.o $(PLATFORM)/obj/testPath.o $(PLATFORM)/obj/testSocket.o $(PLATFORM)/obj/testSprintf.o $(PLATFORM)/obj/testThread.o $(PLATFORM)/obj/testTime.o $(PLATFORM)/obj/testUnicode.o $(LIBS) -lmpr -lmprssl -lssl -lcrypto -lmatrixssl

$(PLATFORM)/obj/manager.o: \
        src/manager.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/manager.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/manager.c

$(PLATFORM)/bin/manager:  \
        $(PLATFORM)/lib/libmpr.dylib \
        $(PLATFORM)/obj/manager.o
	$(CC) -o $(PLATFORM)/bin/manager -arch x86_64 $(LDFLAGS) -L$(PLATFORM)/lib $(PLATFORM)/obj/manager.o $(LIBS) -lmpr

$(PLATFORM)/obj/makerom.o: \
        src/utils/makerom.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/makerom.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/utils/makerom.c

$(PLATFORM)/bin/makerom:  \
        $(PLATFORM)/lib/libmpr.dylib \
        $(PLATFORM)/obj/makerom.o
	$(CC) -o $(PLATFORM)/bin/makerom -arch x86_64 $(LDFLAGS) -L$(PLATFORM)/lib $(PLATFORM)/obj/makerom.o $(LIBS) -lmpr

$(PLATFORM)/obj/charGen.o: \
        src/utils/charGen.c \
        $(PLATFORM)/inc/buildConfig.h \
        $(PLATFORM)/inc/mpr.h
	$(CC) -c -o $(PLATFORM)/obj/charGen.o -arch x86_64 $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc src/utils/charGen.c

$(PLATFORM)/bin/chargen:  \
        $(PLATFORM)/lib/libmpr.dylib \
        $(PLATFORM)/obj/charGen.o
	$(CC) -o $(PLATFORM)/bin/chargen -arch x86_64 $(LDFLAGS) -L$(PLATFORM)/lib $(PLATFORM)/obj/charGen.o $(LIBS) -lmpr

