#
#   mpr-win.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

export PATH="$(SDK)/Bin:$(VS)/VC/Bin:$(VS)/Common7/IDE:$(VS)/Common7/Tools:$(VS)/SDK/v3.5/bin:$(VS)/VC/VCPackages;$(PATH)"
export INCLUDE="$(INCLUDE);$(SDK)/INCLUDE:$(VS)/VC/INCLUDE"
export LIB="$(LIB);$(SDK)/lib:$(VS)/VC/lib"

ARCH="x86"
ARCH="$(shell uname -m | sed 's/i.86/x86/;s/x86_64/x64/')"
OS="win"
PROFILE="debug"
CONFIG="${OS}-${ARCH}-${PROFILE}"
CC="cl.exe"
LD="link.exe"
CFLAGS="-nologo -GR- -W3 -Zi -Od -MDd"
DFLAGS="-D_REENTRANT -D_MT -DBIT_DEBUG"
IFLAGS="-I${CONFIG}/inc"
LDFLAGS="-nologo -nodefaultlib -incremental:no -debug -machine:x86"
LIBPATHS="-libpath:${CONFIG}/bin"
LIBS="ws2_32.lib advapi32.lib user32.lib kernel32.lib oldnames.lib msvcrt.lib shell32.lib"

[ ! -x ${CONFIG}/inc ] && mkdir -p ${CONFIG}/inc ${CONFIG}/obj ${CONFIG}/lib ${CONFIG}/bin

[ ! -f ${CONFIG}/inc/bit.h ] && cp projects/mpr-${OS}-bit.h ${CONFIG}/inc/bit.h
if ! diff ${CONFIG}/inc/bit.h projects/mpr-${OS}-bit.h >/dev/null ; then
	cp projects/mpr-${OS}-bit.h ${CONFIG}/inc/bit.h
fi

rm -rf ${CONFIG}/inc/mpr.h
cp -r src/mpr.h ${CONFIG}/inc/mpr.h

"${CC}" -c -Fo${CONFIG}/obj/dtoa.obj -Fd${CONFIG}/obj/dtoa.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/dtoa.c

"${CC}" -c -Fo${CONFIG}/obj/mpr.obj -Fd${CONFIG}/obj/mpr.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mpr.c

"${CC}" -c -Fo${CONFIG}/obj/mprAsync.obj -Fd${CONFIG}/obj/mprAsync.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprAsync.c

"${CC}" -c -Fo${CONFIG}/obj/mprAtomic.obj -Fd${CONFIG}/obj/mprAtomic.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprAtomic.c

"${CC}" -c -Fo${CONFIG}/obj/mprBuf.obj -Fd${CONFIG}/obj/mprBuf.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprBuf.c

"${CC}" -c -Fo${CONFIG}/obj/mprCache.obj -Fd${CONFIG}/obj/mprCache.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprCache.c

"${CC}" -c -Fo${CONFIG}/obj/mprCmd.obj -Fd${CONFIG}/obj/mprCmd.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprCmd.c

"${CC}" -c -Fo${CONFIG}/obj/mprCond.obj -Fd${CONFIG}/obj/mprCond.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprCond.c

"${CC}" -c -Fo${CONFIG}/obj/mprCrypt.obj -Fd${CONFIG}/obj/mprCrypt.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprCrypt.c

"${CC}" -c -Fo${CONFIG}/obj/mprDisk.obj -Fd${CONFIG}/obj/mprDisk.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprDisk.c

"${CC}" -c -Fo${CONFIG}/obj/mprDispatcher.obj -Fd${CONFIG}/obj/mprDispatcher.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprDispatcher.c

"${CC}" -c -Fo${CONFIG}/obj/mprEncode.obj -Fd${CONFIG}/obj/mprEncode.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprEncode.c

"${CC}" -c -Fo${CONFIG}/obj/mprEpoll.obj -Fd${CONFIG}/obj/mprEpoll.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprEpoll.c

"${CC}" -c -Fo${CONFIG}/obj/mprEvent.obj -Fd${CONFIG}/obj/mprEvent.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprEvent.c

