#
#   win-i686-debug.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

VS="${VSINSTALLDIR}"
: ${VS:="$(VS)"}
SDK="${WindowsSDKDir}"
: ${SDK:="$(SDK)"}

export SDK VS
export PATH="$(SDK)/Bin:$(VS)/VC/Bin:$(VS)/Common7/IDE:$(VS)/Common7/Tools:$(VS)/SDK/v3.5/bin:$(VS)/VC/VCPackages;$(PATH)"
export INCLUDE="$(INCLUDE);$(SDK)/INCLUDE:$(VS)/VC/INCLUDE"
export LIB="$(LIB);$(SDK)/lib:$(VS)/VC/lib"

PLATFORM="win-i686-debug"
CC="cl.exe"
LD="link.exe"
CFLAGS="-nologo -GR- -W3 -Zi -Od -MDd"
DFLAGS="-D_REENTRANT -D_MT"
IFLAGS="-Iwin-i686-debug/inc"
LDFLAGS="-nologo -nodefaultlib -incremental:no -debug -machine:x86"
LIBPATHS="-libpath:${PLATFORM}/bin"
LIBS="ws2_32.lib advapi32.lib user32.lib kernel32.lib oldnames.lib msvcrt.lib shell32.lib"

[ ! -x ${PLATFORM}/inc ] && mkdir -p ${PLATFORM}/inc ${PLATFORM}/obj ${PLATFORM}/lib ${PLATFORM}/bin
cp projects/buildConfig.${PLATFORM} ${PLATFORM}/inc/buildConfig.h

rm -rf win-i686-debug/inc/mpr.h
cp -r src/mpr.h win-i686-debug/inc/mpr.h

"${CC}" -c -Fo${PLATFORM}/obj/dtoa.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/dtoa.c

"${CC}" -c -Fo${PLATFORM}/obj/mpr.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mpr.c

"${CC}" -c -Fo${PLATFORM}/obj/mprAsync.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprAsync.c

"${CC}" -c -Fo${PLATFORM}/obj/mprAtomic.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprAtomic.c

"${CC}" -c -Fo${PLATFORM}/obj/mprBuf.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprBuf.c

"${CC}" -c -Fo${PLATFORM}/obj/mprCache.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprCache.c

"${CC}" -c -Fo${PLATFORM}/obj/mprCmd.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprCmd.c

"${CC}" -c -Fo${PLATFORM}/obj/mprCond.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprCond.c

"${CC}" -c -Fo${PLATFORM}/obj/mprCrypt.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprCrypt.c

"${CC}" -c -Fo${PLATFORM}/obj/mprDisk.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprDisk.c

"${CC}" -c -Fo${PLATFORM}/obj/mprDispatcher.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprDispatcher.c

"${CC}" -c -Fo${PLATFORM}/obj/mprEncode.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprEncode.c

"${CC}" -c -Fo${PLATFORM}/obj/mprEpoll.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprEpoll.c

"${CC}" -c -Fo${PLATFORM}/obj/mprEvent.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprEvent.c

"${CC}" -c -Fo${PLATFORM}/obj/mprFile.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprFile.c

"${CC}" -c -Fo${PLATFORM}/obj/mprFileSystem.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprFileSystem.c

"${CC}" -c -Fo${PLATFORM}/obj/mprHash.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprHash.c

"${CC}" -c -Fo${PLATFORM}/obj/mprJSON.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprJSON.c

"${CC}" -c -Fo${PLATFORM}/obj/mprKqueue.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprKqueue.c

"${CC}" -c -Fo${PLATFORM}/obj/mprList.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprList.c

"${CC}" -c -Fo${PLATFORM}/obj/mprLock.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprLock.c

"${CC}" -c -Fo${PLATFORM}/obj/mprLog.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprLog.c

"${CC}" -c -Fo${PLATFORM}/obj/mprMem.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprMem.c

"${CC}" -c -Fo${PLATFORM}/obj/mprMime.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprMime.c

