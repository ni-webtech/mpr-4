#
#   build.mk -- Build It Makefile to build Multithreaded Portable Runtime for win on i686
#

CC        := cl
CFLAGS    := -nologo -GR- -W3 -Zi -Od -MDd
DFLAGS    := -D_REENTRANT -D_MT
IFLAGS    := -Isrc -Iwin-i686-debug/inc
LDFLAGS   := -nologo -libpath:win-i686-debug/bin -nodefaultlib -incremental:no -debug -machine:x86
LIBS      := ws2_32.lib advapi32.lib user32.lib kernel32.lib oldnames.lib msvcrt.lib

export PATH := %VS%/Bin:%VS%/VC/Bin:%VS%/Common7/IDE:%VS%/Common7/Tools:%VS%/SDK/v3.5/bin:%VS%/VC/VCPackages
export INCLUDE := %VS%/INCLUDE:%VS%/VC/INCLUDE
export LIB := %VS%/lib:%VS%/VC/lib
all: \
        win-i686-debug/bin/benchMpr.exe \
        win-i686-debug/bin/runProgram.exe \
        win-i686-debug/bin/testMpr.exe \
        win-i686-debug/bin/libmpr.dll \
        win-i686-debug/bin/manager.exe \
        win-i686-debug/bin/makerom.exe \
        win-i686-debug/bin/chargen.exe

clean:
	rm -f win-i686-debug/bin/benchMpr.exe
	rm -f win-i686-debug/bin/runProgram.exe
	rm -f win-i686-debug/bin/testMpr.exe
	rm -f win-i686-debug/bin/libmpr.dll
	rm -f win-i686-debug/bin/libmprssl.dll
	rm -f win-i686-debug/bin/manager.exe
	rm -f win-i686-debug/bin/makerom.exe
	rm -f win-i686-debug/bin/chargen.exe
	rm -f win-i686-debug/obj/benchMpr.obj
	rm -f win-i686-debug/obj/runProgram.obj
	rm -f win-i686-debug/obj/testArgv.obj
	rm -f win-i686-debug/obj/testBuf.obj
	rm -f win-i686-debug/obj/testCmd.obj
	rm -f win-i686-debug/obj/testCond.obj
	rm -f win-i686-debug/obj/testEvent.obj
	rm -f win-i686-debug/obj/testFile.obj
	rm -f win-i686-debug/obj/testHash.obj
	rm -f win-i686-debug/obj/testList.obj
	rm -f win-i686-debug/obj/testLock.obj
	rm -f win-i686-debug/obj/testMem.obj
	rm -f win-i686-debug/obj/testMpr.obj
	rm -f win-i686-debug/obj/testPath.obj
	rm -f win-i686-debug/obj/testSocket.obj
	rm -f win-i686-debug/obj/testSprintf.obj
	rm -f win-i686-debug/obj/testThread.obj
	rm -f win-i686-debug/obj/testTime.obj
	rm -f win-i686-debug/obj/testUnicode.obj
	rm -f win-i686-debug/obj/dtoa.obj
	rm -f win-i686-debug/obj/mpr.obj
	rm -f win-i686-debug/obj/mprAsync.obj
	rm -f win-i686-debug/obj/mprAtomic.obj
	rm -f win-i686-debug/obj/mprBuf.obj
	rm -f win-i686-debug/obj/mprCache.obj
	rm -f win-i686-debug/obj/mprCmd.obj
	rm -f win-i686-debug/obj/mprCond.obj
	rm -f win-i686-debug/obj/mprCrypt.obj
	rm -f win-i686-debug/obj/mprDisk.obj
	rm -f win-i686-debug/obj/mprDispatcher.obj
	rm -f win-i686-debug/obj/mprEncode.obj
	rm -f win-i686-debug/obj/mprEpoll.obj
	rm -f win-i686-debug/obj/mprEvent.obj
	rm -f win-i686-debug/obj/mprFile.obj
	rm -f win-i686-debug/obj/mprFileSystem.obj
	rm -f win-i686-debug/obj/mprHash.obj
	rm -f win-i686-debug/obj/mprJSON.obj
	rm -f win-i686-debug/obj/mprKqueue.obj
	rm -f win-i686-debug/obj/mprList.obj
	rm -f win-i686-debug/obj/mprLock.obj
	rm -f win-i686-debug/obj/mprLog.obj
	rm -f win-i686-debug/obj/mprMem.obj
	rm -f win-i686-debug/obj/mprMime.obj
	rm -f win-i686-debug/obj/mprMixed.obj
	rm -f win-i686-debug/obj/mprModule.obj
	rm -f win-i686-debug/obj/mprPath.obj
	rm -f win-i686-debug/obj/mprPoll.obj
	rm -f win-i686-debug/obj/mprPrintf.obj
	rm -f win-i686-debug/obj/mprRomFile.obj
	rm -f win-i686-debug/obj/mprSelect.obj
	rm -f win-i686-debug/obj/mprSignal.obj
	rm -f win-i686-debug/obj/mprSocket.obj
	rm -f win-i686-debug/obj/mprString.obj
	rm -f win-i686-debug/obj/mprTest.obj
	rm -f win-i686-debug/obj/mprThread.obj
	rm -f win-i686-debug/obj/mprTime.obj
	rm -f win-i686-debug/obj/mprUnix.obj
	rm -f win-i686-debug/obj/mprVxworks.obj
	rm -f win-i686-debug/obj/mprWait.obj
	rm -f win-i686-debug/obj/mprWide.obj
	rm -f win-i686-debug/obj/mprWin.obj
	rm -f win-i686-debug/obj/mprWince.obj
	rm -f win-i686-debug/obj/mprXml.obj
	rm -f win-i686-debug/obj/mprMatrixssl.obj
	rm -f win-i686-debug/obj/mprOpenssl.obj
	rm -f win-i686-debug/obj/mprSsl.obj
	rm -f win-i686-debug/obj/manager.obj
	rm -f win-i686-debug/obj/makerom.obj
	rm -f win-i686-debug/obj/charGen.obj

