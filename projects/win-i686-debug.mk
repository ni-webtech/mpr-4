#
#   build.mk -- Build It Makefile to build Multithreaded Portable Runtime for win on i686
#

PLATFORM  := win-i686-debug
CC        := cl
CFLAGS    := -nologo -GR- -W3 -Zi -Od -MDd
DFLAGS    := -D_REENTRANT -D_MT
IFLAGS    := -I$(PLATFORM)/inc -Isrc
LDFLAGS   := -nologo -nodefaultlib -incremental:no -libpath:/Users/mob/git/mpr/$(PLATFORM)/bin -debug -machine:x86
LIBS      := ws2_32.lib advapi32.lib user32.lib kernel32.lib oldnames.lib msvcrt.lib

export PATH := %VS%/Bin:%VS%/VC/Bin:%VS%/Common7/IDE:%VS%/Common7/Tools:%VS%/SDK/v3.5/bin:%VS%/VC/VCPackages
export INCLUDE := %VS%/INCLUDE:%VS%/VC/INCLUDE
export LIB := %VS%/lib:%VS%/VC/lib
all: \
        $(PLATFORM)/bin/benchMpr.exe \
        $(PLATFORM)/bin/runProgram.exe \
        $(PLATFORM)/bin/testMpr.exe \
        $(PLATFORM)/bin/libmpr.dll \
        $(PLATFORM)/bin/manager.exe \
        $(PLATFORM)/bin/makerom.exe \
        $(PLATFORM)/bin/chargen.exe

.PHONY: prep

prep:
	@if [ ! -x $(PLATFORM)/inc ] ; then \
		mkdir -p $(PLATFORM)/inc $(PLATFORM)/obj $(PLATFORM)/lib $(PLATFORM)/bin ; \
		cp src/buildConfig.default $(PLATFORM)/inc\
	fi

