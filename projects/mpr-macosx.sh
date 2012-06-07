#
#   mpr-macosx.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

ARCH="x64"
ARCH="$(shell uname -m | sed 's/i.86/x86/;s/x86_64/x64/')"
OS="macosx"
PROFILE="xcode"
CONFIG="${OS}-${ARCH}-${PROFILE}"
CC="/usr/bin/clang"
LD="/usr/bin/ld"
CFLAGS="-Wall -Wno-deprecated-declarations -O3 -Wno-unused-result -Wshorten-64-to-32"
DFLAGS=""
IFLAGS="-I${CONFIG}/inc"
LDFLAGS="-Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/"
LIBPATHS="-L${CONFIG}/bin"
LIBS="-lpthread -lm -ldl"

[ ! -x ${CONFIG}/inc ] && mkdir -p ${CONFIG}/inc ${CONFIG}/obj ${CONFIG}/lib ${CONFIG}/bin

[ ! -f ${CONFIG}/inc/bit.h ] && cp projects/mpr-${OS}-bit.h ${CONFIG}/inc/bit.h
if ! diff ${CONFIG}/inc/bit.h projects/mpr-${OS}-bit.h >/dev/null ; then
	cp projects/mpr-${OS}-bit.h ${CONFIG}/inc/bit.h
fi

rm -rf ${CONFIG}/inc/mpr.h
cp -r src/mpr.h ${CONFIG}/inc/mpr.h

${DFLAGS}${CC} -c -o ${CONFIG}/obj/dtoa.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/dtoa.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mpr.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mpr.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprAsync.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprAsync.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprAtomic.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprAtomic.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprBuf.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprBuf.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprCache.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprCache.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprCmd.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprCmd.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprCond.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprCond.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprCrypt.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprCrypt.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprDisk.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprDisk.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprDispatcher.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprDispatcher.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprEncode.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprEncode.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprEpoll.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprEpoll.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprEvent.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprEvent.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprFile.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprFile.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprFileSystem.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprFileSystem.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprHash.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprHash.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprJSON.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprJSON.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprKqueue.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprKqueue.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprList.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprList.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprLock.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprLock.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprLog.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprLog.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprMem.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprMem.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprMime.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprMime.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprMixed.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprMixed.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprModule.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprModule.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprPath.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprPath.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprPoll.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprPoll.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprPrintf.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprPrintf.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprRomFile.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprRomFile.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprSelect.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprSelect.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprSignal.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprSignal.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprSocket.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprSocket.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprString.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprString.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprTest.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprTest.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprThread.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprThread.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprTime.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprTime.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprUnix.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprUnix.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprVxworks.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprVxworks.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprWait.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprWait.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprWide.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprWide.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprWin.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprWin.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprWince.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprWince.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprXml.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprXml.c

${DFLAGS}${CC} -dynamiclib -o ${CONFIG}/bin/libmpr.dylib -arch x86_64 ${LDFLAGS} -compatibility_version 4.0.0 -current_version 4.0.0 ${LIBPATHS} -install_name @rpath/libmpr.dylib ${CONFIG}/obj/dtoa.o ${CONFIG}/obj/mpr.o ${CONFIG}/obj/mprAsync.o ${CONFIG}/obj/mprAtomic.o ${CONFIG}/obj/mprBuf.o ${CONFIG}/obj/mprCache.o ${CONFIG}/obj/mprCmd.o ${CONFIG}/obj/mprCond.o ${CONFIG}/obj/mprCrypt.o ${CONFIG}/obj/mprDisk.o ${CONFIG}/obj/mprDispatcher.o ${CONFIG}/obj/mprEncode.o ${CONFIG}/obj/mprEpoll.o ${CONFIG}/obj/mprEvent.o ${CONFIG}/obj/mprFile.o ${CONFIG}/obj/mprFileSystem.o ${CONFIG}/obj/mprHash.o ${CONFIG}/obj/mprJSON.o ${CONFIG}/obj/mprKqueue.o ${CONFIG}/obj/mprList.o ${CONFIG}/obj/mprLock.o ${CONFIG}/obj/mprLog.o ${CONFIG}/obj/mprMem.o ${CONFIG}/obj/mprMime.o ${CONFIG}/obj/mprMixed.o ${CONFIG}/obj/mprModule.o ${CONFIG}/obj/mprPath.o ${CONFIG}/obj/mprPoll.o ${CONFIG}/obj/mprPrintf.o ${CONFIG}/obj/mprRomFile.o ${CONFIG}/obj/mprSelect.o ${CONFIG}/obj/mprSignal.o ${CONFIG}/obj/mprSocket.o ${CONFIG}/obj/mprString.o ${CONFIG}/obj/mprTest.o ${CONFIG}/obj/mprThread.o ${CONFIG}/obj/mprTime.o ${CONFIG}/obj/mprUnix.o ${CONFIG}/obj/mprVxworks.o ${CONFIG}/obj/mprWait.o ${CONFIG}/obj/mprWide.o ${CONFIG}/obj/mprWin.o ${CONFIG}/obj/mprWince.o ${CONFIG}/obj/mprXml.o ${LIBS}

