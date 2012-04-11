#
#   win-i686-debug.mk -- Build It Makefile to build Multithreaded Portable Runtime for win on i686
#

VS             := $(VSINSTALLDIR)
VS             ?= $(PROGRAMFILES)\Microsoft Visual Studio 10.0
SDK            := $(WindowsSDKDir)
SDK            ?= $(PROGRAMFILES)\Microsoft SDKs\Windows\v7.1

export         SDK VS
export PATH    := $(SDK)/Bin;$(VS)/VC/Bin;$(VS)/Common7/IDE;$(VS)/Common7/Tools;$(VS)/SDK/v3.5/bin;$(VS)/VC/VCPackages;$(PATH)
export INCLUDE := $(INCLUDE);$(SDK)/INCLUDE;$(VS)/VC/INCLUDE
export LIB     := $(LIB);$(SDK)/lib;$(VS)/VC/lib

CONFIG   := win-i686-debug
CC       := C:/Program Files/Microsoft Visual Studio 10.0/VC/bin/cl.exe
LD       := C:/Program Files/Microsoft Visual Studio 10.0/VC/bin/link.exe
CFLAGS   := -nologo -GR- -W3 -Zi -Od -MDd
DFLAGS   := -D_REENTRANT -D_MT
IFLAGS   := -I$(CONFIG)/inc
LDFLAGS  := '-nologo' '-nodefaultlib' '-incremental:no' '-debug' '-machine:x86'
LIBPATHS := -libpath:$(CONFIG)/bin
LIBS     := ws2_32.lib advapi32.lib user32.lib kernel32.lib oldnames.lib msvcrt.lib shell32.lib

all: prep \
        $(CONFIG)/bin/benchMpr.exe \
        $(CONFIG)/bin/runProgram.exe \
        $(CONFIG)/bin/testMpr.exe \
        $(CONFIG)/bin/libmpr.dll \
        $(CONFIG)/bin/libmprssl.dll \
        $(CONFIG)/bin/manager.exe \
        $(CONFIG)/bin/makerom.exe \
        $(CONFIG)/bin/chargen.exe

.PHONY: prep

prep:
	@[ ! -x $(CONFIG)/inc ] && mkdir -p $(CONFIG)/inc $(CONFIG)/obj $(CONFIG)/lib $(CONFIG)/bin ; true
	@[ ! -f $(CONFIG)/inc/buildConfig.h ] && cp projects/buildConfig.$(CONFIG) $(CONFIG)/inc/buildConfig.h ; true
	@if ! diff $(CONFIG)/inc/buildConfig.h projects/buildConfig.$(CONFIG) >/dev/null ; then\
		echo cp projects/buildConfig.$(CONFIG) $(CONFIG)/inc/buildConfig.h  ; \
		cp projects/buildConfig.$(CONFIG) $(CONFIG)/inc/buildConfig.h  ; \
	fi; true