"${CC}" -c -Fo${CONFIG}/obj/mprFile.obj -Fd${CONFIG}/obj/mprFile.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprFile.c

"${CC}" -c -Fo${CONFIG}/obj/mprFileSystem.obj -Fd${CONFIG}/obj/mprFileSystem.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprFileSystem.c

"${CC}" -c -Fo${CONFIG}/obj/mprHash.obj -Fd${CONFIG}/obj/mprHash.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprHash.c

"${CC}" -c -Fo${CONFIG}/obj/mprJSON.obj -Fd${CONFIG}/obj/mprJSON.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprJSON.c

"${CC}" -c -Fo${CONFIG}/obj/mprKqueue.obj -Fd${CONFIG}/obj/mprKqueue.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprKqueue.c

"${CC}" -c -Fo${CONFIG}/obj/mprList.obj -Fd${CONFIG}/obj/mprList.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprList.c

"${CC}" -c -Fo${CONFIG}/obj/mprLock.obj -Fd${CONFIG}/obj/mprLock.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprLock.c

"${CC}" -c -Fo${CONFIG}/obj/mprLog.obj -Fd${CONFIG}/obj/mprLog.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprLog.c

"${CC}" -c -Fo${CONFIG}/obj/mprMem.obj -Fd${CONFIG}/obj/mprMem.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprMem.c

"${CC}" -c -Fo${CONFIG}/obj/mprMime.obj -Fd${CONFIG}/obj/mprMime.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprMime.c

"${CC}" -c -Fo${CONFIG}/obj/mprMixed.obj -Fd${CONFIG}/obj/mprMixed.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprMixed.c

"${CC}" -c -Fo${CONFIG}/obj/mprModule.obj -Fd${CONFIG}/obj/mprModule.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprModule.c

"${CC}" -c -Fo${CONFIG}/obj/mprPath.obj -Fd${CONFIG}/obj/mprPath.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprPath.c

"${CC}" -c -Fo${CONFIG}/obj/mprPoll.obj -Fd${CONFIG}/obj/mprPoll.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprPoll.c

"${CC}" -c -Fo${CONFIG}/obj/mprPrintf.obj -Fd${CONFIG}/obj/mprPrintf.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprPrintf.c

"${CC}" -c -Fo${CONFIG}/obj/mprRomFile.obj -Fd${CONFIG}/obj/mprRomFile.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprRomFile.c

"${CC}" -c -Fo${CONFIG}/obj/mprSelect.obj -Fd${CONFIG}/obj/mprSelect.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprSelect.c

"${CC}" -c -Fo${CONFIG}/obj/mprSignal.obj -Fd${CONFIG}/obj/mprSignal.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprSignal.c

"${CC}" -c -Fo${CONFIG}/obj/mprSocket.obj -Fd${CONFIG}/obj/mprSocket.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprSocket.c

"${CC}" -c -Fo${CONFIG}/obj/mprString.obj -Fd${CONFIG}/obj/mprString.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprString.c

"${CC}" -c -Fo${CONFIG}/obj/mprTest.obj -Fd${CONFIG}/obj/mprTest.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprTest.c

"${CC}" -c -Fo${CONFIG}/obj/mprThread.obj -Fd${CONFIG}/obj/mprThread.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprThread.c

"${CC}" -c -Fo${CONFIG}/obj/mprTime.obj -Fd${CONFIG}/obj/mprTime.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprTime.c

"${CC}" -c -Fo${CONFIG}/obj/mprUnix.obj -Fd${CONFIG}/obj/mprUnix.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprUnix.c

"${CC}" -c -Fo${CONFIG}/obj/mprVxworks.obj -Fd${CONFIG}/obj/mprVxworks.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprVxworks.c

"${CC}" -c -Fo${CONFIG}/obj/mprWait.obj -Fd${CONFIG}/obj/mprWait.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprWait.c

"${CC}" -c -Fo${CONFIG}/obj/mprWide.obj -Fd${CONFIG}/obj/mprWide.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprWide.c

"${CC}" -c -Fo${CONFIG}/obj/mprWin.obj -Fd${CONFIG}/obj/mprWin.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprWin.c

