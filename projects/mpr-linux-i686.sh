#
#   linux-i686-debug.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

OS="linux"
CONFIG="${OS}-i686-debug"
CC="gcc"
LD="ld"
CFLAGS="-Wall -fPIC -g -Wno-unused-result -mtune=i686"
DFLAGS="-D_REENTRANT -DPIC -DBLD_DEBUG"
IFLAGS="-I${CONFIG}/inc"
LDFLAGS="-Wl,--enable-new-dtags -Wl,-rpath,\$ORIGIN/ -Wl,-rpath,\$ORIGIN/../lib -rdynamic -g"
LIBPATHS="-L${CONFIG}/lib"
LIBS="-lpthread -lm -ldl"

[ ! -x ${CONFIG}/inc ] && mkdir -p ${CONFIG}/inc ${CONFIG}/obj ${CONFIG}/lib ${CONFIG}/bin

[ ! -f ${CONFIG}/inc/bit.h ] && cp projects/mpr-${OS}-bit.h ${CONFIG}/inc/bit.h
if ! diff ${CONFIG}/inc/bit.h projects/mpr-${OS}-bit.h >/dev/null ; then
	cp projects/mpr-${OS}-bit.h ${CONFIG}/inc/bit.h
fi

rm -rf ${CONFIG}/inc/mpr.h
cp -r src/mpr.h ${CONFIG}/inc/mpr.h

${CC} -c -o ${CONFIG}/obj/dtoa.o ${CFLAGS} -I${CONFIG}/inc src/dtoa.c

${CC} -c -o ${CONFIG}/obj/mpr.o ${CFLAGS} -I${CONFIG}/inc src/mpr.c

${CC} -c -o ${CONFIG}/obj/mprAsync.o ${CFLAGS} -I${CONFIG}/inc src/mprAsync.c

${CC} -c -o ${CONFIG}/obj/mprAtomic.o ${CFLAGS} -I${CONFIG}/inc src/mprAtomic.c

${CC} -c -o ${CONFIG}/obj/mprBuf.o ${CFLAGS} -I${CONFIG}/inc src/mprBuf.c

${CC} -c -o ${CONFIG}/obj/mprCache.o ${CFLAGS} -I${CONFIG}/inc src/mprCache.c

${CC} -c -o ${CONFIG}/obj/mprCmd.o ${CFLAGS} -I${CONFIG}/inc src/mprCmd.c

${CC} -c -o ${CONFIG}/obj/mprCond.o ${CFLAGS} -I${CONFIG}/inc src/mprCond.c

${CC} -c -o ${CONFIG}/obj/mprCrypt.o ${CFLAGS} -I${CONFIG}/inc src/mprCrypt.c

${CC} -c -o ${CONFIG}/obj/mprDisk.o ${CFLAGS} -I${CONFIG}/inc src/mprDisk.c

${CC} -c -o ${CONFIG}/obj/mprDispatcher.o ${CFLAGS} -I${CONFIG}/inc src/mprDispatcher.c

${CC} -c -o ${CONFIG}/obj/mprEncode.o ${CFLAGS} -I${CONFIG}/inc src/mprEncode.c

${CC} -c -o ${CONFIG}/obj/mprEpoll.o ${CFLAGS} -I${CONFIG}/inc src/mprEpoll.c

${CC} -c -o ${CONFIG}/obj/mprEvent.o ${CFLAGS} -I${CONFIG}/inc src/mprEvent.c

${CC} -c -o ${CONFIG}/obj/mprFile.o ${CFLAGS} -I${CONFIG}/inc src/mprFile.c

${CC} -c -o ${CONFIG}/obj/mprFileSystem.o ${CFLAGS} -I${CONFIG}/inc src/mprFileSystem.c

${CC} -c -o ${CONFIG}/obj/mprHash.o ${CFLAGS} -I${CONFIG}/inc src/mprHash.c

${CC} -c -o ${CONFIG}/obj/mprJSON.o ${CFLAGS} -I${CONFIG}/inc src/mprJSON.c

${CC} -c -o ${CONFIG}/obj/mprKqueue.o ${CFLAGS} -I${CONFIG}/inc src/mprKqueue.c

${CC} -c -o ${CONFIG}/obj/mprList.o ${CFLAGS} -I${CONFIG}/inc src/mprList.c

${CC} -c -o ${CONFIG}/obj/mprLock.o ${CFLAGS} -I${CONFIG}/inc src/mprLock.c

${CC} -c -o ${CONFIG}/obj/mprLog.o ${CFLAGS} -I${CONFIG}/inc src/mprLog.c

${CC} -c -o ${CONFIG}/obj/mprMem.o ${CFLAGS} -I${CONFIG}/inc src/mprMem.c

${CC} -c -o ${CONFIG}/obj/mprMime.o ${CFLAGS} -I${CONFIG}/inc src/mprMime.c

${CC} -c -o ${CONFIG}/obj/mprMixed.o ${CFLAGS} -I${CONFIG}/inc src/mprMixed.c

${CC} -c -o ${CONFIG}/obj/mprModule.o ${CFLAGS} -I${CONFIG}/inc src/mprModule.c