clean:
	rm -rf $(CONFIG)/bin/benchMpr.exe
	rm -rf $(CONFIG)/bin/runProgram.exe
	rm -rf $(CONFIG)/bin/testMpr.exe
	rm -rf $(CONFIG)/bin/libmpr.dll
	rm -rf $(CONFIG)/bin/libmprssl.dll
	rm -rf $(CONFIG)/bin/manager.exe
	rm -rf $(CONFIG)/bin/makerom.exe
	rm -rf $(CONFIG)/bin/chargen.exe
	rm -rf $(CONFIG)/obj/benchMpr.obj
	rm -rf $(CONFIG)/obj/runProgram.obj
	rm -rf $(CONFIG)/obj/testArgv.obj
	rm -rf $(CONFIG)/obj/testBuf.obj
	rm -rf $(CONFIG)/obj/testCmd.obj
	rm -rf $(CONFIG)/obj/testCond.obj
	rm -rf $(CONFIG)/obj/testEvent.obj
	rm -rf $(CONFIG)/obj/testFile.obj
	rm -rf $(CONFIG)/obj/testHash.obj
	rm -rf $(CONFIG)/obj/testList.obj
	rm -rf $(CONFIG)/obj/testLock.obj
	rm -rf $(CONFIG)/obj/testMem.obj
	rm -rf $(CONFIG)/obj/testMpr.obj
	rm -rf $(CONFIG)/obj/testPath.obj
	rm -rf $(CONFIG)/obj/testSocket.obj
	rm -rf $(CONFIG)/obj/testSprintf.obj
	rm -rf $(CONFIG)/obj/testThread.obj
	rm -rf $(CONFIG)/obj/testTime.obj
	rm -rf $(CONFIG)/obj/testUnicode.obj
	rm -rf $(CONFIG)/obj/dtoa.obj
	rm -rf $(CONFIG)/obj/mpr.obj
	rm -rf $(CONFIG)/obj/mprAsync.obj
	rm -rf $(CONFIG)/obj/mprAtomic.obj
	rm -rf $(CONFIG)/obj/mprBuf.obj
	rm -rf $(CONFIG)/obj/mprCache.obj
	rm -rf $(CONFIG)/obj/mprCmd.obj
	rm -rf $(CONFIG)/obj/mprCond.obj
	rm -rf $(CONFIG)/obj/mprCrypt.obj
	rm -rf $(CONFIG)/obj/mprDisk.obj
	rm -rf $(CONFIG)/obj/mprDispatcher.obj
	rm -rf $(CONFIG)/obj/mprEncode.obj
	rm -rf $(CONFIG)/obj/mprEpoll.obj
	rm -rf $(CONFIG)/obj/mprEvent.obj
	rm -rf $(CONFIG)/obj/mprFile.obj
	rm -rf $(CONFIG)/obj/mprFileSystem.obj
	rm -rf $(CONFIG)/obj/mprHash.obj
	rm -rf $(CONFIG)/obj/mprJSON.obj
	rm -rf $(CONFIG)/obj/mprKqueue.obj
	rm -rf $(CONFIG)/obj/mprList.obj
	rm -rf $(CONFIG)/obj/mprLock.obj
	rm -rf $(CONFIG)/obj/mprLog.obj
	rm -rf $(CONFIG)/obj/mprMem.obj
	rm -rf $(CONFIG)/obj/mprMime.obj
	rm -rf $(CONFIG)/obj/mprMixed.obj
	rm -rf $(CONFIG)/obj/mprModule.obj
	rm -rf $(CONFIG)/obj/mprPath.obj
	rm -rf $(CONFIG)/obj/mprPoll.obj
	rm -rf $(CONFIG)/obj/mprPrintf.obj
	rm -rf $(CONFIG)/obj/mprRomFile.obj
	rm -rf $(CONFIG)/obj/mprSelect.obj
	rm -rf $(CONFIG)/obj/mprSignal.obj
	rm -rf $(CONFIG)/obj/mprSocket.obj
	rm -rf $(CONFIG)/obj/mprString.obj
	rm -rf $(CONFIG)/obj/mprTest.obj
	rm -rf $(CONFIG)/obj/mprThread.obj
	rm -rf $(CONFIG)/obj/mprTime.obj
	rm -rf $(CONFIG)/obj/mprUnix.obj
	rm -rf $(CONFIG)/obj/mprVxworks.obj
	rm -rf $(CONFIG)/obj/mprWait.obj
	rm -rf $(CONFIG)/obj/mprWide.obj
	rm -rf $(CONFIG)/obj/mprWin.obj
	rm -rf $(CONFIG)/obj/mprWince.obj
	rm -rf $(CONFIG)/obj/mprXml.obj
	rm -rf $(CONFIG)/obj/mprMatrixssl.obj
	rm -rf $(CONFIG)/obj/mprOpenssl.obj
	rm -rf $(CONFIG)/obj/mprSsl.obj
	rm -rf $(CONFIG)/obj/manager.obj
	rm -rf $(CONFIG)/obj/makerom.obj
	rm -rf $(CONFIG)/obj/charGen.obj

clobber: clean
	rm -fr ./$(CONFIG)

$(CONFIG)/inc/mpr.h: 
	rm -fr win-i686-debug/inc/mpr.h
	cp -r src\mpr.h win-i686-debug/inc/mpr.h

$(CONFIG)/obj/dtoa.obj: \
        src\dtoa.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/dtoa.obj -Fd$(CONFIG)/obj/dtoa.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\dtoa.c

$(CONFIG)/obj/mpr.obj: \
        src\mpr.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mpr.obj -Fd$(CONFIG)/obj/mpr.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mpr.c

$(CONFIG)/obj/mprAsync.obj: \
        src\mprAsync.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprAsync.obj -Fd$(CONFIG)/obj/mprAsync.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprAsync.c