"${CC}" -c -Fo${PLATFORM}/obj/mprMixed.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprMixed.c

"${CC}" -c -Fo${PLATFORM}/obj/mprModule.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprModule.c

"${CC}" -c -Fo${PLATFORM}/obj/mprPath.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprPath.c

"${CC}" -c -Fo${PLATFORM}/obj/mprPoll.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprPoll.c

"${CC}" -c -Fo${PLATFORM}/obj/mprPrintf.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprPrintf.c

"${CC}" -c -Fo${PLATFORM}/obj/mprRomFile.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprRomFile.c

"${CC}" -c -Fo${PLATFORM}/obj/mprSelect.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprSelect.c

"${CC}" -c -Fo${PLATFORM}/obj/mprSignal.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprSignal.c

"${CC}" -c -Fo${PLATFORM}/obj/mprSocket.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprSocket.c

"${CC}" -c -Fo${PLATFORM}/obj/mprString.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprString.c

"${CC}" -c -Fo${PLATFORM}/obj/mprTest.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprTest.c

"${CC}" -c -Fo${PLATFORM}/obj/mprThread.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprThread.c

"${CC}" -c -Fo${PLATFORM}/obj/mprTime.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprTime.c

"${CC}" -c -Fo${PLATFORM}/obj/mprUnix.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprUnix.c

"${CC}" -c -Fo${PLATFORM}/obj/mprVxworks.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprVxworks.c

"${CC}" -c -Fo${PLATFORM}/obj/mprWait.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprWait.c

"${CC}" -c -Fo${PLATFORM}/obj/mprWide.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprWide.c

"${CC}" -c -Fo${PLATFORM}/obj/mprWin.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprWin.c

"${CC}" -c -Fo${PLATFORM}/obj/mprWince.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprWince.c

"${CC}" -c -Fo${PLATFORM}/obj/mprXml.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprXml.c

"${LD}" -dll -out:${PLATFORM}/bin/libmpr.dll -entry:_DllMainCRTStartup@12 -def:${PLATFORM}/bin/libmpr.def ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/dtoa.obj ${PLATFORM}/obj/mpr.obj ${PLATFORM}/obj/mprAsync.obj ${PLATFORM}/obj/mprAtomic.obj ${PLATFORM}/obj/mprBuf.obj ${PLATFORM}/obj/mprCache.obj ${PLATFORM}/obj/mprCmd.obj ${PLATFORM}/obj/mprCond.obj ${PLATFORM}/obj/mprCrypt.obj ${PLATFORM}/obj/mprDisk.obj ${PLATFORM}/obj/mprDispatcher.obj ${PLATFORM}/obj/mprEncode.obj ${PLATFORM}/obj/mprEpoll.obj ${PLATFORM}/obj/mprEvent.obj ${PLATFORM}/obj/mprFile.obj ${PLATFORM}/obj/mprFileSystem.obj ${PLATFORM}/obj/mprHash.obj ${PLATFORM}/obj/mprJSON.obj ${PLATFORM}/obj/mprKqueue.obj ${PLATFORM}/obj/mprList.obj ${PLATFORM}/obj/mprLock.obj ${PLATFORM}/obj/mprLog.obj ${PLATFORM}/obj/mprMem.obj ${PLATFORM}/obj/mprMime.obj ${PLATFORM}/obj/mprMixed.obj ${PLATFORM}/obj/mprModule.obj ${PLATFORM}/obj/mprPath.obj ${PLATFORM}/obj/mprPoll.obj ${PLATFORM}/obj/mprPrintf.obj ${PLATFORM}/obj/mprRomFile.obj ${PLATFORM}/obj/mprSelect.obj ${PLATFORM}/obj/mprSignal.obj ${PLATFORM}/obj/mprSocket.obj ${PLATFORM}/obj/mprString.obj ${PLATFORM}/obj/mprTest.obj ${PLATFORM}/obj/mprThread.obj ${PLATFORM}/obj/mprTime.obj ${PLATFORM}/obj/mprUnix.obj ${PLATFORM}/obj/mprVxworks.obj ${PLATFORM}/obj/mprWait.obj ${PLATFORM}/obj/mprWide.obj ${PLATFORM}/obj/mprWin.obj ${PLATFORM}/obj/mprWince.obj ${PLATFORM}/obj/mprXml.obj ${LIBS}