"${CC}" -c -Fo${CONFIG}/obj/mprWince.obj -Fd${CONFIG}/obj/mprWince.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprWince.c

"${CC}" -c -Fo${CONFIG}/obj/mprXml.obj -Fd${CONFIG}/obj/mprXml.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprXml.c

"${LD}" -dll -out:${CONFIG}/bin/libmpr.dll -entry:_DllMainCRTStartup@12 -def:${CONFIG}/bin/libmpr.def ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/dtoa.obj ${CONFIG}/obj/mpr.obj ${CONFIG}/obj/mprAsync.obj ${CONFIG}/obj/mprAtomic.obj ${CONFIG}/obj/mprBuf.obj ${CONFIG}/obj/mprCache.obj ${CONFIG}/obj/mprCmd.obj ${CONFIG}/obj/mprCond.obj ${CONFIG}/obj/mprCrypt.obj ${CONFIG}/obj/mprDisk.obj ${CONFIG}/obj/mprDispatcher.obj ${CONFIG}/obj/mprEncode.obj ${CONFIG}/obj/mprEpoll.obj ${CONFIG}/obj/mprEvent.obj ${CONFIG}/obj/mprFile.obj ${CONFIG}/obj/mprFileSystem.obj ${CONFIG}/obj/mprHash.obj ${CONFIG}/obj/mprJSON.obj ${CONFIG}/obj/mprKqueue.obj ${CONFIG}/obj/mprList.obj ${CONFIG}/obj/mprLock.obj ${CONFIG}/obj/mprLog.obj ${CONFIG}/obj/mprMem.obj ${CONFIG}/obj/mprMime.obj ${CONFIG}/obj/mprMixed.obj ${CONFIG}/obj/mprModule.obj ${CONFIG}/obj/mprPath.obj ${CONFIG}/obj/mprPoll.obj ${CONFIG}/obj/mprPrintf.obj ${CONFIG}/obj/mprRomFile.obj ${CONFIG}/obj/mprSelect.obj ${CONFIG}/obj/mprSignal.obj ${CONFIG}/obj/mprSocket.obj ${CONFIG}/obj/mprString.obj ${CONFIG}/obj/mprTest.obj ${CONFIG}/obj/mprThread.obj ${CONFIG}/obj/mprTime.obj ${CONFIG}/obj/mprUnix.obj ${CONFIG}/obj/mprVxworks.obj ${CONFIG}/obj/mprWait.obj ${CONFIG}/obj/mprWide.obj ${CONFIG}/obj/mprWin.obj ${CONFIG}/obj/mprWince.obj ${CONFIG}/obj/mprXml.obj ${LIBS}

"${CC}" -c -Fo${CONFIG}/obj/benchMpr.obj -Fd${CONFIG}/obj/benchMpr.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/benchMpr.c

"${LD}" -out:${CONFIG}/bin/benchMpr.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/benchMpr.obj ${LIBS} libmpr.lib

"${CC}" -c -Fo${CONFIG}/obj/runProgram.obj -Fd${CONFIG}/obj/runProgram.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/runProgram.c

"${LD}" -out:${CONFIG}/bin/runProgram.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/runProgram.obj ${LIBS}

"${CC}" -c -Fo${CONFIG}/obj/mprMatrixssl.obj -Fd${CONFIG}/obj/mprMatrixssl.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprMatrixssl.c

"${CC}" -c -Fo${CONFIG}/obj/mprOpenssl.obj -Fd${CONFIG}/obj/mprOpenssl.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprOpenssl.c

"${CC}" -c -Fo${CONFIG}/obj/mprSsl.obj -Fd${CONFIG}/obj/mprSsl.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/mprSsl.c

"${LD}" -dll -out:${CONFIG}/bin/libmprssl.dll -entry:_DllMainCRTStartup@12 -def:${CONFIG}/bin/libmprssl.def ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/mprMatrixssl.obj ${CONFIG}/obj/mprOpenssl.obj ${CONFIG}/obj/mprSsl.obj ${LIBS} libmpr.lib

"${CC}" -c -Fo${CONFIG}/obj/testArgv.obj -Fd${CONFIG}/obj/testArgv.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testArgv.c