$(CONFIG)/obj/mprAtomic.obj: \
        src\mprAtomic.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprAtomic.obj -Fd$(CONFIG)/obj/mprAtomic.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprAtomic.c

$(CONFIG)/obj/mprBuf.obj: \
        src\mprBuf.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprBuf.obj -Fd$(CONFIG)/obj/mprBuf.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprBuf.c

$(CONFIG)/obj/mprCache.obj: \
        src\mprCache.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprCache.obj -Fd$(CONFIG)/obj/mprCache.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprCache.c

$(CONFIG)/obj/mprCmd.obj: \
        src\mprCmd.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprCmd.obj -Fd$(CONFIG)/obj/mprCmd.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprCmd.c

$(CONFIG)/obj/mprCond.obj: \
        src\mprCond.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprCond.obj -Fd$(CONFIG)/obj/mprCond.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprCond.c

$(CONFIG)/obj/mprCrypt.obj: \
        src\mprCrypt.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprCrypt.obj -Fd$(CONFIG)/obj/mprCrypt.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprCrypt.c

$(CONFIG)/obj/mprDisk.obj: \
        src\mprDisk.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprDisk.obj -Fd$(CONFIG)/obj/mprDisk.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprDisk.c

$(CONFIG)/obj/mprDispatcher.obj: \
        src\mprDispatcher.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprDispatcher.obj -Fd$(CONFIG)/obj/mprDispatcher.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprDispatcher.c

$(CONFIG)/obj/mprEncode.obj: \
        src\mprEncode.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprEncode.obj -Fd$(CONFIG)/obj/mprEncode.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprEncode.c

$(CONFIG)/obj/mprEpoll.obj: \
        src\mprEpoll.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprEpoll.obj -Fd$(CONFIG)/obj/mprEpoll.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprEpoll.c

$(CONFIG)/obj/mprEvent.obj: \
        src\mprEvent.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprEvent.obj -Fd$(CONFIG)/obj/mprEvent.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprEvent.c

$(CONFIG)/obj/mprFile.obj: \
        src\mprFile.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprFile.obj -Fd$(CONFIG)/obj/mprFile.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprFile.c

$(CONFIG)/obj/mprFileSystem.obj: \
        src\mprFileSystem.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprFileSystem.obj -Fd$(CONFIG)/obj/mprFileSystem.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprFileSystem.c

$(CONFIG)/obj/mprHash.obj: \
        src\mprHash.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprHash.obj -Fd$(CONFIG)/obj/mprHash.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprHash.c

$(CONFIG)/obj/mprJSON.obj: \
        src\mprJSON.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprJSON.obj -Fd$(CONFIG)/obj/mprJSON.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprJSON.c

$(CONFIG)/obj/mprKqueue.obj: \
        src\mprKqueue.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprKqueue.obj -Fd$(CONFIG)/obj/mprKqueue.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprKqueue.c

$(CONFIG)/obj/mprList.obj: \
        src\mprList.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprList.obj -Fd$(CONFIG)/obj/mprList.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprList.c

$(CONFIG)/obj/mprLock.obj: \
        src\mprLock.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprLock.obj -Fd$(CONFIG)/obj/mprLock.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprLock.c

$(CONFIG)/obj/mprLog.obj: \
        src\mprLog.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprLog.obj -Fd$(CONFIG)/obj/mprLog.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprLog.c

$(CONFIG)/obj/mprMem.obj: \
        src\mprMem.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprMem.obj -Fd$(CONFIG)/obj/mprMem.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprMem.c

$(CONFIG)/obj/mprMime.obj: \
        src\mprMime.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprMime.obj -Fd$(CONFIG)/obj/mprMime.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprMime.c

$(CONFIG)/obj/mprMixed.obj: \
        src\mprMixed.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprMixed.obj -Fd$(CONFIG)/obj/mprMixed.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprMixed.c

$(CONFIG)/obj/mprModule.obj: \
        src\mprModule.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprModule.obj -Fd$(CONFIG)/obj/mprModule.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprModule.c

$(CONFIG)/obj/mprPath.obj: \
        src\mprPath.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprPath.obj -Fd$(CONFIG)/obj/mprPath.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprPath.c

$(CONFIG)/obj/mprPoll.obj: \
        src\mprPoll.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprPoll.obj -Fd$(CONFIG)/obj/mprPoll.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprPoll.c

