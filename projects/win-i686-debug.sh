#
#   build.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

CC="cl"
CFLAGS="-nologo -GR- -W3 -Zi -Od -MDd"
DFLAGS="-D_REENTRANT -D_MT"
IFLAGS="-Isrc -Iwin-i686-debug\inc"
LDFLAGS="-nologo -nodefaultlib -incremental:no -libpath:C:\cygwin\home\mob\mpr\win-i686-debug\bin -debug -machine:x86"
LIBS="ws2_32.lib advapi32.lib user32.lib kernel32.lib oldnames.lib msvcrt.lib"

export PATH="%VS%\Bin;%VS%/VC/Bin;%VS%/Common7/IDE;%VS%/Common7/Tools;%VS%/SDK/v3.5/bin;%VS%/VC/VCPackages"
export INCLUDE="%VS%\INCLUDE;%VS%/VC/INCLUDE"
export LIB="%VS%\lib;%VS%/VC/lib"
"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\dtoa.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\dtoa.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mpr.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mpr.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprAsync.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprAsync.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprAtomic.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprAtomic.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprBuf.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprBuf.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprCache.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCache.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprCmd.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCmd.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprCond.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCond.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprCrypt.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCrypt.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprDisk.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprDisk.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprDispatcher.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprDispatcher.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprEncode.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprEncode.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprEpoll.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprEpoll.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprEvent.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprEvent.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprFile.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprFile.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprFileSystem.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprFileSystem.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprHash.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprHash.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprJSON.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprJSON.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprKqueue.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprKqueue.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprList.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprList.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprLock.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprLock.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprLog.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprLog.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprMem.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprMem.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprMime.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprMime.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprMixed.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprMixed.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprModule.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprModule.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprPath.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprPath.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprPoll.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprPoll.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprPrintf.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprPrintf.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprRomFile.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprRomFile.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprSelect.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprSelect.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprSignal.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprSignal.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprSocket.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprSocket.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprString.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprString.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprTest.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprTest.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprThread.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprThread.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprTime.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprTime.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprUnix.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprUnix.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprVxworks.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprVxworks.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprWait.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWait.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprWide.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWide.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprWin.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWin.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprWince.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWince.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\mprXml.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprXml.c

"link" -dll -out:C:\cygwin\home\mob\mpr\win-i686-debug\bin\libmpr.dll -entry:_DllMainCRTStartup@12 -def:C:\cygwin\home\mob\mpr\win-i686-debug\bin\libmpr.def ${LDFLAGS} C:\cygwin\home\mob\mpr\win-i686-debug\obj\dtoa.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mpr.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprAsync.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprAtomic.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprBuf.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprCache.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprCmd.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprCond.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprCrypt.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprDisk.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprDispatcher.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprEncode.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprEpoll.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprEvent.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprFile.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprFileSystem.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprHash.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprJSON.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprKqueue.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprList.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprLock.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprLog.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprMem.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprMime.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprMixed.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprModule.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprPath.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprPoll.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprPrintf.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprRomFile.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprSelect.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprSignal.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprSocket.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprString.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprTest.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprThread.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprTime.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprUnix.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprVxworks.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprWait.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprWide.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprWin.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprWince.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\mprXml.obj ${LIBS}

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\benchMpr.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\benchMpr.c

"link" -out:C:\cygwin\home\mob\mpr\win-i686-debug\bin\benchMpr.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} C:\cygwin\home\mob\mpr\win-i686-debug\obj\benchMpr.obj ${LIBS} C:\cygwin\home\mob\mpr\win-i686-debug\bin\libmpr.lib

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\runProgram.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\runProgram.c

"link" -out:C:\cygwin\home\mob\mpr\win-i686-debug\bin\runProgram.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} C:\cygwin\home\mob\mpr\win-i686-debug\obj\runProgram.obj ${LIBS}

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testArgv.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testArgv.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testBuf.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testBuf.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testCmd.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testCmd.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testCond.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testCond.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testEvent.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testEvent.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testFile.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testFile.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testHash.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testHash.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testList.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testList.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testLock.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testLock.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testMem.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testMem.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testMpr.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testMpr.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testPath.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testPath.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testSocket.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testSocket.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testSprintf.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testSprintf.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testThread.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testThread.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testTime.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testTime.c

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\testUnicode.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\test\testUnicode.c

"link" -out:C:\cygwin\home\mob\mpr\win-i686-debug\bin\testMpr.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} C:\cygwin\home\mob\mpr\win-i686-debug\obj\testArgv.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testBuf.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testCmd.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testCond.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testEvent.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testFile.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testHash.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testList.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testLock.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testMem.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testMpr.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testPath.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testSocket.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testSprintf.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testThread.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testTime.obj C:\cygwin\home\mob\mpr\win-i686-debug\obj\testUnicode.obj ${LIBS} C:\cygwin\home\mob\mpr\win-i686-debug\bin\libmpr.lib

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\manager.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\manager.c

"link" -out:C:\cygwin\home\mob\mpr\win-i686-debug\bin\manager.exe -entry:WinMainCRTStartup -subsystem:Windows ${LDFLAGS} C:\cygwin\home\mob\mpr\win-i686-debug\obj\manager.obj ${LIBS} C:\cygwin\home\mob\mpr\win-i686-debug\bin\libmpr.lib shell32.lib

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\makerom.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\utils\makerom.c

"link" -out:C:\cygwin\home\mob\mpr\win-i686-debug\bin\makerom.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} C:\cygwin\home\mob\mpr\win-i686-debug\obj\makerom.obj ${LIBS} C:\cygwin\home\mob\mpr\win-i686-debug\bin\libmpr.lib

"${CC}" -c -FoC:\cygwin\home\mob\mpr\win-i686-debug\obj\charGen.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug\inc C:\cygwin\home\mob\mpr\src\utils\charGen.c

"link" -out:C:\cygwin\home\mob\mpr\win-i686-debug\bin\chargen.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} C:\cygwin\home\mob\mpr\win-i686-debug\obj\charGen.obj ${LIBS} C:\cygwin\home\mob\mpr\win-i686-debug\bin\libmpr.lib

