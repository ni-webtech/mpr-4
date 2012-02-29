#
#   build.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

CC="cl"
CFLAGS="-nologo -GR- -W3 -Zi -Od -MDd"
DFLAGS="-D_REENTRANT -D_MT"
IFLAGS="-Isrc -Iwin-i686-debug/inc"
LDFLAGS="-nologo -nodefaultlib -incremental:no -libpath:/Users/mob/git/mpr/win-i686-debug/bin -debug -machine:x86"
LIBS="ws2_32.lib advapi32.lib user32.lib kernel32.lib oldnames.lib msvcrt.lib"

export PATH="%VS%/Bin:%VS%/VC/Bin:%VS%/Common7/IDE:%VS%/Common7/Tools:%VS%/SDK/v3.5/bin:%VS%/VC/VCPackages"
export INCLUDE="%VS%/INCLUDE:%VS%/VC/INCLUDE"
export LIB="%VS%/lib:%VS%/VC/lib"
"${CC}" -c -Fowin-i686-debug/obj/dtoa.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/dtoa.c

"${CC}" -c -Fowin-i686-debug/obj/mpr.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mpr.c

"${CC}" -c -Fowin-i686-debug/obj/mprAsync.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprAsync.c

"${CC}" -c -Fowin-i686-debug/obj/mprAtomic.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprAtomic.c

"${CC}" -c -Fowin-i686-debug/obj/mprBuf.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprBuf.c

"${CC}" -c -Fowin-i686-debug/obj/mprCache.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprCache.c

"${CC}" -c -Fowin-i686-debug/obj/mprCmd.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprCmd.c

"${CC}" -c -Fowin-i686-debug/obj/mprCond.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprCond.c

"${CC}" -c -Fowin-i686-debug/obj/mprCrypt.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprCrypt.c

"${CC}" -c -Fowin-i686-debug/obj/mprDisk.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprDisk.c

"${CC}" -c -Fowin-i686-debug/obj/mprDispatcher.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprDispatcher.c

"${CC}" -c -Fowin-i686-debug/obj/mprEncode.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprEncode.c

"${CC}" -c -Fowin-i686-debug/obj/mprEpoll.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprEpoll.c

"${CC}" -c -Fowin-i686-debug/obj/mprEvent.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprEvent.c

"${CC}" -c -Fowin-i686-debug/obj/mprFile.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprFile.c

"${CC}" -c -Fowin-i686-debug/obj/mprFileSystem.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprFileSystem.c

"${CC}" -c -Fowin-i686-debug/obj/mprHash.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprHash.c

"${CC}" -c -Fowin-i686-debug/obj/mprJSON.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprJSON.c

"${CC}" -c -Fowin-i686-debug/obj/mprKqueue.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprKqueue.c

"${CC}" -c -Fowin-i686-debug/obj/mprList.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprList.c

"${CC}" -c -Fowin-i686-debug/obj/mprLock.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprLock.c

"${CC}" -c -Fowin-i686-debug/obj/mprLog.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprLog.c

"${CC}" -c -Fowin-i686-debug/obj/mprMem.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprMem.c

"${CC}" -c -Fowin-i686-debug/obj/mprMime.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprMime.c

"${CC}" -c -Fowin-i686-debug/obj/mprMixed.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprMixed.c

"${CC}" -c -Fowin-i686-debug/obj/mprModule.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprModule.c

"${CC}" -c -Fowin-i686-debug/obj/mprPath.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprPath.c

"${CC}" -c -Fowin-i686-debug/obj/mprPoll.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprPoll.c

"${CC}" -c -Fowin-i686-debug/obj/mprPrintf.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprPrintf.c

"${CC}" -c -Fowin-i686-debug/obj/mprRomFile.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprRomFile.c

"${CC}" -c -Fowin-i686-debug/obj/mprSelect.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprSelect.c

"${CC}" -c -Fowin-i686-debug/obj/mprSignal.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprSignal.c

"${CC}" -c -Fowin-i686-debug/obj/mprSocket.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprSocket.c

"${CC}" -c -Fowin-i686-debug/obj/mprString.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprString.c

"${CC}" -c -Fowin-i686-debug/obj/mprTest.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprTest.c

"${CC}" -c -Fowin-i686-debug/obj/mprThread.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprThread.c

"${CC}" -c -Fowin-i686-debug/obj/mprTime.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprTime.c

"${CC}" -c -Fowin-i686-debug/obj/mprUnix.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprUnix.c

"${CC}" -c -Fowin-i686-debug/obj/mprVxworks.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprVxworks.c

"${CC}" -c -Fowin-i686-debug/obj/mprWait.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprWait.c

"${CC}" -c -Fowin-i686-debug/obj/mprWide.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprWide.c

"${CC}" -c -Fowin-i686-debug/obj/mprWin.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprWin.c

"${CC}" -c -Fowin-i686-debug/obj/mprWince.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprWince.c

"${CC}" -c -Fowin-i686-debug/obj/mprXml.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/mprXml.c