clean:
	rm -rf $(PLATFORM)/bin/benchMpr.exe
	rm -rf $(PLATFORM)/bin/runProgram.exe
	rm -rf $(PLATFORM)/bin/testMpr.exe
	rm -rf $(PLATFORM)/bin/libmpr.dll
	rm -rf $(PLATFORM)/bin/libmprssl.dll
	rm -rf $(PLATFORM)/bin/manager.exe
	rm -rf $(PLATFORM)/bin/makerom.exe
	rm -rf $(PLATFORM)/bin/chargen.exe
	rm -rf $(PLATFORM)/obj/benchMpr.obj
	rm -rf $(PLATFORM)/obj/runProgram.obj
	rm -rf $(PLATFORM)/obj/testArgv.obj
	rm -rf $(PLATFORM)/obj/testBuf.obj
	rm -rf $(PLATFORM)/obj/testCmd.obj
	rm -rf $(PLATFORM)/obj/testCond.obj
	rm -rf $(PLATFORM)/obj/testEvent.obj
	rm -rf $(PLATFORM)/obj/testFile.obj
	rm -rf $(PLATFORM)/obj/testHash.obj
	rm -rf $(PLATFORM)/obj/testList.obj
	rm -rf $(PLATFORM)/obj/testLock.obj
	rm -rf $(PLATFORM)/obj/testMem.obj
	rm -rf $(PLATFORM)/obj/testMpr.obj
	rm -rf $(PLATFORM)/obj/testPath.obj
	rm -rf $(PLATFORM)/obj/testSocket.obj
	rm -rf $(PLATFORM)/obj/testSprintf.obj
	rm -rf $(PLATFORM)/obj/testThread.obj
	rm -rf $(PLATFORM)/obj/testTime.obj
	rm -rf $(PLATFORM)/obj/testUnicode.obj
	rm -rf $(PLATFORM)/obj/dtoa.obj
	rm -rf $(PLATFORM)/obj/mpr.obj
	rm -rf $(PLATFORM)/obj/mprAsync.obj
	rm -rf $(PLATFORM)/obj/mprAtomic.obj
	rm -rf $(PLATFORM)/obj/mprBuf.obj
	rm -rf $(PLATFORM)/obj/mprCache.obj
	rm -rf $(PLATFORM)/obj/mprCmd.obj
	rm -rf $(PLATFORM)/obj/mprCond.obj
	rm -rf $(PLATFORM)/obj/mprCrypt.obj
	rm -rf $(PLATFORM)/obj/mprDisk.obj
	rm -rf $(PLATFORM)/obj/mprDispatcher.obj
	rm -rf $(PLATFORM)/obj/mprEncode.obj
	rm -rf $(PLATFORM)/obj/mprEpoll.obj
	rm -rf $(PLATFORM)/obj/mprEvent.obj
	rm -rf $(PLATFORM)/obj/mprFile.obj
	rm -rf $(PLATFORM)/obj/mprFileSystem.obj
	rm -rf $(PLATFORM)/obj/mprHash.obj
	rm -rf $(PLATFORM)/obj/mprJSON.obj
	rm -rf $(PLATFORM)/obj/mprKqueue.obj
	rm -rf $(PLATFORM)/obj/mprList.obj
	rm -rf $(PLATFORM)/obj/mprLock.obj
	rm -rf $(PLATFORM)/obj/mprLog.obj
	rm -rf $(PLATFORM)/obj/mprMem.obj
	rm -rf $(PLATFORM)/obj/mprMime.obj
	rm -rf $(PLATFORM)/obj/mprMixed.obj
	rm -rf $(PLATFORM)/obj/mprModule.obj
	rm -rf $(PLATFORM)/obj/mprPath.obj
	rm -rf $(PLATFORM)/obj/mprPoll.obj
	rm -rf $(PLATFORM)/obj/mprPrintf.obj
	rm -rf $(PLATFORM)/obj/mprRomFile.obj
	rm -rf $(PLATFORM)/obj/mprSelect.obj
	rm -rf $(PLATFORM)/obj/mprSignal.obj
	rm -rf $(PLATFORM)/obj/mprSocket.obj
	rm -rf $(PLATFORM)/obj/mprString.obj
	rm -rf $(PLATFORM)/obj/mprTest.obj
	rm -rf $(PLATFORM)/obj/mprThread.obj
	rm -rf $(PLATFORM)/obj/mprTime.obj
	rm -rf $(PLATFORM)/obj/mprUnix.obj
	rm -rf $(PLATFORM)/obj/mprVxworks.obj
	rm -rf $(PLATFORM)/obj/mprWait.obj
	rm -rf $(PLATFORM)/obj/mprWide.obj
	rm -rf $(PLATFORM)/obj/mprWin.obj
	rm -rf $(PLATFORM)/obj/mprWince.obj
	rm -rf $(PLATFORM)/obj/mprXml.obj
	rm -rf $(PLATFORM)/obj/mprMatrixssl.obj
	rm -rf $(PLATFORM)/obj/mprOpenssl.obj
	rm -rf $(PLATFORM)/obj/mprSsl.obj
	rm -rf $(PLATFORM)/obj/manager.obj
	rm -rf $(PLATFORM)/obj/makerom.obj
	rm -rf $(PLATFORM)/obj/charGen.obj

$(PLATFORM)/obj/dtoa.obj: \
        src/dtoa.c \
        $(PLATFORM)/inc/bit.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/dtoa.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/dtoa.c

$(PLATFORM)/obj/mpr.obj: \
        src/mpr.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mpr.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mpr.c

$(PLATFORM)/obj/mprAsync.obj: \
        src/mprAsync.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprAsync.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprAsync.c

$(PLATFORM)/obj/mprAtomic.obj: \
        src/mprAtomic.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprAtomic.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprAtomic.c

$(PLATFORM)/obj/mprBuf.obj: \
        src/mprBuf.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprBuf.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprBuf.c

$(PLATFORM)/obj/mprCache.obj: \
        src/mprCache.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprCache.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprCache.c

$(PLATFORM)/obj/mprCmd.obj: \
        src/mprCmd.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprCmd.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprCmd.c

$(PLATFORM)/obj/mprCond.obj: \
        src/mprCond.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprCond.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprCond.c

$(PLATFORM)/obj/mprCrypt.obj: \
        src/mprCrypt.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprCrypt.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprCrypt.c

$(PLATFORM)/obj/mprDisk.obj: \
        src/mprDisk.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprDisk.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprDisk.c

