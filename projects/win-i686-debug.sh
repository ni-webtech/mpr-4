#
#   build.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

PLATFORM="win-i686-debug"
CC="cl"
CFLAGS="-nologo -GR- -W3 -Zi -Od -MDd"
DFLAGS="-D_REENTRANT -D_MT"
IFLAGS="-Iwin-i686-debug/inc -Isrc"
LDFLAGS="-nologo -nodefaultlib -incremental:no -libpath:/Users/mob/git/mpr/${PLATFORM}/bin -debug -machine:x86"
LIBS="ws2_32.lib advapi32.lib user32.lib kernel32.lib oldnames.lib msvcrt.lib"

export PATH="%VS%/Bin:%VS%/VC/Bin:%VS%/Common7/IDE:%VS%/Common7/Tools:%VS%/SDK/v3.5/bin:%VS%/VC/VCPackages"
export INCLUDE="%VS%/INCLUDE:%VS%/VC/INCLUDE"
export LIB="%VS%/lib:%VS%/VC/lib"
"${CC}" -c -Fo${PLATFORM}/obj/dtoa.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/dtoa.c

"${CC}" -c -Fo${PLATFORM}/obj/mpr.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mpr.c

"${CC}" -c -Fo${PLATFORM}/obj/mprAsync.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprAsync.c

"${CC}" -c -Fo${PLATFORM}/obj/mprAtomic.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprAtomic.c

"${CC}" -c -Fo${PLATFORM}/obj/mprBuf.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprBuf.c

"${CC}" -c -Fo${PLATFORM}/obj/mprCache.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprCache.c

"${CC}" -c -Fo${PLATFORM}/obj/mprCmd.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprCmd.c

"${CC}" -c -Fo${PLATFORM}/obj/mprCond.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprCond.c

"${CC}" -c -Fo${PLATFORM}/obj/mprCrypt.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprCrypt.c

"${CC}" -c -Fo${PLATFORM}/obj/mprDisk.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprDisk.c

"${CC}" -c -Fo${PLATFORM}/obj/mprDispatcher.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprDispatcher.c

"${CC}" -c -Fo${PLATFORM}/obj/mprEncode.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprEncode.c

"${CC}" -c -Fo${PLATFORM}/obj/mprEpoll.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprEpoll.c

"${CC}" -c -Fo${PLATFORM}/obj/mprEvent.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprEvent.c

"${CC}" -c -Fo${PLATFORM}/obj/mprFile.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprFile.c

"${CC}" -c -Fo${PLATFORM}/obj/mprFileSystem.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprFileSystem.c

"${CC}" -c -Fo${PLATFORM}/obj/mprHash.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprHash.c

"${CC}" -c -Fo${PLATFORM}/obj/mprJSON.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprJSON.c

"${CC}" -c -Fo${PLATFORM}/obj/mprKqueue.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprKqueue.c

"${CC}" -c -Fo${PLATFORM}/obj/mprList.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprList.c

"${CC}" -c -Fo${PLATFORM}/obj/mprLock.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprLock.c

"${CC}" -c -Fo${PLATFORM}/obj/mprLog.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprLog.c

"${CC}" -c -Fo${PLATFORM}/obj/mprMem.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprMem.c

"${CC}" -c -Fo${PLATFORM}/obj/mprMime.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprMime.c

"${CC}" -c -Fo${PLATFORM}/obj/mprMixed.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprMixed.c

"${CC}" -c -Fo${PLATFORM}/obj/mprModule.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprModule.c

"${CC}" -c -Fo${PLATFORM}/obj/mprPath.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprPath.c

"${CC}" -c -Fo${PLATFORM}/obj/mprPoll.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprPoll.c

"${CC}" -c -Fo${PLATFORM}/obj/mprPrintf.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprPrintf.c

"${CC}" -c -Fo${PLATFORM}/obj/mprRomFile.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprRomFile.c

"${CC}" -c -Fo${PLATFORM}/obj/mprSelect.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprSelect.c

"${CC}" -c -Fo${PLATFORM}/obj/mprSignal.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprSignal.c

"${CC}" -c -Fo${PLATFORM}/obj/mprSocket.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprSocket.c