win-i686-debug/obj/dtoa.obj: \
        src/dtoa.c \
        win-i686-debug/inc/bit.h
	"$(CC)" -c -Fowin-i686-debug/obj/dtoa.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/dtoa.c

win-i686-debug/obj/mpr.obj: \
        src/mpr.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mpr.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mpr.c

win-i686-debug/obj/mprAsync.obj: \
        src/mprAsync.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprAsync.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprAsync.c

win-i686-debug/obj/mprAtomic.obj: \
        src/mprAtomic.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprAtomic.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprAtomic.c

win-i686-debug/obj/mprBuf.obj: \
        src/mprBuf.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprBuf.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprBuf.c

win-i686-debug/obj/mprCache.obj: \
        src/mprCache.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprCache.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprCache.c

win-i686-debug/obj/mprCmd.obj: \
        src/mprCmd.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprCmd.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprCmd.c

win-i686-debug/obj/mprCond.obj: \
        src/mprCond.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprCond.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprCond.c

win-i686-debug/obj/mprCrypt.obj: \
        src/mprCrypt.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprCrypt.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprCrypt.c

win-i686-debug/obj/mprDisk.obj: \
        src/mprDisk.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprDisk.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprDisk.c

win-i686-debug/obj/mprDispatcher.obj: \
        src/mprDispatcher.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprDispatcher.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprDispatcher.c

win-i686-debug/obj/mprEncode.obj: \
        src/mprEncode.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprEncode.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprEncode.c

win-i686-debug/obj/mprEpoll.obj: \
        src/mprEpoll.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprEpoll.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprEpoll.c

win-i686-debug/obj/mprEvent.obj: \
        src/mprEvent.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprEvent.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprEvent.c

win-i686-debug/obj/mprFile.obj: \
        src/mprFile.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprFile.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprFile.c

win-i686-debug/obj/mprFileSystem.obj: \
        src/mprFileSystem.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprFileSystem.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprFileSystem.c

win-i686-debug/obj/mprHash.obj: \
        src/mprHash.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprHash.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprHash.c

win-i686-debug/obj/mprJSON.obj: \
        src/mprJSON.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprJSON.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprJSON.c

win-i686-debug/obj/mprKqueue.obj: \
        src/mprKqueue.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprKqueue.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprKqueue.c

win-i686-debug/obj/mprList.obj: \
        src/mprList.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprList.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprList.c

win-i686-debug/obj/mprLock.obj: \
        src/mprLock.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprLock.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprLock.c

win-i686-debug/obj/mprLog.obj: \
        src/mprLog.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprLog.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprLog.c

win-i686-debug/obj/mprMem.obj: \
        src/mprMem.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprMem.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprMem.c

win-i686-debug/obj/mprMime.obj: \
        src/mprMime.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprMime.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprMime.c

win-i686-debug/obj/mprMixed.obj: \
        src/mprMixed.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprMixed.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprMixed.c