$(PLATFORM)/obj/mprDispatcher.obj: \
        src/mprDispatcher.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprDispatcher.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprDispatcher.c

$(PLATFORM)/obj/mprEncode.obj: \
        src/mprEncode.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprEncode.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprEncode.c

$(PLATFORM)/obj/mprEpoll.obj: \
        src/mprEpoll.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprEpoll.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprEpoll.c

$(PLATFORM)/obj/mprEvent.obj: \
        src/mprEvent.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprEvent.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprEvent.c

$(PLATFORM)/obj/mprFile.obj: \
        src/mprFile.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprFile.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprFile.c

$(PLATFORM)/obj/mprFileSystem.obj: \
        src/mprFileSystem.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprFileSystem.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprFileSystem.c

$(PLATFORM)/obj/mprHash.obj: \
        src/mprHash.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprHash.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprHash.c

$(PLATFORM)/obj/mprJSON.obj: \
        src/mprJSON.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprJSON.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprJSON.c

$(PLATFORM)/obj/mprKqueue.obj: \
        src/mprKqueue.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprKqueue.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprKqueue.c

$(PLATFORM)/obj/mprList.obj: \
        src/mprList.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprList.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprList.c

$(PLATFORM)/obj/mprLock.obj: \
        src/mprLock.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprLock.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprLock.c

$(PLATFORM)/obj/mprLog.obj: \
        src/mprLog.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprLog.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprLog.c

$(PLATFORM)/obj/mprMem.obj: \
        src/mprMem.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprMem.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprMem.c

$(PLATFORM)/obj/mprMime.obj: \
        src/mprMime.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprMime.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprMime.c

$(PLATFORM)/obj/mprMixed.obj: \
        src/mprMixed.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprMixed.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprMixed.c

$(PLATFORM)/obj/mprModule.obj: \
        src/mprModule.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprModule.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprModule.c

$(PLATFORM)/obj/mprPath.obj: \
        src/mprPath.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprPath.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprPath.c

$(PLATFORM)/obj/mprPoll.obj: \
        src/mprPoll.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprPoll.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprPoll.c

$(PLATFORM)/obj/mprPrintf.obj: \
        src/mprPrintf.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprPrintf.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprPrintf.c

$(PLATFORM)/obj/mprRomFile.obj: \
        src/mprRomFile.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprRomFile.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprRomFile.c

$(PLATFORM)/obj/mprSelect.obj: \
        src/mprSelect.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprSelect.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprSelect.c

$(PLATFORM)/obj/mprSignal.obj: \
        src/mprSignal.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprSignal.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprSignal.c

$(PLATFORM)/obj/mprSocket.obj: \
        src/mprSocket.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprSocket.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprSocket.c

$(PLATFORM)/obj/mprString.obj: \
        src/mprString.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprString.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprString.c

$(PLATFORM)/obj/mprTest.obj: \
        src/mprTest.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprTest.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprTest.c

$(PLATFORM)/obj/mprThread.obj: \
        src/mprThread.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprThread.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprThread.c

$(PLATFORM)/obj/mprTime.obj: \
        src/mprTime.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprTime.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprTime.c

$(PLATFORM)/obj/mprUnix.obj: \
        src/mprUnix.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprUnix.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprUnix.c

$(PLATFORM)/obj/mprVxworks.obj: \
        src/mprVxworks.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprVxworks.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprVxworks.c

$(PLATFORM)/obj/mprWait.obj: \
        src/mprWait.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprWait.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprWait.c

$(PLATFORM)/obj/mprWide.obj: \
        src/mprWide.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprWide.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprWide.c

$(PLATFORM)/obj/mprWin.obj: \
        src/mprWin.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprWin.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprWin.c

$(PLATFORM)/obj/mprWince.obj: \
        src/mprWince.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprWince.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprWince.c

$(PLATFORM)/obj/mprXml.obj: \
        src/mprXml.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/mprXml.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/mprXml.c