$(CONFIG)/obj/mprPrintf.obj: \
        src\mprPrintf.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprPrintf.obj -Fd$(CONFIG)/obj/mprPrintf.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprPrintf.c

$(CONFIG)/obj/mprRomFile.obj: \
        src\mprRomFile.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprRomFile.obj -Fd$(CONFIG)/obj/mprRomFile.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprRomFile.c

$(CONFIG)/obj/mprSelect.obj: \
        src\mprSelect.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprSelect.obj -Fd$(CONFIG)/obj/mprSelect.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprSelect.c

$(CONFIG)/obj/mprSignal.obj: \
        src\mprSignal.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprSignal.obj -Fd$(CONFIG)/obj/mprSignal.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprSignal.c

$(CONFIG)/obj/mprSocket.obj: \
        src\mprSocket.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprSocket.obj -Fd$(CONFIG)/obj/mprSocket.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprSocket.c

$(CONFIG)/obj/mprString.obj: \
        src\mprString.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprString.obj -Fd$(CONFIG)/obj/mprString.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprString.c

$(CONFIG)/obj/mprTest.obj: \
        src\mprTest.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprTest.obj -Fd$(CONFIG)/obj/mprTest.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprTest.c

$(CONFIG)/obj/mprThread.obj: \
        src\mprThread.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprThread.obj -Fd$(CONFIG)/obj/mprThread.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprThread.c

$(CONFIG)/obj/mprTime.obj: \
        src\mprTime.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprTime.obj -Fd$(CONFIG)/obj/mprTime.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprTime.c

$(CONFIG)/obj/mprUnix.obj: \
        src\mprUnix.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprUnix.obj -Fd$(CONFIG)/obj/mprUnix.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprUnix.c

$(CONFIG)/obj/mprVxworks.obj: \
        src\mprVxworks.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprVxworks.obj -Fd$(CONFIG)/obj/mprVxworks.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprVxworks.c

$(CONFIG)/obj/mprWait.obj: \
        src\mprWait.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprWait.obj -Fd$(CONFIG)/obj/mprWait.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprWait.c

$(CONFIG)/obj/mprWide.obj: \
        src\mprWide.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprWide.obj -Fd$(CONFIG)/obj/mprWide.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprWide.c

$(CONFIG)/obj/mprWin.obj: \
        src\mprWin.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprWin.obj -Fd$(CONFIG)/obj/mprWin.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprWin.c

$(CONFIG)/obj/mprWince.obj: \
        src\mprWince.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprWince.obj -Fd$(CONFIG)/obj/mprWince.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprWince.c

$(CONFIG)/obj/mprXml.obj: \
        src\mprXml.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprXml.obj -Fd$(CONFIG)/obj/mprXml.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\mprXml.c