${DFLAGS}${CC} -c -o ${CONFIG}/obj/benchMpr.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/benchMpr.c

${DFLAGS}${CC} -o ${CONFIG}/bin/benchMpr -arch x86_64 ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/benchMpr.o ${LIBS} -lmpr

${DFLAGS}${CC} -c -o ${CONFIG}/obj/runProgram.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/runProgram.c

${DFLAGS}${CC} -o ${CONFIG}/bin/runProgram -arch x86_64 ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/runProgram.o ${LIBS}

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprMatrixssl.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprMatrixssl.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprOpenssl.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprOpenssl.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/mprSsl.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/mprSsl.c

${DFLAGS}${CC} -dynamiclib -o ${CONFIG}/bin/libmprssl.dylib -arch x86_64 ${LDFLAGS} -compatibility_version 4.0.0 -current_version 4.0.0 ${LIBPATHS} -install_name @rpath/libmprssl.dylib ${CONFIG}/obj/mprMatrixssl.o ${CONFIG}/obj/mprOpenssl.o ${CONFIG}/obj/mprSsl.o ${LIBS} -lmpr

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testArgv.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testArgv.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testBuf.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testBuf.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testCmd.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testCmd.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testCond.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testCond.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testEvent.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testEvent.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testFile.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testFile.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testHash.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testHash.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testList.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testList.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testLock.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testLock.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testMem.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testMem.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testMpr.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testMpr.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testPath.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testPath.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testSocket.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testSocket.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testSprintf.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testSprintf.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testThread.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testThread.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testTime.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testTime.c

${DFLAGS}${CC} -c -o ${CONFIG}/obj/testUnicode.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc test/testUnicode.c

${DFLAGS}${CC} -o ${CONFIG}/bin/testMpr -arch x86_64 ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/testArgv.o ${CONFIG}/obj/testBuf.o ${CONFIG}/obj/testCmd.o ${CONFIG}/obj/testCond.o ${CONFIG}/obj/testEvent.o ${CONFIG}/obj/testFile.o ${CONFIG}/obj/testHash.o ${CONFIG}/obj/testList.o ${CONFIG}/obj/testLock.o ${CONFIG}/obj/testMem.o ${CONFIG}/obj/testMpr.o ${CONFIG}/obj/testPath.o ${CONFIG}/obj/testSocket.o ${CONFIG}/obj/testSprintf.o ${CONFIG}/obj/testThread.o ${CONFIG}/obj/testTime.o ${CONFIG}/obj/testUnicode.o ${LIBS} -lmpr -lmprssl

${DFLAGS}${CC} -c -o ${CONFIG}/obj/manager.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/manager.c

${DFLAGS}${CC} -o ${CONFIG}/bin/manager -arch x86_64 ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/manager.o ${LIBS} -lmpr

${DFLAGS}${CC} -c -o ${CONFIG}/obj/makerom.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/utils/makerom.c

${DFLAGS}${CC} -o ${CONFIG}/bin/makerom -arch x86_64 ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/makerom.o ${LIBS} -lmpr

${DFLAGS}${CC} -c -o ${CONFIG}/obj/charGen.o -arch x86_64 ${CFLAGS} -I${CONFIG}/inc src/utils/charGen.c

${DFLAGS}${CC} -o ${CONFIG}/bin/chargen -arch x86_64 ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/charGen.o ${LIBS} -lmpr

