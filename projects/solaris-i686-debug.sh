#
#   solaris-i686-debug.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

CONFIG="solaris-i686-debug"
CC="cc"
LD="ld"
CFLAGS="-Wall -fPIC -O3 -mcpu=i686 -fPIC -O3 -mcpu=i686"
DFLAGS="-D_REENTRANT -DCPU=${ARCH} -DPIC -DPIC"
IFLAGS="-Isolaris-i686-debug/inc -Isolaris-i686-debug/inc"
LDFLAGS=""
LIBPATHS="-L${CONFIG}/lib -L${CONFIG}/lib"
LIBS="-llxnet -lrt -lsocket -lpthread -lm -lpthread -lm"

[ ! -x ${CONFIG}/inc ] && mkdir -p ${CONFIG}/inc ${CONFIG}/obj ${CONFIG}/lib ${CONFIG}/bin
cp projects/buildConfig.${CONFIG} ${CONFIG}/inc/buildConfig.h

rm -rf solaris-i686-debug/inc/mpr.h
cp -r src/mpr.h solaris-i686-debug/inc/mpr.h

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/dtoa.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/dtoa.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mpr.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mpr.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprAsync.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprAsync.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprAtomic.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprAtomic.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprBuf.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprBuf.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprCache.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprCache.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprCmd.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprCmd.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprCond.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprCond.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprCrypt.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprCrypt.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprDisk.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprDisk.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprDispatcher.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprDispatcher.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprEncode.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprEncode.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprEpoll.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprEpoll.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprEvent.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprEvent.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprFile.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprFile.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprFileSystem.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprFileSystem.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprHash.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprHash.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprJSON.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprJSON.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprKqueue.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprKqueue.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprList.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprList.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprLock.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprLock.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprLog.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprLog.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprMem.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprMem.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprMime.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprMime.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprMixed.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprMixed.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprModule.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprModule.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprPath.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprPath.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprPoll.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprPoll.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprPrintf.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprPrintf.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprRomFile.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprRomFile.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprSelect.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprSelect.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprSignal.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprSignal.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprSocket.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprSocket.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprString.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprString.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprTest.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprTest.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprThread.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprThread.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprTime.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprTime.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprUnix.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprUnix.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprVxworks.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprVxworks.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprWait.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprWait.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprWide.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprWide.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprWin.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprWin.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprWince.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprWince.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/mprXml.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/mprXml.c

${LDFLAGS}${LDFLAGS}${CC} -shared -o ${CONFIG}/lib/libmpr.so ${LIBPATHS} ${CONFIG}/obj/dtoa.o ${CONFIG}/obj/mpr.o ${CONFIG}/obj/mprAsync.o ${CONFIG}/obj/mprAtomic.o ${CONFIG}/obj/mprBuf.o ${CONFIG}/obj/mprCache.o ${CONFIG}/obj/mprCmd.o ${CONFIG}/obj/mprCond.o ${CONFIG}/obj/mprCrypt.o ${CONFIG}/obj/mprDisk.o ${CONFIG}/obj/mprDispatcher.o ${CONFIG}/obj/mprEncode.o ${CONFIG}/obj/mprEpoll.o ${CONFIG}/obj/mprEvent.o ${CONFIG}/obj/mprFile.o ${CONFIG}/obj/mprFileSystem.o ${CONFIG}/obj/mprHash.o ${CONFIG}/obj/mprJSON.o ${CONFIG}/obj/mprKqueue.o ${CONFIG}/obj/mprList.o ${CONFIG}/obj/mprLock.o ${CONFIG}/obj/mprLog.o ${CONFIG}/obj/mprMem.o ${CONFIG}/obj/mprMime.o ${CONFIG}/obj/mprMixed.o ${CONFIG}/obj/mprModule.o ${CONFIG}/obj/mprPath.o ${CONFIG}/obj/mprPoll.o ${CONFIG}/obj/mprPrintf.o ${CONFIG}/obj/mprRomFile.o ${CONFIG}/obj/mprSelect.o ${CONFIG}/obj/mprSignal.o ${CONFIG}/obj/mprSocket.o ${CONFIG}/obj/mprString.o ${CONFIG}/obj/mprTest.o ${CONFIG}/obj/mprThread.o ${CONFIG}/obj/mprTime.o ${CONFIG}/obj/mprUnix.o ${CONFIG}/obj/mprVxworks.o ${CONFIG}/obj/mprWait.o ${CONFIG}/obj/mprWide.o ${CONFIG}/obj/mprWin.o ${CONFIG}/obj/mprWince.o ${CONFIG}/obj/mprXml.o ${LIBS}

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/benchMpr.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/benchMpr.c

${LDFLAGS}${LDFLAGS}${CC} -o ${CONFIG}/bin/benchMpr ${LIBPATHS} ${CONFIG}/obj/benchMpr.o ${LIBS} -lmpr 

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/runProgram.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/runProgram.c

${LDFLAGS}${LDFLAGS}${CC} -o ${CONFIG}/bin/runProgram ${LIBPATHS} ${CONFIG}/obj/runProgram.o ${LIBS} 

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testArgv.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testArgv.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testBuf.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testBuf.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testCmd.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testCmd.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testCond.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testCond.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testEvent.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testEvent.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testFile.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testFile.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testHash.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testHash.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testList.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testList.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testLock.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testLock.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testMem.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testMem.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testMpr.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testMpr.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testPath.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testPath.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testSocket.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testSocket.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testSprintf.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testSprintf.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testThread.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testThread.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testTime.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testTime.c

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/testUnicode.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc test/testUnicode.c

${LDFLAGS}${LDFLAGS}${CC} -o ${CONFIG}/bin/testMpr ${LIBPATHS} ${CONFIG}/obj/testArgv.o ${CONFIG}/obj/testBuf.o ${CONFIG}/obj/testCmd.o ${CONFIG}/obj/testCond.o ${CONFIG}/obj/testEvent.o ${CONFIG}/obj/testFile.o ${CONFIG}/obj/testHash.o ${CONFIG}/obj/testList.o ${CONFIG}/obj/testLock.o ${CONFIG}/obj/testMem.o ${CONFIG}/obj/testMpr.o ${CONFIG}/obj/testPath.o ${CONFIG}/obj/testSocket.o ${CONFIG}/obj/testSprintf.o ${CONFIG}/obj/testThread.o ${CONFIG}/obj/testTime.o ${CONFIG}/obj/testUnicode.o ${LIBS} -lmpr 

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/manager.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/manager.c

${LDFLAGS}${LDFLAGS}${CC} -o ${CONFIG}/bin/manager ${LIBPATHS} ${CONFIG}/obj/manager.o ${LIBS} -lmpr 

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/makerom.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/utils/makerom.c

${LDFLAGS}${LDFLAGS}${CC} -o ${CONFIG}/bin/makerom ${LIBPATHS} ${CONFIG}/obj/makerom.o ${LIBS} -lmpr 

${LDFLAGS}${LDFLAGS}${CC} -c -o ${CONFIG}/obj/charGen.o ${CFLAGS} -D_REENTRANT -DCPU=i686 -DPIC -DPIC -I${CONFIG}/inc -I${CONFIG}/inc src/utils/charGen.c

${LDFLAGS}${LDFLAGS}${CC} -o ${CONFIG}/bin/chargen ${LIBPATHS} ${CONFIG}/obj/charGen.o ${LIBS} -lmpr 