"${CC}" -c -Fo${PLATFORM}/obj/mprString.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprString.c

"${CC}" -c -Fo${PLATFORM}/obj/mprTest.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprTest.c

"${CC}" -c -Fo${PLATFORM}/obj/mprThread.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprThread.c

"${CC}" -c -Fo${PLATFORM}/obj/mprTime.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprTime.c

"${CC}" -c -Fo${PLATFORM}/obj/mprUnix.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprUnix.c

"${CC}" -c -Fo${PLATFORM}/obj/mprVxworks.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprVxworks.c

"${CC}" -c -Fo${PLATFORM}/obj/mprWait.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprWait.c

"${CC}" -c -Fo${PLATFORM}/obj/mprWide.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprWide.c

"${CC}" -c -Fo${PLATFORM}/obj/mprWin.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprWin.c

"${CC}" -c -Fo${PLATFORM}/obj/mprWince.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprWince.c

"${CC}" -c -Fo${PLATFORM}/obj/mprXml.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprXml.c

"link" -dll -out:${PLATFORM}/bin/libmpr.dll -entry:_DllMainCRTStartup@12 -def:${PLATFORM}/bin/libmpr.def -nologo -nodefaultlib -incremental:no -libpath:${PLATFORM}/bin -debug -machine:x86 ${PLATFORM}/obj/dtoa.obj ${PLATFORM}/obj/mpr.obj ${PLATFORM}/obj/mprAsync.obj ${PLATFORM}/obj/mprAtomic.obj ${PLATFORM}/obj/mprBuf.obj ${PLATFORM}/obj/mprCache.obj ${PLATFORM}/obj/mprCmd.obj ${PLATFORM}/obj/mprCond.obj ${PLATFORM}/obj/mprCrypt.obj ${PLATFORM}/obj/mprDisk.obj ${PLATFORM}/obj/mprDispatcher.obj ${PLATFORM}/obj/mprEncode.obj ${PLATFORM}/obj/mprEpoll.obj ${PLATFORM}/obj/mprEvent.obj ${PLATFORM}/obj/mprFile.obj ${PLATFORM}/obj/mprFileSystem.obj ${PLATFORM}/obj/mprHash.obj ${PLATFORM}/obj/mprJSON.obj ${PLATFORM}/obj/mprKqueue.obj ${PLATFORM}/obj/mprList.obj ${PLATFORM}/obj/mprLock.obj ${PLATFORM}/obj/mprLog.obj ${PLATFORM}/obj/mprMem.obj ${PLATFORM}/obj/mprMime.obj ${PLATFORM}/obj/mprMixed.obj ${PLATFORM}/obj/mprModule.obj ${PLATFORM}/obj/mprPath.obj ${PLATFORM}/obj/mprPoll.obj ${PLATFORM}/obj/mprPrintf.obj ${PLATFORM}/obj/mprRomFile.obj ${PLATFORM}/obj/mprSelect.obj ${PLATFORM}/obj/mprSignal.obj ${PLATFORM}/obj/mprSocket.obj ${PLATFORM}/obj/mprString.obj ${PLATFORM}/obj/mprTest.obj ${PLATFORM}/obj/mprThread.obj ${PLATFORM}/obj/mprTime.obj ${PLATFORM}/obj/mprUnix.obj ${PLATFORM}/obj/mprVxworks.obj ${PLATFORM}/obj/mprWait.obj ${PLATFORM}/obj/mprWide.obj ${PLATFORM}/obj/mprWin.obj ${PLATFORM}/obj/mprWince.obj ${PLATFORM}/obj/mprXml.obj ${LIBS}

"${CC}" -c -Fo${PLATFORM}/obj/benchMpr.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/benchMpr.c

"link" -out:${PLATFORM}/bin/benchMpr.exe -entry:mainCRTStartup -subsystem:console -nologo -nodefaultlib -incremental:no -libpath:${PLATFORM}/bin -debug -machine:x86 ${PLATFORM}/obj/benchMpr.obj ${LIBS} mpr.lib

"${CC}" -c -Fo${PLATFORM}/obj/runProgram.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/runProgram.c

