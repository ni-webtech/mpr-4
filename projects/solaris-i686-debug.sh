#
#   solaris-i686-debug.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

CONFIG="solaris-i686-debug"
CC="cc"
LD="ld"
CFLAGS="-Wall -fPIC -g -mcpu=i686"
DFLAGS="-D_REENTRANT -DCPU=${ARCH} -DPIC"
IFLAGS="-Isolaris-i686-debug/inc"
LDFLAGS="-g"
LIBPATHS="-L${CONFIG}/lib"
LIBS="-llxnet -lrt -lsocket -lpthread -lm"

[ ! -x ${CONFIG}/inc ] && mkdir -p ${CONFIG}/inc ${CONFIG}/obj ${CONFIG}/lib ${CONFIG}/bin
cp projects/buildConfig.${CONFIG} ${CONFIG}/inc/buildConfig.h

rm -rf solaris-i686-debug/inc/mpr.h
cp -r src/mpr.h solaris-i686-debug/inc/mpr.h

${CC} -c -o ${CONFIG}/obj/dtoa.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/dtoa.c

${CC} -c -o ${CONFIG}/obj/mpr.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mpr.c

${CC} -c -o ${CONFIG}/obj/mprAsync.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprAsync.c

${CC} -c -o ${CONFIG}/obj/mprAtomic.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprAtomic.c

${CC} -c -o ${CONFIG}/obj/mprBuf.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprBuf.c

${CC} -c -o ${CONFIG}/obj/mprCache.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprCache.c

${CC} -c -o ${CONFIG}/obj/mprCmd.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprCmd.c

${CC} -c -o ${CONFIG}/obj/mprCond.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprCond.c

${CC} -c -o ${CONFIG}/obj/mprCrypt.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprCrypt.c

${CC} -c -o ${CONFIG}/obj/mprDisk.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprDisk.c

${CC} -c -o ${CONFIG}/obj/mprDispatcher.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprDispatcher.c

${CC} -c -o ${CONFIG}/obj/mprEncode.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprEncode.c

${CC} -c -o ${CONFIG}/obj/mprEpoll.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprEpoll.c

${CC} -c -o ${CONFIG}/obj/mprEvent.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprEvent.c

${CC} -c -o ${CONFIG}/obj/mprFile.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprFile.c

${CC} -c -o ${CONFIG}/obj/mprFileSystem.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprFileSystem.c

${CC} -c -o ${CONFIG}/obj/mprHash.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprHash.c

${CC} -c -o ${CONFIG}/obj/mprJSON.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprJSON.c

${CC} -c -o ${CONFIG}/obj/mprKqueue.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprKqueue.c

${CC} -c -o ${CONFIG}/obj/mprList.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprList.c

${CC} -c -o ${CONFIG}/obj/mprLock.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprLock.c

${CC} -c -o ${CONFIG}/obj/mprLog.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprLog.c

${CC} -c -o ${CONFIG}/obj/mprMem.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprMem.c

${CC} -c -o ${CONFIG}/obj/mprMime.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprMime.c

${CC} -c -o ${CONFIG}/obj/mprMixed.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprMixed.c

${CC} -c -o ${CONFIG}/obj/mprModule.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprModule.c

${CC} -c -o ${CONFIG}/obj/mprPath.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprPath.c

${CC} -c -o ${CONFIG}/obj/mprPoll.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprPoll.c

${CC} -c -o ${CONFIG}/obj/mprPrintf.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprPrintf.c

${CC} -c -o ${CONFIG}/obj/mprRomFile.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprRomFile.c

${CC} -c -o ${CONFIG}/obj/mprSelect.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprSelect.c

${CC} -c -o ${CONFIG}/obj/mprSignal.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprSignal.c

${CC} -c -o ${CONFIG}/obj/mprSocket.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprSocket.c

${CC} -c -o ${CONFIG}/obj/mprString.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprString.c

${CC} -c -o ${CONFIG}/obj/mprTest.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprTest.c

${CC} -c -o ${CONFIG}/obj/mprThread.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprThread.c

${CC} -c -o ${CONFIG}/obj/mprTime.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprTime.c

${CC} -c -o ${CONFIG}/obj/mprUnix.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprUnix.c

${CC} -c -o ${CONFIG}/obj/mprVxworks.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprVxworks.c

${CC} -c -o ${CONFIG}/obj/mprWait.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprWait.c

${CC} -c -o ${CONFIG}/obj/mprWide.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprWide.c

${CC} -c -o ${CONFIG}/obj/mprWin.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprWin.c

${CC} -c -o ${CONFIG}/obj/mprWince.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprWince.c

${CC} -c -o ${CONFIG}/obj/mprXml.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/mprXml.c