win-i686-debug/obj/mprModule.obj: \
        src/mprModule.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprModule.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprModule.c

win-i686-debug/obj/mprPath.obj: \
        src/mprPath.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprPath.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprPath.c

win-i686-debug/obj/mprPoll.obj: \
        src/mprPoll.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprPoll.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprPoll.c

win-i686-debug/obj/mprPrintf.obj: \
        src/mprPrintf.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprPrintf.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprPrintf.c

win-i686-debug/obj/mprRomFile.obj: \
        src/mprRomFile.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprRomFile.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprRomFile.c

win-i686-debug/obj/mprSelect.obj: \
        src/mprSelect.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprSelect.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprSelect.c

win-i686-debug/obj/mprSignal.obj: \
        src/mprSignal.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprSignal.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprSignal.c

win-i686-debug/obj/mprSocket.obj: \
        src/mprSocket.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprSocket.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprSocket.c

win-i686-debug/obj/mprString.obj: \
        src/mprString.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprString.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprString.c

win-i686-debug/obj/mprTest.obj: \
        src/mprTest.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprTest.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprTest.c

win-i686-debug/obj/mprThread.obj: \
        src/mprThread.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprThread.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprThread.c

win-i686-debug/obj/mprTime.obj: \
        src/mprTime.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprTime.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprTime.c

win-i686-debug/obj/mprUnix.obj: \
        src/mprUnix.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprUnix.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprUnix.c

win-i686-debug/obj/mprVxworks.obj: \
        src/mprVxworks.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprVxworks.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprVxworks.c

win-i686-debug/obj/mprWait.obj: \
        src/mprWait.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprWait.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprWait.c

win-i686-debug/obj/mprWide.obj: \
        src/mprWide.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprWide.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprWide.c

win-i686-debug/obj/mprWin.obj: \
        src/mprWin.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprWin.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprWin.c

win-i686-debug/obj/mprWince.obj: \
        src/mprWince.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprWince.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprWince.c

win-i686-debug/obj/mprXml.obj: \
        src/mprXml.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/mprXml.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/mprXml.c