"link" -out:${PLATFORM}/bin/runProgram.exe -entry:mainCRTStartup -subsystem:console -nologo -nodefaultlib -incremental:no -libpath:${PLATFORM}/bin -debug -machine:x86 ${PLATFORM}/obj/runProgram.obj ${LIBS}

"${CC}" -c -Fo${PLATFORM}/obj/testArgv.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testArgv.c

"${CC}" -c -Fo${PLATFORM}/obj/testBuf.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testBuf.c

"${CC}" -c -Fo${PLATFORM}/obj/testCmd.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testCmd.c

"${CC}" -c -Fo${PLATFORM}/obj/testCond.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testCond.c

"${CC}" -c -Fo${PLATFORM}/obj/testEvent.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testEvent.c

"${CC}" -c -Fo${PLATFORM}/obj/testFile.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testFile.c

"${CC}" -c -Fo${PLATFORM}/obj/testHash.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testHash.c

"${CC}" -c -Fo${PLATFORM}/obj/testList.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testList.c

"${CC}" -c -Fo${PLATFORM}/obj/testLock.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testLock.c

"${CC}" -c -Fo${PLATFORM}/obj/testMem.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testMem.c

"${CC}" -c -Fo${PLATFORM}/obj/testMpr.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testMpr.c

"${CC}" -c -Fo${PLATFORM}/obj/testPath.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testPath.c

"${CC}" -c -Fo${PLATFORM}/obj/testSocket.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testSocket.c

"${CC}" -c -Fo${PLATFORM}/obj/testSprintf.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testSprintf.c

"${CC}" -c -Fo${PLATFORM}/obj/testThread.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testThread.c

"${CC}" -c -Fo${PLATFORM}/obj/testTime.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testTime.c

"${CC}" -c -Fo${PLATFORM}/obj/testUnicode.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testUnicode.c

"link" -out:${PLATFORM}/bin/testMpr.exe -entry:mainCRTStartup -subsystem:console -nologo -nodefaultlib -incremental:no -libpath:${PLATFORM}/bin -debug -machine:x86 ${PLATFORM}/obj/testArgv.obj ${PLATFORM}/obj/testBuf.obj ${PLATFORM}/obj/testCmd.obj ${PLATFORM}/obj/testCond.obj ${PLATFORM}/obj/testEvent.obj ${PLATFORM}/obj/testFile.obj ${PLATFORM}/obj/testHash.obj ${PLATFORM}/obj/testList.obj ${PLATFORM}/obj/testLock.obj ${PLATFORM}/obj/testMem.obj ${PLATFORM}/obj/testMpr.obj ${PLATFORM}/obj/testPath.obj ${PLATFORM}/obj/testSocket.obj ${PLATFORM}/obj/testSprintf.obj ${PLATFORM}/obj/testThread.obj ${PLATFORM}/obj/testTime.obj ${PLATFORM}/obj/testUnicode.obj ${LIBS} mpr.lib

"${CC}" -c -Fo${PLATFORM}/obj/manager.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/manager.c

"link" -out:${PLATFORM}/bin/manager.exe -entry:WinMainCRTStartup -subsystem:Windows -nologo -nodefaultlib -incremental:no -libpath:${PLATFORM}/bin -debug -machine:x86 ${PLATFORM}/obj/manager.obj ${LIBS} mpr.lib shell32.lib

"${CC}" -c -Fo${PLATFORM}/obj/makerom.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/utils/makerom.c

"link" -out:${PLATFORM}/bin/makerom.exe -entry:mainCRTStartup -subsystem:console -nologo -nodefaultlib -incremental:no -libpath:${PLATFORM}/bin -debug -machine:x86 ${PLATFORM}/obj/makerom.obj ${LIBS} mpr.lib

"${CC}" -c -Fo${PLATFORM}/obj/charGen.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/utils/charGen.c

"link" -out:${PLATFORM}/bin/chargen.exe -entry:mainCRTStartup -subsystem:console -nologo -nodefaultlib -incremental:no -libpath:${PLATFORM}/bin -debug -machine:x86 ${PLATFORM}/obj/charGen.obj ${LIBS} mpr.lib