${CC} -shared -o ${CONFIG}/lib/libmpr.so ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/dtoa.o ${CONFIG}/obj/mpr.o ${CONFIG}/obj/mprAsync.o ${CONFIG}/obj/mprAtomic.o ${CONFIG}/obj/mprBuf.o ${CONFIG}/obj/mprCache.o ${CONFIG}/obj/mprCmd.o ${CONFIG}/obj/mprCond.o ${CONFIG}/obj/mprCrypt.o ${CONFIG}/obj/mprDisk.o ${CONFIG}/obj/mprDispatcher.o ${CONFIG}/obj/mprEncode.o ${CONFIG}/obj/mprEpoll.o ${CONFIG}/obj/mprEvent.o ${CONFIG}/obj/mprFile.o ${CONFIG}/obj/mprFileSystem.o ${CONFIG}/obj/mprHash.o ${CONFIG}/obj/mprJSON.o ${CONFIG}/obj/mprKqueue.o ${CONFIG}/obj/mprList.o ${CONFIG}/obj/mprLock.o ${CONFIG}/obj/mprLog.o ${CONFIG}/obj/mprMem.o ${CONFIG}/obj/mprMime.o ${CONFIG}/obj/mprMixed.o ${CONFIG}/obj/mprModule.o ${CONFIG}/obj/mprPath.o ${CONFIG}/obj/mprPoll.o ${CONFIG}/obj/mprPrintf.o ${CONFIG}/obj/mprRomFile.o ${CONFIG}/obj/mprSelect.o ${CONFIG}/obj/mprSignal.o ${CONFIG}/obj/mprSocket.o ${CONFIG}/obj/mprString.o ${CONFIG}/obj/mprTest.o ${CONFIG}/obj/mprThread.o ${CONFIG}/obj/mprTime.o ${CONFIG}/obj/mprUnix.o ${CONFIG}/obj/mprVxworks.o ${CONFIG}/obj/mprWait.o ${CONFIG}/obj/mprWide.o ${CONFIG}/obj/mprWin.o ${CONFIG}/obj/mprWince.o ${CONFIG}/obj/mprXml.o ${LIBS}

${CC} -c -o ${CONFIG}/obj/benchMpr.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/benchMpr.c

${CC} -o ${CONFIG}/bin/benchMpr ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/benchMpr.o ${LIBS} -lmpr ${LDFLAGS}

${CC} -c -o ${CONFIG}/obj/runProgram.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/runProgram.c

${CC} -o ${CONFIG}/bin/runProgram ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/runProgram.o ${LIBS} ${LDFLAGS}

${CC} -c -o ${CONFIG}/obj/testArgv.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testArgv.c

${CC} -c -o ${CONFIG}/obj/testBuf.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testBuf.c

${CC} -c -o ${CONFIG}/obj/testCmd.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testCmd.c

${CC} -c -o ${CONFIG}/obj/testCond.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testCond.c

${CC} -c -o ${CONFIG}/obj/testEvent.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testEvent.c

${CC} -c -o ${CONFIG}/obj/testFile.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testFile.c

${CC} -c -o ${CONFIG}/obj/testHash.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testHash.c

${CC} -c -o ${CONFIG}/obj/testList.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testList.c

${CC} -c -o ${CONFIG}/obj/testLock.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testLock.c

${CC} -c -o ${CONFIG}/obj/testMem.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testMem.c

${CC} -c -o ${CONFIG}/obj/testMpr.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testMpr.c

${CC} -c -o ${CONFIG}/obj/testPath.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testPath.c

${CC} -c -o ${CONFIG}/obj/testSocket.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testSocket.c

${CC} -c -o ${CONFIG}/obj/testSprintf.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testSprintf.c

${CC} -c -o ${CONFIG}/obj/testThread.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testThread.c

${CC} -c -o ${CONFIG}/obj/testTime.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testTime.c

${CC} -c -o ${CONFIG}/obj/testUnicode.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc test/testUnicode.c

${CC} -o ${CONFIG}/bin/testMpr ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/testArgv.o ${CONFIG}/obj/testBuf.o ${CONFIG}/obj/testCmd.o ${CONFIG}/obj/testCond.o ${CONFIG}/obj/testEvent.o ${CONFIG}/obj/testFile.o ${CONFIG}/obj/testHash.o ${CONFIG}/obj/testList.o ${CONFIG}/obj/testLock.o ${CONFIG}/obj/testMem.o ${CONFIG}/obj/testMpr.o ${CONFIG}/obj/testPath.o ${CONFIG}/obj/testSocket.o ${CONFIG}/obj/testSprintf.o ${CONFIG}/obj/testThread.o ${CONFIG}/obj/testTime.o ${CONFIG}/obj/testUnicode.o ${LIBS} -lmpr ${LDFLAGS}

${CC} -c -o ${CONFIG}/obj/manager.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/manager.c

${CC} -o ${CONFIG}/bin/manager ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/manager.o ${LIBS} -lmpr ${LDFLAGS}

${CC} -c -o ${CONFIG}/obj/makerom.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/utils/makerom.c

${CC} -o ${CONFIG}/bin/makerom ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/makerom.o ${LIBS} -lmpr ${LDFLAGS}

${CC} -c -o ${CONFIG}/obj/charGen.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 -D_REENTRANT -DCPU=i686 -DPIC -I${CONFIG}/inc src/utils/charGen.c

${CC} -o ${CONFIG}/bin/chargen ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/charGen.o ${LIBS} -lmpr ${LDFLAGS}