$(PLATFORM)/bin/libmpr.dll:  \
        $(PLATFORM)/obj/dtoa.obj \
        $(PLATFORM)/obj/mpr.obj \
        $(PLATFORM)/obj/mprAsync.obj \
        $(PLATFORM)/obj/mprAtomic.obj \
        $(PLATFORM)/obj/mprBuf.obj \
        $(PLATFORM)/obj/mprCache.obj \
        $(PLATFORM)/obj/mprCmd.obj \
        $(PLATFORM)/obj/mprCond.obj \
        $(PLATFORM)/obj/mprCrypt.obj \
        $(PLATFORM)/obj/mprDisk.obj \
        $(PLATFORM)/obj/mprDispatcher.obj \
        $(PLATFORM)/obj/mprEncode.obj \
        $(PLATFORM)/obj/mprEpoll.obj \
        $(PLATFORM)/obj/mprEvent.obj \
        $(PLATFORM)/obj/mprFile.obj \
        $(PLATFORM)/obj/mprFileSystem.obj \
        $(PLATFORM)/obj/mprHash.obj \
        $(PLATFORM)/obj/mprJSON.obj \
        $(PLATFORM)/obj/mprKqueue.obj \
        $(PLATFORM)/obj/mprList.obj \
        $(PLATFORM)/obj/mprLock.obj \
        $(PLATFORM)/obj/mprLog.obj \
        $(PLATFORM)/obj/mprMem.obj \
        $(PLATFORM)/obj/mprMime.obj \
        $(PLATFORM)/obj/mprMixed.obj \
        $(PLATFORM)/obj/mprModule.obj \
        $(PLATFORM)/obj/mprPath.obj \
        $(PLATFORM)/obj/mprPoll.obj \
        $(PLATFORM)/obj/mprPrintf.obj \
        $(PLATFORM)/obj/mprRomFile.obj \
        $(PLATFORM)/obj/mprSelect.obj \
        $(PLATFORM)/obj/mprSignal.obj \
        $(PLATFORM)/obj/mprSocket.obj \
        $(PLATFORM)/obj/mprString.obj \
        $(PLATFORM)/obj/mprTest.obj \
        $(PLATFORM)/obj/mprThread.obj \
        $(PLATFORM)/obj/mprTime.obj \
        $(PLATFORM)/obj/mprUnix.obj \
        $(PLATFORM)/obj/mprVxworks.obj \
        $(PLATFORM)/obj/mprWait.obj \
        $(PLATFORM)/obj/mprWide.obj \
        $(PLATFORM)/obj/mprWin.obj \
        $(PLATFORM)/obj/mprWince.obj \
        $(PLATFORM)/obj/mprXml.obj
	"link" -dll -out:$(PLATFORM)/bin/libmpr.dll -entry:_DllMainCRTStartup@12 -def:$(PLATFORM)/bin/libmpr.def -nologo -nodefaultlib -incremental:no -libpath:$(PLATFORM)/bin -debug -machine:x86 $(PLATFORM)/obj/dtoa.obj $(PLATFORM)/obj/mpr.obj $(PLATFORM)/obj/mprAsync.obj $(PLATFORM)/obj/mprAtomic.obj $(PLATFORM)/obj/mprBuf.obj $(PLATFORM)/obj/mprCache.obj $(PLATFORM)/obj/mprCmd.obj $(PLATFORM)/obj/mprCond.obj $(PLATFORM)/obj/mprCrypt.obj $(PLATFORM)/obj/mprDisk.obj $(PLATFORM)/obj/mprDispatcher.obj $(PLATFORM)/obj/mprEncode.obj $(PLATFORM)/obj/mprEpoll.obj $(PLATFORM)/obj/mprEvent.obj $(PLATFORM)/obj/mprFile.obj $(PLATFORM)/obj/mprFileSystem.obj $(PLATFORM)/obj/mprHash.obj $(PLATFORM)/obj/mprJSON.obj $(PLATFORM)/obj/mprKqueue.obj $(PLATFORM)/obj/mprList.obj $(PLATFORM)/obj/mprLock.obj $(PLATFORM)/obj/mprLog.obj $(PLATFORM)/obj/mprMem.obj $(PLATFORM)/obj/mprMime.obj $(PLATFORM)/obj/mprMixed.obj $(PLATFORM)/obj/mprModule.obj $(PLATFORM)/obj/mprPath.obj $(PLATFORM)/obj/mprPoll.obj $(PLATFORM)/obj/mprPrintf.obj $(PLATFORM)/obj/mprRomFile.obj $(PLATFORM)/obj/mprSelect.obj $(PLATFORM)/obj/mprSignal.obj $(PLATFORM)/obj/mprSocket.obj $(PLATFORM)/obj/mprString.obj $(PLATFORM)/obj/mprTest.obj $(PLATFORM)/obj/mprThread.obj $(PLATFORM)/obj/mprTime.obj $(PLATFORM)/obj/mprUnix.obj $(PLATFORM)/obj/mprVxworks.obj $(PLATFORM)/obj/mprWait.obj $(PLATFORM)/obj/mprWide.obj $(PLATFORM)/obj/mprWin.obj $(PLATFORM)/obj/mprWince.obj $(PLATFORM)/obj/mprXml.obj $(LIBS)