win-i686-debug/bin/libmpr.dll:  \
        win-i686-debug/obj/dtoa.obj \
        win-i686-debug/obj/mpr.obj \
        win-i686-debug/obj/mprAsync.obj \
        win-i686-debug/obj/mprAtomic.obj \
        win-i686-debug/obj/mprBuf.obj \
        win-i686-debug/obj/mprCache.obj \
        win-i686-debug/obj/mprCmd.obj \
        win-i686-debug/obj/mprCond.obj \
        win-i686-debug/obj/mprCrypt.obj \
        win-i686-debug/obj/mprDisk.obj \
        win-i686-debug/obj/mprDispatcher.obj \
        win-i686-debug/obj/mprEncode.obj \
        win-i686-debug/obj/mprEpoll.obj \
        win-i686-debug/obj/mprEvent.obj \
        win-i686-debug/obj/mprFile.obj \
        win-i686-debug/obj/mprFileSystem.obj \
        win-i686-debug/obj/mprHash.obj \
        win-i686-debug/obj/mprJSON.obj \
        win-i686-debug/obj/mprKqueue.obj \
        win-i686-debug/obj/mprList.obj \
        win-i686-debug/obj/mprLock.obj \
        win-i686-debug/obj/mprLog.obj \
        win-i686-debug/obj/mprMem.obj \
        win-i686-debug/obj/mprMime.obj \
        win-i686-debug/obj/mprMixed.obj \
        win-i686-debug/obj/mprModule.obj \
        win-i686-debug/obj/mprPath.obj \
        win-i686-debug/obj/mprPoll.obj \
        win-i686-debug/obj/mprPrintf.obj \
        win-i686-debug/obj/mprRomFile.obj \
        win-i686-debug/obj/mprSelect.obj \
        win-i686-debug/obj/mprSignal.obj \
        win-i686-debug/obj/mprSocket.obj \
        win-i686-debug/obj/mprString.obj \
        win-i686-debug/obj/mprTest.obj \
        win-i686-debug/obj/mprThread.obj \
        win-i686-debug/obj/mprTime.obj \
        win-i686-debug/obj/mprUnix.obj \
        win-i686-debug/obj/mprVxworks.obj \
        win-i686-debug/obj/mprWait.obj \
        win-i686-debug/obj/mprWide.obj \
        win-i686-debug/obj/mprWin.obj \
        win-i686-debug/obj/mprWince.obj \
        win-i686-debug/obj/mprXml.obj
	"link" -dll -out:win-i686-debug/bin/libmpr.dll -entry:_DllMainCRTStartup@12 -def:win-i686-debug/bin/libmpr.def $(LDFLAGS) win-i686-debug/obj/dtoa.obj win-i686-debug/obj/mpr.obj win-i686-debug/obj/mprAsync.obj win-i686-debug/obj/mprAtomic.obj win-i686-debug/obj/mprBuf.obj win-i686-debug/obj/mprCache.obj win-i686-debug/obj/mprCmd.obj win-i686-debug/obj/mprCond.obj win-i686-debug/obj/mprCrypt.obj win-i686-debug/obj/mprDisk.obj win-i686-debug/obj/mprDispatcher.obj win-i686-debug/obj/mprEncode.obj win-i686-debug/obj/mprEpoll.obj win-i686-debug/obj/mprEvent.obj win-i686-debug/obj/mprFile.obj win-i686-debug/obj/mprFileSystem.obj win-i686-debug/obj/mprHash.obj win-i686-debug/obj/mprJSON.obj win-i686-debug/obj/mprKqueue.obj win-i686-debug/obj/mprList.obj win-i686-debug/obj/mprLock.obj win-i686-debug/obj/mprLog.obj win-i686-debug/obj/mprMem.obj win-i686-debug/obj/mprMime.obj win-i686-debug/obj/mprMixed.obj win-i686-debug/obj/mprModule.obj win-i686-debug/obj/mprPath.obj win-i686-debug/obj/mprPoll.obj win-i686-debug/obj/mprPrintf.obj win-i686-debug/obj/mprRomFile.obj win-i686-debug/obj/mprSelect.obj win-i686-debug/obj/mprSignal.obj win-i686-debug/obj/mprSocket.obj win-i686-debug/obj/mprString.obj win-i686-debug/obj/mprTest.obj win-i686-debug/obj/mprThread.obj win-i686-debug/obj/mprTime.obj win-i686-debug/obj/mprUnix.obj win-i686-debug/obj/mprVxworks.obj win-i686-debug/obj/mprWait.obj win-i686-debug/obj/mprWide.obj win-i686-debug/obj/mprWin.obj win-i686-debug/obj/mprWince.obj win-i686-debug/obj/mprXml.obj $(LIBS)

win-i686-debug/obj/benchMpr.obj: \
        src/test/benchMpr.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/benchMpr.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/benchMpr.c

win-i686-debug/bin/benchMpr.exe:  \
        win-i686-debug/bin/libmpr.dll \
        win-i686-debug/obj/benchMpr.obj
	"link" -out:win-i686-debug/bin/benchMpr.exe -entry:mainCRTStartup -subsystem:console $(LDFLAGS) win-i686-debug/obj/benchMpr.obj $(LIBS) mpr.lib

win-i686-debug/obj/runProgram.obj: \
        src/test/runProgram.c \
        win-i686-debug/inc/bit.h \
        win-i686-debug/inc/buildConfig.h
	"$(CC)" -c -Fowin-i686-debug/obj/runProgram.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/runProgram.c

win-i686-debug/bin/runProgram.exe:  \
        win-i686-debug/obj/runProgram.obj
	"link" -out:win-i686-debug/bin/runProgram.exe -entry:mainCRTStartup -subsystem:console $(LDFLAGS) win-i686-debug/obj/runProgram.obj $(LIBS)

win-i686-debug/obj/testArgv.obj: \
        src/test/testArgv.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testArgv.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testArgv.c

win-i686-debug/obj/testBuf.obj: \
        src/test/testBuf.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testBuf.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testBuf.c

win-i686-debug/obj/testCmd.obj: \
        src/test/testCmd.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testCmd.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testCmd.c

win-i686-debug/obj/testCond.obj: \
        src/test/testCond.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testCond.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testCond.c

win-i686-debug/obj/testEvent.obj: \
        src/test/testEvent.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testEvent.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testEvent.c

win-i686-debug/obj/testFile.obj: \
        src/test/testFile.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testFile.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testFile.c

win-i686-debug/obj/testHash.obj: \
        src/test/testHash.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testHash.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testHash.c

win-i686-debug/obj/testList.obj: \
        src/test/testList.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testList.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testList.c

win-i686-debug/obj/testLock.obj: \
        src/test/testLock.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testLock.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testLock.c