"${CC}" -c -Fo${CONFIG}/obj/testBuf.obj -Fd${CONFIG}/obj/testBuf.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testBuf.c

"${CC}" -c -Fo${CONFIG}/obj/testCmd.obj -Fd${CONFIG}/obj/testCmd.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testCmd.c

"${CC}" -c -Fo${CONFIG}/obj/testCond.obj -Fd${CONFIG}/obj/testCond.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testCond.c

"${CC}" -c -Fo${CONFIG}/obj/testEvent.obj -Fd${CONFIG}/obj/testEvent.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testEvent.c

"${CC}" -c -Fo${CONFIG}/obj/testFile.obj -Fd${CONFIG}/obj/testFile.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testFile.c

"${CC}" -c -Fo${CONFIG}/obj/testHash.obj -Fd${CONFIG}/obj/testHash.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testHash.c

"${CC}" -c -Fo${CONFIG}/obj/testList.obj -Fd${CONFIG}/obj/testList.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testList.c

"${CC}" -c -Fo${CONFIG}/obj/testLock.obj -Fd${CONFIG}/obj/testLock.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testLock.c

"${CC}" -c -Fo${CONFIG}/obj/testMem.obj -Fd${CONFIG}/obj/testMem.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testMem.c

"${CC}" -c -Fo${CONFIG}/obj/testMpr.obj -Fd${CONFIG}/obj/testMpr.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testMpr.c

"${CC}" -c -Fo${CONFIG}/obj/testPath.obj -Fd${CONFIG}/obj/testPath.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testPath.c

"${CC}" -c -Fo${CONFIG}/obj/testSocket.obj -Fd${CONFIG}/obj/testSocket.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testSocket.c

"${CC}" -c -Fo${CONFIG}/obj/testSprintf.obj -Fd${CONFIG}/obj/testSprintf.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testSprintf.c

"${CC}" -c -Fo${CONFIG}/obj/testThread.obj -Fd${CONFIG}/obj/testThread.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testThread.c

"${CC}" -c -Fo${CONFIG}/obj/testTime.obj -Fd${CONFIG}/obj/testTime.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testTime.c

"${CC}" -c -Fo${CONFIG}/obj/testUnicode.obj -Fd${CONFIG}/obj/testUnicode.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc test/testUnicode.c

"${LD}" -out:${CONFIG}/bin/testMpr.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/testArgv.obj ${CONFIG}/obj/testBuf.obj ${CONFIG}/obj/testCmd.obj ${CONFIG}/obj/testCond.obj ${CONFIG}/obj/testEvent.obj ${CONFIG}/obj/testFile.obj ${CONFIG}/obj/testHash.obj ${CONFIG}/obj/testList.obj ${CONFIG}/obj/testLock.obj ${CONFIG}/obj/testMem.obj ${CONFIG}/obj/testMpr.obj ${CONFIG}/obj/testPath.obj ${CONFIG}/obj/testSocket.obj ${CONFIG}/obj/testSprintf.obj ${CONFIG}/obj/testThread.obj ${CONFIG}/obj/testTime.obj ${CONFIG}/obj/testUnicode.obj ${LIBS} libmpr.lib libmprssl.lib

"${CC}" -c -Fo${CONFIG}/obj/manager.obj -Fd${CONFIG}/obj/manager.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/manager.c

"${LD}" -out:${CONFIG}/bin/manager.exe -entry:WinMainCRTStartup -subsystem:Windows ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/manager.obj ${LIBS} libmpr.lib

"${CC}" -c -Fo${CONFIG}/obj/makerom.obj -Fd${CONFIG}/obj/makerom.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/utils/makerom.c

"${LD}" -out:${CONFIG}/bin/makerom.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/makerom.obj ${LIBS} libmpr.lib

"${CC}" -c -Fo${CONFIG}/obj/charGen.obj -Fd${CONFIG}/obj/charGen.pdb ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc src/utils/charGen.c

"${LD}" -out:${CONFIG}/bin/chargen.exe -entry:mainCRTStartup -subsystem:console ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/charGen.obj ${LIBS} libmpr.lib