$(CONFIG)/bin/libmpr.dll:  \
        $(CONFIG)/inc/mpr.h \
        $(CONFIG)/obj/dtoa.obj \
        $(CONFIG)/obj/mpr.obj \
        $(CONFIG)/obj/mprAsync.obj \
        $(CONFIG)/obj/mprAtomic.obj \
        $(CONFIG)/obj/mprBuf.obj \
        $(CONFIG)/obj/mprCache.obj \
        $(CONFIG)/obj/mprCmd.obj \
        $(CONFIG)/obj/mprCond.obj \
        $(CONFIG)/obj/mprCrypt.obj \
        $(CONFIG)/obj/mprDisk.obj \
        $(CONFIG)/obj/mprDispatcher.obj \
        $(CONFIG)/obj/mprEncode.obj \
        $(CONFIG)/obj/mprEpoll.obj \
        $(CONFIG)/obj/mprEvent.obj \
        $(CONFIG)/obj/mprFile.obj \
        $(CONFIG)/obj/mprFileSystem.obj \
        $(CONFIG)/obj/mprHash.obj \
        $(CONFIG)/obj/mprJSON.obj \
        $(CONFIG)/obj/mprKqueue.obj \
        $(CONFIG)/obj/mprList.obj \
        $(CONFIG)/obj/mprLock.obj \
        $(CONFIG)/obj/mprLog.obj \
        $(CONFIG)/obj/mprMem.obj \
        $(CONFIG)/obj/mprMime.obj \
        $(CONFIG)/obj/mprMixed.obj \
        $(CONFIG)/obj/mprModule.obj \
        $(CONFIG)/obj/mprPath.obj \
        $(CONFIG)/obj/mprPoll.obj \
        $(CONFIG)/obj/mprPrintf.obj \
        $(CONFIG)/obj/mprRomFile.obj \
        $(CONFIG)/obj/mprSelect.obj \
        $(CONFIG)/obj/mprSignal.obj \
        $(CONFIG)/obj/mprSocket.obj \
        $(CONFIG)/obj/mprString.obj \
        $(CONFIG)/obj/mprTest.obj \
        $(CONFIG)/obj/mprThread.obj \
        $(CONFIG)/obj/mprTime.obj \
        $(CONFIG)/obj/mprUnix.obj \
        $(CONFIG)/obj/mprVxworks.obj \
        $(CONFIG)/obj/mprWait.obj \
        $(CONFIG)/obj/mprWide.obj \
        $(CONFIG)/obj/mprWin.obj \
        $(CONFIG)/obj/mprWince.obj \
        $(CONFIG)/obj/mprXml.obj
	"$(LD)" -dll -out:$(CONFIG)/bin/libmpr.dll -entry:_DllMainCRTStartup@12 -def:$(CONFIG)/bin/libmpr.def $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/dtoa.obj $(CONFIG)/obj/mpr.obj $(CONFIG)/obj/mprAsync.obj $(CONFIG)/obj/mprAtomic.obj $(CONFIG)/obj/mprBuf.obj $(CONFIG)/obj/mprCache.obj $(CONFIG)/obj/mprCmd.obj $(CONFIG)/obj/mprCond.obj $(CONFIG)/obj/mprCrypt.obj $(CONFIG)/obj/mprDisk.obj $(CONFIG)/obj/mprDispatcher.obj $(CONFIG)/obj/mprEncode.obj $(CONFIG)/obj/mprEpoll.obj $(CONFIG)/obj/mprEvent.obj $(CONFIG)/obj/mprFile.obj $(CONFIG)/obj/mprFileSystem.obj $(CONFIG)/obj/mprHash.obj $(CONFIG)/obj/mprJSON.obj $(CONFIG)/obj/mprKqueue.obj $(CONFIG)/obj/mprList.obj $(CONFIG)/obj/mprLock.obj $(CONFIG)/obj/mprLog.obj $(CONFIG)/obj/mprMem.obj $(CONFIG)/obj/mprMime.obj $(CONFIG)/obj/mprMixed.obj $(CONFIG)/obj/mprModule.obj $(CONFIG)/obj/mprPath.obj $(CONFIG)/obj/mprPoll.obj $(CONFIG)/obj/mprPrintf.obj $(CONFIG)/obj/mprRomFile.obj $(CONFIG)/obj/mprSelect.obj $(CONFIG)/obj/mprSignal.obj $(CONFIG)/obj/mprSocket.obj $(CONFIG)/obj/mprString.obj $(CONFIG)/obj/mprTest.obj $(CONFIG)/obj/mprThread.obj $(CONFIG)/obj/mprTime.obj $(CONFIG)/obj/mprUnix.obj $(CONFIG)/obj/mprVxworks.obj $(CONFIG)/obj/mprWait.obj $(CONFIG)/obj/mprWide.obj $(CONFIG)/obj/mprWin.obj $(CONFIG)/obj/mprWince.obj $(CONFIG)/obj/mprXml.obj $(LIBS)

$(CONFIG)/obj/benchMpr.obj: \
        test\benchMpr.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/benchMpr.obj -Fd$(CONFIG)/obj/benchMpr.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\benchMpr.c

$(CONFIG)/bin/benchMpr.exe:  \
        $(CONFIG)/bin/libmpr.dll \
        $(CONFIG)/obj/benchMpr.obj
	"$(LD)" -out:$(CONFIG)/bin/benchMpr.exe -entry:mainCRTStartup -subsystem:console $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/benchMpr.obj $(LIBS) libmpr.lib

$(CONFIG)/obj/runProgram.obj: \
        test\runProgram.c \
        $(CONFIG)/inc/buildConfig.h
	"$(CC)" -c -Fo$(CONFIG)/obj/runProgram.obj -Fd$(CONFIG)/obj/runProgram.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\runProgram.c

$(CONFIG)/bin/runProgram.exe:  \
        $(CONFIG)/obj/runProgram.obj
	"$(LD)" -out:$(CONFIG)/bin/runProgram.exe -entry:mainCRTStartup -subsystem:console $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/runProgram.obj $(LIBS)