"link" -dll -out:win-i686-debug/bin/libmpr.dll -entry:_DllMainCRTStartup@12 -def:win-i686-debug/bin/libmpr.def ${LDFLAGS} win-i686-debug/obj/dtoa.obj win-i686-debug/obj/mpr.obj win-i686-debug/obj/mprAsync.obj win-i686-debug/obj/mprAtomic.obj win-i686-debug/obj/mprBuf.obj win-i686-debug/obj/mprCache.obj win-i686-debug/obj/mprCmd.obj win-i686-debug/obj/mprCond.obj win-i686-debug/obj/mprCrypt.obj win-i686-debug/obj/mprDisk.obj win-i686-debug/obj/mprDispatcher.obj win-i686-debug/obj/mprEncode.obj win-i686-debug/obj/mprEpoll.obj win-i686-debug/obj/mprEvent.obj win-i686-debug/obj/mprFile.obj win-i686-debug/obj/mprFileSystem.obj win-i686-debug/obj/mprHash.obj win-i686-debug/obj/mprJSON.obj win-i686-debug/obj/mprKqueue.obj win-i686-debug/obj/mprList.obj win-i686-debug/obj/mprLock.obj win-i686-debug/obj/mprLog.obj win-i686-debug/obj/mprMem.obj win-i686-debug/obj/mprMime.obj win-i686-debug/obj/mprMixed.obj win-i686-debug/obj/mprModule.obj win-i686-debug/obj/mprPath.obj win-i686-debug/obj/mprPoll.obj win-i686-debug/obj/mprPrintf.obj win-i686-debug/obj/mprRomFile.obj win-i686-debug/obj/mprSelect.obj win-i686-debug/obj/mprSignal.obj win-i686-debug/obj/mprSocket.obj win-i686-debug/obj/mprString.obj win-i686-debug/obj/mprTest.obj win-i686-debug/obj/mprThread.obj win-i686-debug/obj/mprTime.obj win-i686-debug/obj/mprUnix.obj win-i686-debug/obj/mprVxworks.obj win-i686-debug/obj/mprWait.obj win-i686-debug/obj/mprWide.obj win-i686-debug/obj/mprWin.obj win-i686-debug/obj/mprWince.obj win-i686-debug/obj/mprXml.obj ${LIBS}

"${CC}" -c -Fowin-i686-debug/obj/benchMpr.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/benchMpr.c

"link" -out:win-i686-debug/bin/benchMpr.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} win-i686-debug/obj/benchMpr.obj ${LIBS} mpr.lib

"${CC}" -c -Fowin-i686-debug/obj/runProgram.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/runProgram.c

"link" -out:win-i686-debug/bin/runProgram.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} win-i686-debug/obj/runProgram.obj ${LIBS}

"${CC}" -c -Fowin-i686-debug/obj/testArgv.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testArgv.c

"${CC}" -c -Fowin-i686-debug/obj/testBuf.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testBuf.c

"${CC}" -c -Fowin-i686-debug/obj/testCmd.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testCmd.c

"${CC}" -c -Fowin-i686-debug/obj/testCond.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testCond.c

"${CC}" -c -Fowin-i686-debug/obj/testEvent.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testEvent.c

"${CC}" -c -Fowin-i686-debug/obj/testFile.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testFile.c

"${CC}" -c -Fowin-i686-debug/obj/testHash.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testHash.c

"${CC}" -c -Fowin-i686-debug/obj/testList.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testList.c

"${CC}" -c -Fowin-i686-debug/obj/testLock.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testLock.c

"${CC}" -c -Fowin-i686-debug/obj/testMem.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testMem.c

"${CC}" -c -Fowin-i686-debug/obj/testMpr.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testMpr.c

"${CC}" -c -Fowin-i686-debug/obj/testPath.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testPath.c

"${CC}" -c -Fowin-i686-debug/obj/testSocket.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testSocket.c

"${CC}" -c -Fowin-i686-debug/obj/testSprintf.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testSprintf.c

"${CC}" -c -Fowin-i686-debug/obj/testThread.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testThread.c

"${CC}" -c -Fowin-i686-debug/obj/testTime.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testTime.c

"${CC}" -c -Fowin-i686-debug/obj/testUnicode.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc test/testUnicode.c

"link" -out:win-i686-debug/bin/testMpr.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} win-i686-debug/obj/testArgv.obj win-i686-debug/obj/testBuf.obj win-i686-debug/obj/testCmd.obj win-i686-debug/obj/testCond.obj win-i686-debug/obj/testEvent.obj win-i686-debug/obj/testFile.obj win-i686-debug/obj/testHash.obj win-i686-debug/obj/testList.obj win-i686-debug/obj/testLock.obj win-i686-debug/obj/testMem.obj win-i686-debug/obj/testMpr.obj win-i686-debug/obj/testPath.obj win-i686-debug/obj/testSocket.obj win-i686-debug/obj/testSprintf.obj win-i686-debug/obj/testThread.obj win-i686-debug/obj/testTime.obj win-i686-debug/obj/testUnicode.obj ${LIBS} mpr.lib

"${CC}" -c -Fowin-i686-debug/obj/manager.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/manager.c

"link" -out:win-i686-debug/bin/manager.exe -entry:WinMainCRTStartup -subsystem:Windows ${LDFLAGS} win-i686-debug/obj/manager.obj ${LIBS} mpr.lib shell32.lib

"${CC}" -c -Fowin-i686-debug/obj/makerom.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/utils/makerom.c

"link" -out:win-i686-debug/bin/makerom.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} win-i686-debug/obj/makerom.obj ${LIBS} mpr.lib

"${CC}" -c -Fowin-i686-debug/obj/charGen.obj -Fdwin-i686-debug/obj ${CFLAGS} ${DFLAGS} -Isrc -Iwin-i686-debug/inc src/utils/charGen.c

"link" -out:win-i686-debug/bin/chargen.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} win-i686-debug/obj/charGen.obj ${LIBS} mpr.lib