${CC} -c -o ${CONFIG}/obj/mprPath.o ${CFLAGS} -I${CONFIG}/inc src/mprPath.c

${CC} -c -o ${CONFIG}/obj/mprPoll.o ${CFLAGS} -I${CONFIG}/inc src/mprPoll.c

${CC} -c -o ${CONFIG}/obj/mprPrintf.o ${CFLAGS} -I${CONFIG}/inc src/mprPrintf.c

${CC} -c -o ${CONFIG}/obj/mprRomFile.o ${CFLAGS} -I${CONFIG}/inc src/mprRomFile.c

${CC} -c -o ${CONFIG}/obj/mprSelect.o ${CFLAGS} -I${CONFIG}/inc src/mprSelect.c

${CC} -c -o ${CONFIG}/obj/mprSignal.o ${CFLAGS} -I${CONFIG}/inc src/mprSignal.c

${CC} -c -o ${CONFIG}/obj/mprSocket.o ${CFLAGS} -I${CONFIG}/inc src/mprSocket.c

${CC} -c -o ${CONFIG}/obj/mprString.o ${CFLAGS} -I${CONFIG}/inc src/mprString.c

${CC} -c -o ${CONFIG}/obj/mprTest.o ${CFLAGS} -I${CONFIG}/inc src/mprTest.c

${CC} -c -o ${CONFIG}/obj/mprThread.o ${CFLAGS} -I${CONFIG}/inc src/mprThread.c

${CC} -c -o ${CONFIG}/obj/mprTime.o ${CFLAGS} -I${CONFIG}/inc src/mprTime.c

${CC} -c -o ${CONFIG}/obj/mprUnix.o ${CFLAGS} -I${CONFIG}/inc src/mprUnix.c

${CC} -c -o ${CONFIG}/obj/mprVxworks.o ${CFLAGS} -I${CONFIG}/inc src/mprVxworks.c

${CC} -c -o ${CONFIG}/obj/mprWait.o ${CFLAGS} -I${CONFIG}/inc src/mprWait.c

${CC} -c -o ${CONFIG}/obj/mprWide.o ${CFLAGS} -I${CONFIG}/inc src/mprWide.c

${CC} -c -o ${CONFIG}/obj/mprWin.o ${CFLAGS} -I${CONFIG}/inc src/mprWin.c

${CC} -c -o ${CONFIG}/obj/mprWince.o ${CFLAGS} -I${CONFIG}/inc src/mprWince.c

${CC} -c -o ${CONFIG}/obj/mprXml.o ${CFLAGS} -I${CONFIG}/inc src/mprXml.c

${CC} -shared -o ${CONFIG}/lib/libmpr.so ${LIBPATHS} ${CONFIG}/obj/dtoa.o ${CONFIG}/obj/mpr.o ${CONFIG}/obj/mprAsync.o ${CONFIG}/obj/mprAtomic.o ${CONFIG}/obj/mprBuf.o ${CONFIG}/obj/mprCache.o ${CONFIG}/obj/mprCmd.o ${CONFIG}/obj/mprCond.o ${CONFIG}/obj/mprCrypt.o ${CONFIG}/obj/mprDisk.o ${CONFIG}/obj/mprDispatcher.o ${CONFIG}/obj/mprEncode.o ${CONFIG}/obj/mprEpoll.o ${CONFIG}/obj/mprEvent.o ${CONFIG}/obj/mprFile.o ${CONFIG}/obj/mprFileSystem.o ${CONFIG}/obj/mprHash.o ${CONFIG}/obj/mprJSON.o ${CONFIG}/obj/mprKqueue.o ${CONFIG}/obj/mprList.o ${CONFIG}/obj/mprLock.o ${CONFIG}/obj/mprLog.o ${CONFIG}/obj/mprMem.o ${CONFIG}/obj/mprMime.o ${CONFIG}/obj/mprMixed.o ${CONFIG}/obj/mprModule.o ${CONFIG}/obj/mprPath.o ${CONFIG}/obj/mprPoll.o ${CONFIG}/obj/mprPrintf.o ${CONFIG}/obj/mprRomFile.o ${CONFIG}/obj/mprSelect.o ${CONFIG}/obj/mprSignal.o ${CONFIG}/obj/mprSocket.o ${CONFIG}/obj/mprString.o ${CONFIG}/obj/mprTest.o ${CONFIG}/obj/mprThread.o ${CONFIG}/obj/mprTime.o ${CONFIG}/obj/mprUnix.o ${CONFIG}/obj/mprVxworks.o ${CONFIG}/obj/mprWait.o ${CONFIG}/obj/mprWide.o ${CONFIG}/obj/mprWin.o ${CONFIG}/obj/mprWince.o ${CONFIG}/obj/mprXml.o ${LIBS}

${CC} -c -o ${CONFIG}/obj/benchMpr.o ${CFLAGS} -I${CONFIG}/inc test/benchMpr.c

${CC} -o ${CONFIG}/bin/benchMpr ${LIBPATHS} ${CONFIG}/obj/benchMpr.o ${LIBS} -lmpr 