$(PLATFORM)/obj/benchMpr.obj: \
        test/benchMpr.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/benchMpr.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/benchMpr.c

$(PLATFORM)/bin/benchMpr.exe:  \
        $(PLATFORM)/bin/libmpr.dll \
        $(PLATFORM)/obj/benchMpr.obj
	"link" -out:$(PLATFORM)/bin/benchMpr.exe -entry:mainCRTStartup -subsystem:console -nologo -nodefaultlib -incremental:no -libpath:$(PLATFORM)/bin -debug -machine:x86 $(PLATFORM)/obj/benchMpr.obj $(LIBS) mpr.lib

$(PLATFORM)/obj/runProgram.obj: \
        test/runProgram.c \
        $(PLATFORM)/inc/bit.h \
        $(PLATFORM)/inc/buildConfig.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/runProgram.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/runProgram.c

$(PLATFORM)/bin/runProgram.exe:  \
        $(PLATFORM)/obj/runProgram.obj
	"link" -out:$(PLATFORM)/bin/runProgram.exe -entry:mainCRTStartup -subsystem:console -nologo -nodefaultlib -incremental:no -libpath:$(PLATFORM)/bin -debug -machine:x86 $(PLATFORM)/obj/runProgram.obj $(LIBS)

$(PLATFORM)/obj/testArgv.obj: \
        test/testArgv.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testArgv.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testArgv.c

$(PLATFORM)/obj/testBuf.obj: \
        test/testBuf.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testBuf.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testBuf.c

$(PLATFORM)/obj/testCmd.obj: \
        test/testCmd.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testCmd.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testCmd.c

$(PLATFORM)/obj/testCond.obj: \
        test/testCond.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testCond.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testCond.c

$(PLATFORM)/obj/testEvent.obj: \
        test/testEvent.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testEvent.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testEvent.c

$(PLATFORM)/obj/testFile.obj: \
        test/testFile.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testFile.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testFile.c

$(PLATFORM)/obj/testHash.obj: \
        test/testHash.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testHash.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testHash.c

$(PLATFORM)/obj/testList.obj: \
        test/testList.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testList.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testList.c

$(PLATFORM)/obj/testLock.obj: \
        test/testLock.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testLock.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testLock.c

$(PLATFORM)/obj/testMem.obj: \
        test/testMem.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testMem.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testMem.c

$(PLATFORM)/obj/testMpr.obj: \
        test/testMpr.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testMpr.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testMpr.c

$(PLATFORM)/obj/testPath.obj: \
        test/testPath.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testPath.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testPath.c

$(PLATFORM)/obj/testSocket.obj: \
        test/testSocket.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testSocket.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testSocket.c

$(PLATFORM)/obj/testSprintf.obj: \
        test/testSprintf.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testSprintf.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testSprintf.c

$(PLATFORM)/obj/testThread.obj: \
        test/testThread.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testThread.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testThread.c

$(PLATFORM)/obj/testTime.obj: \
        test/testTime.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testTime.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testTime.c

$(PLATFORM)/obj/testUnicode.obj: \
        test/testUnicode.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/testUnicode.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc test/testUnicode.c