$(CONFIG)/obj/mprMatrixssl.obj: \
        src\mprMatrixssl.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprMatrixssl.obj -Fd$(CONFIG)/obj/mprMatrixssl.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc -I../packages-win-i686/openssl/openssl-1.0.0d/inc32 src\mprMatrixssl.c

$(CONFIG)/obj/mprOpenssl.obj: \
        src\mprOpenssl.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprOpenssl.obj -Fd$(CONFIG)/obj/mprOpenssl.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc -I../packages-win-i686/openssl/openssl-1.0.0d/inc32 src\mprOpenssl.c

$(CONFIG)/obj/mprSsl.obj: \
        src\mprSsl.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/mprSsl.obj -Fd$(CONFIG)/obj/mprSsl.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc -I../packages-win-i686/openssl/openssl-1.0.0d/inc32 src\mprSsl.c

$(CONFIG)/bin/libmprssl.dll:  \
        $(CONFIG)/bin/libmpr.dll \
        $(CONFIG)/obj/mprMatrixssl.obj \
        $(CONFIG)/obj/mprOpenssl.obj \
        $(CONFIG)/obj/mprSsl.obj
	"$(LD)" -dll -out:$(CONFIG)/bin/libmprssl.dll -entry:_DllMainCRTStartup@12 -def:$(CONFIG)/bin/libmprssl.def $(LDFLAGS) $(LIBPATHS) -libpath:C:/cygwin/home/mob/packages-win-i686/openssl/openssl-1.0.0d/out32dll $(CONFIG)/obj/mprMatrixssl.obj $(CONFIG)/obj/mprOpenssl.obj $(CONFIG)/obj/mprSsl.obj $(LIBS) libmpr.lib libeay32.lib ssleay32.lib

$(CONFIG)/obj/testArgv.obj: \
        test\testArgv.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testArgv.obj -Fd$(CONFIG)/obj/testArgv.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testArgv.c

$(CONFIG)/obj/testBuf.obj: \
        test\testBuf.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testBuf.obj -Fd$(CONFIG)/obj/testBuf.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testBuf.c

$(CONFIG)/obj/testCmd.obj: \
        test\testCmd.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testCmd.obj -Fd$(CONFIG)/obj/testCmd.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testCmd.c

$(CONFIG)/obj/testCond.obj: \
        test\testCond.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testCond.obj -Fd$(CONFIG)/obj/testCond.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testCond.c

$(CONFIG)/obj/testEvent.obj: \
        test\testEvent.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testEvent.obj -Fd$(CONFIG)/obj/testEvent.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testEvent.c

$(CONFIG)/obj/testFile.obj: \
        test\testFile.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testFile.obj -Fd$(CONFIG)/obj/testFile.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testFile.c

$(CONFIG)/obj/testHash.obj: \
        test\testHash.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testHash.obj -Fd$(CONFIG)/obj/testHash.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testHash.c

$(CONFIG)/obj/testList.obj: \
        test\testList.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testList.obj -Fd$(CONFIG)/obj/testList.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testList.c

$(CONFIG)/obj/testLock.obj: \
        test\testLock.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testLock.obj -Fd$(CONFIG)/obj/testLock.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testLock.c

$(CONFIG)/obj/testMem.obj: \
        test\testMem.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testMem.obj -Fd$(CONFIG)/obj/testMem.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testMem.c

$(CONFIG)/obj/testMpr.obj: \
        test\testMpr.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testMpr.obj -Fd$(CONFIG)/obj/testMpr.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testMpr.c

$(CONFIG)/obj/testPath.obj: \
        test\testPath.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testPath.obj -Fd$(CONFIG)/obj/testPath.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testPath.c

$(CONFIG)/obj/testSocket.obj: \
        test\testSocket.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testSocket.obj -Fd$(CONFIG)/obj/testSocket.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testSocket.c

$(CONFIG)/obj/testSprintf.obj: \
        test\testSprintf.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testSprintf.obj -Fd$(CONFIG)/obj/testSprintf.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testSprintf.c

$(CONFIG)/obj/testThread.obj: \
        test\testThread.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testThread.obj -Fd$(CONFIG)/obj/testThread.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testThread.c

$(CONFIG)/obj/testTime.obj: \
        test\testTime.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testTime.obj -Fd$(CONFIG)/obj/testTime.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testTime.c