"${CC}" -c -Fo${PLATFORM}/obj/benchMpr.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/benchMpr.c

"${LD}" -out:${PLATFORM}/bin/benchMpr.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/benchMpr.obj ${LIBS} libmpr.lib

"${CC}" -c -Fo${PLATFORM}/obj/runProgram.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/runProgram.c

"${LD}" -out:${PLATFORM}/bin/runProgram.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/runProgram.obj ${LIBS}

"${CC}" -c -Fo${PLATFORM}/obj/testArgv.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testArgv.c

"${CC}" -c -Fo${PLATFORM}/obj/testBuf.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testBuf.c

"${CC}" -c -Fo${PLATFORM}/obj/testCmd.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testCmd.c

"${CC}" -c -Fo${PLATFORM}/obj/testCond.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testCond.c

"${CC}" -c -Fo${PLATFORM}/obj/testEvent.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testEvent.c

"${CC}" -c -Fo${PLATFORM}/obj/testFile.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testFile.c

"${CC}" -c -Fo${PLATFORM}/obj/testHash.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testHash.c

"${CC}" -c -Fo${PLATFORM}/obj/testList.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testList.c

"${CC}" -c -Fo${PLATFORM}/obj/testLock.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testLock.c

"${CC}" -c -Fo${PLATFORM}/obj/testMem.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testMem.c

"${CC}" -c -Fo${PLATFORM}/obj/testMpr.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testMpr.c

"${CC}" -c -Fo${PLATFORM}/obj/testPath.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testPath.c

"${CC}" -c -Fo${PLATFORM}/obj/testSocket.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testSocket.c

"${CC}" -c -Fo${PLATFORM}/obj/testSprintf.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testSprintf.c

"${CC}" -c -Fo${PLATFORM}/obj/testThread.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testThread.c

"${CC}" -c -Fo${PLATFORM}/obj/testTime.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testTime.c

"${CC}" -c -Fo${PLATFORM}/obj/testUnicode.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testUnicode.c

"${LD}" -out:${PLATFORM}/bin/testMpr.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/testArgv.obj ${PLATFORM}/obj/testBuf.obj ${PLATFORM}/obj/testCmd.obj ${PLATFORM}/obj/testCond.obj ${PLATFORM}/obj/testEvent.obj ${PLATFORM}/obj/testFile.obj ${PLATFORM}/obj/testHash.obj ${PLATFORM}/obj/testList.obj ${PLATFORM}/obj/testLock.obj ${PLATFORM}/obj/testMem.obj ${PLATFORM}/obj/testMpr.obj ${PLATFORM}/obj/testPath.obj ${PLATFORM}/obj/testSocket.obj ${PLATFORM}/obj/testSprintf.obj ${PLATFORM}/obj/testThread.obj ${PLATFORM}/obj/testTime.obj ${PLATFORM}/obj/testUnicode.obj ${LIBS} libmpr.lib

"${CC}" -c -Fo${PLATFORM}/obj/manager.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/manager.c

"${LD}" -out:${PLATFORM}/bin/manager.exe -entry:WinMainCRTStartup -subsystem:Windows ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/manager.obj ${LIBS} libmpr.lib

"${CC}" -c -Fo${PLATFORM}/obj/makerom.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/utils/makerom.c

"${LD}" -out:${PLATFORM}/bin/makerom.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/makerom.obj ${LIBS} libmpr.lib

"${CC}" -c -Fo${PLATFORM}/obj/charGen.obj -Fd${PLATFORM}/obj ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/utils/charGen.c

"${LD}" -out:${PLATFORM}/bin/chargen.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/charGen.obj ${LIBS} libmpr.lib