$(PLATFORM)/bin/testMpr.exe:  \
        $(PLATFORM)/bin/libmpr.dll \
        $(PLATFORM)/bin/runProgram.exe \
        $(PLATFORM)/obj/testArgv.obj \
        $(PLATFORM)/obj/testBuf.obj \
        $(PLATFORM)/obj/testCmd.obj \
        $(PLATFORM)/obj/testCond.obj \
        $(PLATFORM)/obj/testEvent.obj \
        $(PLATFORM)/obj/testFile.obj \
        $(PLATFORM)/obj/testHash.obj \
        $(PLATFORM)/obj/testList.obj \
        $(PLATFORM)/obj/testLock.obj \
        $(PLATFORM)/obj/testMem.obj \
        $(PLATFORM)/obj/testMpr.obj \
        $(PLATFORM)/obj/testPath.obj \
        $(PLATFORM)/obj/testSocket.obj \
        $(PLATFORM)/obj/testSprintf.obj \
        $(PLATFORM)/obj/testThread.obj \
        $(PLATFORM)/obj/testTime.obj \
        $(PLATFORM)/obj/testUnicode.obj
	"link" -out:$(PLATFORM)/bin/testMpr.exe -entry:mainCRTStartup -subsystem:console -nologo -nodefaultlib -incremental:no -libpath:$(PLATFORM)/bin -debug -machine:x86 $(PLATFORM)/obj/testArgv.obj $(PLATFORM)/obj/testBuf.obj $(PLATFORM)/obj/testCmd.obj $(PLATFORM)/obj/testCond.obj $(PLATFORM)/obj/testEvent.obj $(PLATFORM)/obj/testFile.obj $(PLATFORM)/obj/testHash.obj $(PLATFORM)/obj/testList.obj $(PLATFORM)/obj/testLock.obj $(PLATFORM)/obj/testMem.obj $(PLATFORM)/obj/testMpr.obj $(PLATFORM)/obj/testPath.obj $(PLATFORM)/obj/testSocket.obj $(PLATFORM)/obj/testSprintf.obj $(PLATFORM)/obj/testThread.obj $(PLATFORM)/obj/testTime.obj $(PLATFORM)/obj/testUnicode.obj $(LIBS) mpr.lib

$(PLATFORM)/obj/manager.obj: \
        src/manager.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/manager.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/manager.c

$(PLATFORM)/bin/manager.exe:  \
        $(PLATFORM)/bin/libmpr.dll \
        $(PLATFORM)/obj/manager.obj
	"link" -out:$(PLATFORM)/bin/manager.exe -entry:WinMainCRTStartup -subsystem:Windows -nologo -nodefaultlib -incremental:no -libpath:$(PLATFORM)/bin -debug -machine:x86 $(PLATFORM)/obj/manager.obj $(LIBS) mpr.lib shell32.lib

$(PLATFORM)/obj/makerom.obj: \
        src/utils/makerom.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/makerom.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/utils/makerom.c

$(PLATFORM)/bin/makerom.exe:  \
        $(PLATFORM)/bin/libmpr.dll \
        $(PLATFORM)/obj/makerom.obj
	"link" -out:$(PLATFORM)/bin/makerom.exe -entry:mainCRTStartup -subsystem:console -nologo -nodefaultlib -incremental:no -libpath:$(PLATFORM)/bin -debug -machine:x86 $(PLATFORM)/obj/makerom.obj $(LIBS) mpr.lib

$(PLATFORM)/obj/charGen.obj: \
        src/utils/charGen.c \
        $(PLATFORM)/inc/bit.h \
        src/mpr.h
	"$(CC)" -c -Fo$(PLATFORM)/obj/charGen.obj -Fd$(PLATFORM)/obj $(CFLAGS) $(DFLAGS) -I$(PLATFORM)/inc -Isrc src/utils/charGen.c

$(PLATFORM)/bin/chargen.exe:  \
        $(PLATFORM)/bin/libmpr.dll \
        $(PLATFORM)/obj/charGen.obj
	"link" -out:$(PLATFORM)/bin/chargen.exe -entry:mainCRTStartup -subsystem:console -nologo -nodefaultlib -incremental:no -libpath:$(PLATFORM)/bin -debug -machine:x86 $(PLATFORM)/obj/charGen.obj $(LIBS) mpr.lib