$(CONFIG)/obj/testUnicode.obj: \
        test\testUnicode.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/testUnicode.obj -Fd$(CONFIG)/obj/testUnicode.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc test\testUnicode.c

$(CONFIG)/bin/testMpr.exe:  \
        $(CONFIG)/bin/libmpr.dll \
        $(CONFIG)/bin/libmprssl.dll \
        $(CONFIG)/bin/runProgram.exe \
        $(CONFIG)/obj/testArgv.obj \
        $(CONFIG)/obj/testBuf.obj \
        $(CONFIG)/obj/testCmd.obj \
        $(CONFIG)/obj/testCond.obj \
        $(CONFIG)/obj/testEvent.obj \
        $(CONFIG)/obj/testFile.obj \
        $(CONFIG)/obj/testHash.obj \
        $(CONFIG)/obj/testList.obj \
        $(CONFIG)/obj/testLock.obj \
        $(CONFIG)/obj/testMem.obj \
        $(CONFIG)/obj/testMpr.obj \
        $(CONFIG)/obj/testPath.obj \
        $(CONFIG)/obj/testSocket.obj \
        $(CONFIG)/obj/testSprintf.obj \
        $(CONFIG)/obj/testThread.obj \
        $(CONFIG)/obj/testTime.obj \
        $(CONFIG)/obj/testUnicode.obj
	"$(LD)" -out:$(CONFIG)/bin/testMpr.exe -entry:mainCRTStartup -subsystem:console $(LDFLAGS) $(LIBPATHS) -libpath:C:/cygwin/home/mob/packages-win-i686/openssl/openssl-1.0.0d/out32dll $(CONFIG)/obj/testArgv.obj $(CONFIG)/obj/testBuf.obj $(CONFIG)/obj/testCmd.obj $(CONFIG)/obj/testCond.obj $(CONFIG)/obj/testEvent.obj $(CONFIG)/obj/testFile.obj $(CONFIG)/obj/testHash.obj $(CONFIG)/obj/testList.obj $(CONFIG)/obj/testLock.obj $(CONFIG)/obj/testMem.obj $(CONFIG)/obj/testMpr.obj $(CONFIG)/obj/testPath.obj $(CONFIG)/obj/testSocket.obj $(CONFIG)/obj/testSprintf.obj $(CONFIG)/obj/testThread.obj $(CONFIG)/obj/testTime.obj $(CONFIG)/obj/testUnicode.obj $(LIBS) libmpr.lib libmprssl.lib libeay32.lib ssleay32.lib

$(CONFIG)/obj/manager.obj: \
        src\manager.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/manager.obj -Fd$(CONFIG)/obj/manager.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\manager.c

$(CONFIG)/bin/manager.exe:  \
        $(CONFIG)/bin/libmpr.dll \
        $(CONFIG)/obj/manager.obj
	"$(LD)" -out:$(CONFIG)/bin/manager.exe -entry:WinMainCRTStartup -subsystem:Windows $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/manager.obj $(LIBS) libmpr.lib

$(CONFIG)/obj/makerom.obj: \
        src\utils\makerom.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/makerom.obj -Fd$(CONFIG)/obj/makerom.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\utils\makerom.c

$(CONFIG)/bin/makerom.exe:  \
        $(CONFIG)/bin/libmpr.dll \
        $(CONFIG)/obj/makerom.obj
	"$(LD)" -out:$(CONFIG)/bin/makerom.exe -entry:mainCRTStartup -subsystem:console $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/makerom.obj $(LIBS) libmpr.lib

$(CONFIG)/obj/charGen.obj: \
        src\utils\charGen.c \
        $(CONFIG)/inc/buildConfig.h \
        $(CONFIG)/inc/mpr.h
	"$(CC)" -c -Fo$(CONFIG)/obj/charGen.obj -Fd$(CONFIG)/obj/charGen.pdb $(CFLAGS) $(DFLAGS) -I$(CONFIG)/inc src\utils\charGen.c

$(CONFIG)/bin/chargen.exe:  \
        $(CONFIG)/bin/libmpr.dll \
        $(CONFIG)/obj/charGen.obj
	"$(LD)" -out:$(CONFIG)/bin/chargen.exe -entry:mainCRTStartup -subsystem:console $(LDFLAGS) $(LIBPATHS) $(CONFIG)/obj/charGen.obj $(LIBS) libmpr.lib