${CC} -c -o ${CONFIG}/obj/runProgram.o ${CFLAGS} -I${CONFIG}/inc test/runProgram.c

${CC} -o ${CONFIG}/bin/runProgram ${LIBPATHS} ${CONFIG}/obj/runProgram.o ${LIBS} 

${CC} -c -o ${CONFIG}/obj/mprMatrixssl.o ${CFLAGS} -I${CONFIG}/inc src/mprMatrixssl.c

${CC} -c -o ${CONFIG}/obj/mprOpenssl.o ${CFLAGS} -I${CONFIG}/inc src/mprOpenssl.c

${CC} -c -o ${CONFIG}/obj/mprSsl.o ${CFLAGS} -I${CONFIG}/inc src/mprSsl.c

${CC} -shared -o ${CONFIG}/lib/libmprssl.so ${LIBPATHS} ${CONFIG}/obj/mprMatrixssl.o ${CONFIG}/obj/mprOpenssl.o ${CONFIG}/obj/mprSsl.o ${LIBS} -lmpr

${CC} -c -o ${CONFIG}/obj/testArgv.o ${CFLAGS} -I${CONFIG}/inc test/testArgv.c

${CC} -c -o ${CONFIG}/obj/testBuf.o ${CFLAGS} -I${CONFIG}/inc test/testBuf.c

${CC} -c -o ${CONFIG}/obj/testCmd.o ${CFLAGS} -I${CONFIG}/inc test/testCmd.c

${CC} -c -o ${CONFIG}/obj/testCond.o ${CFLAGS} -I${CONFIG}/inc test/testCond.c

${CC} -c -o ${CONFIG}/obj/testEvent.o ${CFLAGS} -I${CONFIG}/inc test/testEvent.c

${CC} -c -o ${CONFIG}/obj/testFile.o ${CFLAGS} -I${CONFIG}/inc test/testFile.c

${CC} -c -o ${CONFIG}/obj/testHash.o ${CFLAGS} -I${CONFIG}/inc test/testHash.c

${CC} -c -o ${CONFIG}/obj/testList.o ${CFLAGS} -I${CONFIG}/inc test/testList.c

${CC} -c -o ${CONFIG}/obj/testLock.o ${CFLAGS} -I${CONFIG}/inc test/testLock.c

${CC} -c -o ${CONFIG}/obj/testMem.o ${CFLAGS} -I${CONFIG}/inc test/testMem.c

${CC} -c -o ${CONFIG}/obj/testMpr.o ${CFLAGS} -I${CONFIG}/inc test/testMpr.c

${CC} -c -o ${CONFIG}/obj/testPath.o ${CFLAGS} -I${CONFIG}/inc test/testPath.c

${CC} -c -o ${CONFIG}/obj/testSocket.o ${CFLAGS} -I${CONFIG}/inc test/testSocket.c

${CC} -c -o ${CONFIG}/obj/testSprintf.o ${CFLAGS} -I${CONFIG}/inc test/testSprintf.c

${CC} -c -o ${CONFIG}/obj/testThread.o ${CFLAGS} -I${CONFIG}/inc test/testThread.c

${CC} -c -o ${CONFIG}/obj/testTime.o ${CFLAGS} -I${CONFIG}/inc test/testTime.c

${CC} -c -o ${CONFIG}/obj/testUnicode.o ${CFLAGS} -I${CONFIG}/inc test/testUnicode.c

${CC} -o ${CONFIG}/bin/testMpr ${LIBPATHS} ${CONFIG}/obj/testArgv.o ${CONFIG}/obj/testBuf.o ${CONFIG}/obj/testCmd.o ${CONFIG}/obj/testCond.o ${CONFIG}/obj/testEvent.o ${CONFIG}/obj/testFile.o ${CONFIG}/obj/testHash.o ${CONFIG}/obj/testList.o ${CONFIG}/obj/testLock.o ${CONFIG}/obj/testMem.o ${CONFIG}/obj/testMpr.o ${CONFIG}/obj/testPath.o ${CONFIG}/obj/testSocket.o ${CONFIG}/obj/testSprintf.o ${CONFIG}/obj/testThread.o ${CONFIG}/obj/testTime.o ${CONFIG}/obj/testUnicode.o ${LIBS} -lmpr -lmprssl 

${CC} -c -o ${CONFIG}/obj/manager.o ${CFLAGS} -I${CONFIG}/inc src/manager.c

${CC} -o ${CONFIG}/bin/manager ${LIBPATHS} ${CONFIG}/obj/manager.o ${LIBS} -lmpr 

${CC} -c -o ${CONFIG}/obj/makerom.o ${CFLAGS} -I${CONFIG}/inc src/utils/makerom.c

${CC} -o ${CONFIG}/bin/makerom ${LIBPATHS} ${CONFIG}/obj/makerom.o ${LIBS} -lmpr 

${CC} -c -o ${CONFIG}/obj/charGen.o ${CFLAGS} -I${CONFIG}/inc src/utils/charGen.c

${CC} -o ${CONFIG}/bin/chargen ${LIBPATHS} ${CONFIG}/obj/charGen.o ${LIBS} -lmpr 