win-i686-debug/obj/testMem.obj: \
        src/test/testMem.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testMem.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testMem.c

win-i686-debug/obj/testMpr.obj: \
        src/test/testMpr.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testMpr.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testMpr.c

win-i686-debug/obj/testPath.obj: \
        src/test/testPath.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testPath.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testPath.c

win-i686-debug/obj/testSocket.obj: \
        src/test/testSocket.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testSocket.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testSocket.c

win-i686-debug/obj/testSprintf.obj: \
        src/test/testSprintf.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testSprintf.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testSprintf.c

win-i686-debug/obj/testThread.obj: \
        src/test/testThread.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testThread.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testThread.c

win-i686-debug/obj/testTime.obj: \
        src/test/testTime.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testTime.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testTime.c

win-i686-debug/obj/testUnicode.obj: \
        src/test/testUnicode.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/testUnicode.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/test/testUnicode.c

win-i686-debug/bin/testMpr.exe:  \
        win-i686-debug/bin/libmpr.dll \
        win-i686-debug/bin/runProgram.exe \
        win-i686-debug/obj/testArgv.obj \
        win-i686-debug/obj/testBuf.obj \
        win-i686-debug/obj/testCmd.obj \
        win-i686-debug/obj/testCond.obj \
        win-i686-debug/obj/testEvent.obj \
        win-i686-debug/obj/testFile.obj \
        win-i686-debug/obj/testHash.obj \
        win-i686-debug/obj/testList.obj \
        win-i686-debug/obj/testLock.obj \
        win-i686-debug/obj/testMem.obj \
        win-i686-debug/obj/testMpr.obj \
        win-i686-debug/obj/testPath.obj \
        win-i686-debug/obj/testSocket.obj \
        win-i686-debug/obj/testSprintf.obj \
        win-i686-debug/obj/testThread.obj \
        win-i686-debug/obj/testTime.obj \
        win-i686-debug/obj/testUnicode.obj
	"link" -out:win-i686-debug/bin/testMpr.exe -entry:mainCRTStartup -subsystem:console $(LDFLAGS) win-i686-debug/obj/testArgv.obj win-i686-debug/obj/testBuf.obj win-i686-debug/obj/testCmd.obj win-i686-debug/obj/testCond.obj win-i686-debug/obj/testEvent.obj win-i686-debug/obj/testFile.obj win-i686-debug/obj/testHash.obj win-i686-debug/obj/testList.obj win-i686-debug/obj/testLock.obj win-i686-debug/obj/testMem.obj win-i686-debug/obj/testMpr.obj win-i686-debug/obj/testPath.obj win-i686-debug/obj/testSocket.obj win-i686-debug/obj/testSprintf.obj win-i686-debug/obj/testThread.obj win-i686-debug/obj/testTime.obj win-i686-debug/obj/testUnicode.obj $(LIBS) mpr.lib

win-i686-debug/obj/manager.obj: \
        src/manager.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/manager.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/manager.c

win-i686-debug/bin/manager.exe:  \
        win-i686-debug/bin/libmpr.dll \
        win-i686-debug/obj/manager.obj
	"link" -out:win-i686-debug/bin/manager.exe -entry:WinMainCRTStartup -subsystem:Windows $(LDFLAGS) win-i686-debug/obj/manager.obj $(LIBS) mpr.lib shell32.lib

win-i686-debug/obj/makerom.obj: \
        src/utils/makerom.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/makerom.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/utils/makerom.c

win-i686-debug/bin/makerom.exe:  \
        win-i686-debug/bin/libmpr.dll \
        win-i686-debug/obj/makerom.obj
	"link" -out:win-i686-debug/bin/makerom.exe -entry:mainCRTStartup -subsystem:console $(LDFLAGS) win-i686-debug/obj/makerom.obj $(LIBS) mpr.lib

win-i686-debug/obj/charGen.obj: \
        src/utils/charGen.c \
        win-i686-debug/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fowin-i686-debug/obj/charGen.obj -Fdwin-i686-debug/obj $(CFLAGS) $(DFLAGS) -Isrc -Iwin-i686-debug/inc src/utils/charGen.c

win-i686-debug/bin/chargen.exe:  \
        win-i686-debug/bin/libmpr.dll \
        win-i686-debug/obj/charGen.obj
	"link" -out:win-i686-debug/bin/chargen.exe -entry:mainCRTStartup -subsystem:console $(LDFLAGS) win-i686-debug/obj/charGen.obj $(LIBS) mpr.lib

