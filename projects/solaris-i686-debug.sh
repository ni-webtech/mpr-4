#
#   solaris-i686-debug.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

PLATFORM="solaris-i686-debug"
CC="cc"
LD="ld"
CFLAGS="-Wall -fPIC -g -mcpu=i686"
DFLAGS="-D_REENTRANT -DCPU=i686 -DPIC"
IFLAGS="-Isolaris-i686-debug/inc"
LDFLAGS="-g"
LIBPATHS="-L${PLATFORM}/lib"
LIBS="-llxnet -lrt -lsocket -lpthread -lm"

[ ! -x ${PLATFORM}/inc ] && mkdir -p ${PLATFORM}/inc ${PLATFORM}/obj ${PLATFORM}/lib ${PLATFORM}/bin
cp projects/buildConfig.${PLATFORM} ${PLATFORM}/inc/buildConfig.h

rm -rf solaris-i686-debug/inc/mpr.h
cp -r src/mpr.h solaris-i686-debug/inc/mpr.h

${CC} -c -o ${PLATFORM}/obj/dtoa.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/dtoa.c

${CC} -c -o ${PLATFORM}/obj/mpr.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mpr.c

${CC} -c -o ${PLATFORM}/obj/mprAsync.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprAsync.c

${CC} -c -o ${PLATFORM}/obj/mprAtomic.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprAtomic.c

${CC} -c -o ${PLATFORM}/obj/mprBuf.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprBuf.c

${CC} -c -o ${PLATFORM}/obj/mprCache.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprCache.c

${CC} -c -o ${PLATFORM}/obj/mprCmd.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprCmd.c

${CC} -c -o ${PLATFORM}/obj/mprCond.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprCond.c

${CC} -c -o ${PLATFORM}/obj/mprCrypt.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprCrypt.c

${CC} -c -o ${PLATFORM}/obj/mprDisk.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprDisk.c

${CC} -c -o ${PLATFORM}/obj/mprDispatcher.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprDispatcher.c

${CC} -c -o ${PLATFORM}/obj/mprEncode.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprEncode.c

${CC} -c -o ${PLATFORM}/obj/mprEpoll.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprEpoll.c

${CC} -c -o ${PLATFORM}/obj/mprEvent.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprEvent.c

${CC} -c -o ${PLATFORM}/obj/mprFile.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprFile.c

${CC} -c -o ${PLATFORM}/obj/mprFileSystem.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprFileSystem.c

${CC} -c -o ${PLATFORM}/obj/mprHash.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprHash.c

${CC} -c -o ${PLATFORM}/obj/mprJSON.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprJSON.c

${CC} -c -o ${PLATFORM}/obj/mprKqueue.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprKqueue.c

${CC} -c -o ${PLATFORM}/obj/mprList.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprList.c

${CC} -c -o ${PLATFORM}/obj/mprLock.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprLock.c

${CC} -c -o ${PLATFORM}/obj/mprLog.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprLog.c

${CC} -c -o ${PLATFORM}/obj/mprMem.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprMem.c

${CC} -c -o ${PLATFORM}/obj/mprMime.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprMime.c

${CC} -c -o ${PLATFORM}/obj/mprMixed.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprMixed.c

${CC} -c -o ${PLATFORM}/obj/mprModule.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprModule.c

${CC} -c -o ${PLATFORM}/obj/mprPath.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprPath.c

${CC} -c -o ${PLATFORM}/obj/mprPoll.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprPoll.c

${CC} -c -o ${PLATFORM}/obj/mprPrintf.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprPrintf.c

${CC} -c -o ${PLATFORM}/obj/mprRomFile.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprRomFile.c

${CC} -c -o ${PLATFORM}/obj/mprSelect.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprSelect.c

${CC} -c -o ${PLATFORM}/obj/mprSignal.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprSignal.c

${CC} -c -o ${PLATFORM}/obj/mprSocket.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprSocket.c

${CC} -c -o ${PLATFORM}/obj/mprString.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprString.c

${CC} -c -o ${PLATFORM}/obj/mprTest.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprTest.c

${CC} -c -o ${PLATFORM}/obj/mprThread.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprThread.c

${CC} -c -o ${PLATFORM}/obj/mprTime.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprTime.c

${CC} -c -o ${PLATFORM}/obj/mprUnix.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprUnix.c

${CC} -c -o ${PLATFORM}/obj/mprVxworks.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprVxworks.c

${CC} -c -o ${PLATFORM}/obj/mprWait.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprWait.c

${CC} -c -o ${PLATFORM}/obj/mprWide.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprWide.c

${CC} -c -o ${PLATFORM}/obj/mprWin.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprWin.c

${CC} -c -o ${PLATFORM}/obj/mprWince.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprWince.c

${CC} -c -o ${PLATFORM}/obj/mprXml.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/mprXml.c

${CC} -shared -o ${PLATFORM}/lib/libmpr.so ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/dtoa.o ${PLATFORM}/obj/mpr.o ${PLATFORM}/obj/mprAsync.o ${PLATFORM}/obj/mprAtomic.o ${PLATFORM}/obj/mprBuf.o ${PLATFORM}/obj/mprCache.o ${PLATFORM}/obj/mprCmd.o ${PLATFORM}/obj/mprCond.o ${PLATFORM}/obj/mprCrypt.o ${PLATFORM}/obj/mprDisk.o ${PLATFORM}/obj/mprDispatcher.o ${PLATFORM}/obj/mprEncode.o ${PLATFORM}/obj/mprEpoll.o ${PLATFORM}/obj/mprEvent.o ${PLATFORM}/obj/mprFile.o ${PLATFORM}/obj/mprFileSystem.o ${PLATFORM}/obj/mprHash.o ${PLATFORM}/obj/mprJSON.o ${PLATFORM}/obj/mprKqueue.o ${PLATFORM}/obj/mprList.o ${PLATFORM}/obj/mprLock.o ${PLATFORM}/obj/mprLog.o ${PLATFORM}/obj/mprMem.o ${PLATFORM}/obj/mprMime.o ${PLATFORM}/obj/mprMixed.o ${PLATFORM}/obj/mprModule.o ${PLATFORM}/obj/mprPath.o ${PLATFORM}/obj/mprPoll.o ${PLATFORM}/obj/mprPrintf.o ${PLATFORM}/obj/mprRomFile.o ${PLATFORM}/obj/mprSelect.o ${PLATFORM}/obj/mprSignal.o ${PLATFORM}/obj/mprSocket.o ${PLATFORM}/obj/mprString.o ${PLATFORM}/obj/mprTest.o ${PLATFORM}/obj/mprThread.o ${PLATFORM}/obj/mprTime.o ${PLATFORM}/obj/mprUnix.o ${PLATFORM}/obj/mprVxworks.o ${PLATFORM}/obj/mprWait.o ${PLATFORM}/obj/mprWide.o ${PLATFORM}/obj/mprWin.o ${PLATFORM}/obj/mprWince.o ${PLATFORM}/obj/mprXml.o ${LIBS}

${CC} -c -o ${PLATFORM}/obj/benchMpr.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/benchMpr.c

${CC} -o ${PLATFORM}/bin/benchMpr ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/benchMpr.o ${LIBS} -lmpr ${LDFLAGS}

${CC} -c -o ${PLATFORM}/obj/runProgram.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/runProgram.c

${CC} -o ${PLATFORM}/bin/runProgram ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/runProgram.o ${LIBS} ${LDFLAGS}

${CC} -c -o ${PLATFORM}/obj/testArgv.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testArgv.c

${CC} -c -o ${PLATFORM}/obj/testBuf.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testBuf.c

${CC} -c -o ${PLATFORM}/obj/testCmd.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testCmd.c

${CC} -c -o ${PLATFORM}/obj/testCond.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testCond.c

${CC} -c -o ${PLATFORM}/obj/testEvent.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testEvent.c

${CC} -c -o ${PLATFORM}/obj/testFile.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testFile.c

${CC} -c -o ${PLATFORM}/obj/testHash.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testHash.c

${CC} -c -o ${PLATFORM}/obj/testList.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testList.c

${CC} -c -o ${PLATFORM}/obj/testLock.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testLock.c

${CC} -c -o ${PLATFORM}/obj/testMem.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testMem.c

${CC} -c -o ${PLATFORM}/obj/testMpr.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testMpr.c

${CC} -c -o ${PLATFORM}/obj/testPath.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testPath.c

${CC} -c -o ${PLATFORM}/obj/testSocket.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testSocket.c

${CC} -c -o ${PLATFORM}/obj/testSprintf.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testSprintf.c

${CC} -c -o ${PLATFORM}/obj/testThread.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testThread.c

${CC} -c -o ${PLATFORM}/obj/testTime.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testTime.c

${CC} -c -o ${PLATFORM}/obj/testUnicode.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc test/testUnicode.c

${CC} -o ${PLATFORM}/bin/testMpr ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/testArgv.o ${PLATFORM}/obj/testBuf.o ${PLATFORM}/obj/testCmd.o ${PLATFORM}/obj/testCond.o ${PLATFORM}/obj/testEvent.o ${PLATFORM}/obj/testFile.o ${PLATFORM}/obj/testHash.o ${PLATFORM}/obj/testList.o ${PLATFORM}/obj/testLock.o ${PLATFORM}/obj/testMem.o ${PLATFORM}/obj/testMpr.o ${PLATFORM}/obj/testPath.o ${PLATFORM}/obj/testSocket.o ${PLATFORM}/obj/testSprintf.o ${PLATFORM}/obj/testThread.o ${PLATFORM}/obj/testTime.o ${PLATFORM}/obj/testUnicode.o ${LIBS} -lmpr ${LDFLAGS}

${CC} -c -o ${PLATFORM}/obj/manager.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/manager.c

${CC} -o ${PLATFORM}/bin/manager ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/manager.o ${LIBS} -lmpr ${LDFLAGS}

${CC} -c -o ${PLATFORM}/obj/makerom.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/utils/makerom.c

${CC} -o ${PLATFORM}/bin/makerom ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/makerom.o ${LIBS} -lmpr ${LDFLAGS}

${CC} -c -o ${PLATFORM}/obj/charGen.o -Wall -fPIC ${LDFLAGS} -mcpu=i686 ${DFLAGS} -I${PLATFORM}/inc src/utils/charGen.c

${CC} -o ${PLATFORM}/bin/chargen ${LDFLAGS} ${LIBPATHS} ${PLATFORM}/obj/charGen.o ${LIBS} -lmpr ${LDFLAGS}

