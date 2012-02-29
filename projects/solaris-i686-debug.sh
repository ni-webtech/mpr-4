#
#   build.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

CC="cc"
CFLAGS="-fPIC -g -mcpu=i686"
DFLAGS="-DPIC"
IFLAGS="-Isrc -Isolaris-i686-debug/inc"
LDFLAGS="-L/Users/mob/git/mpr/solaris-i686-debug/lib -g"
LIBS="-lpthread -lm"

${CC} -c -o solaris-i686-debug/obj/dtoa.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/dtoa.c

${CC} -c -o solaris-i686-debug/obj/mpr.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mpr.c

${CC} -c -o solaris-i686-debug/obj/mprAsync.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprAsync.c

${CC} -c -o solaris-i686-debug/obj/mprAtomic.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprAtomic.c

${CC} -c -o solaris-i686-debug/obj/mprBuf.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprBuf.c

${CC} -c -o solaris-i686-debug/obj/mprCache.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprCache.c

${CC} -c -o solaris-i686-debug/obj/mprCmd.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprCmd.c

${CC} -c -o solaris-i686-debug/obj/mprCond.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprCond.c

${CC} -c -o solaris-i686-debug/obj/mprCrypt.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprCrypt.c

${CC} -c -o solaris-i686-debug/obj/mprDisk.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprDisk.c

${CC} -c -o solaris-i686-debug/obj/mprDispatcher.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprDispatcher.c

${CC} -c -o solaris-i686-debug/obj/mprEncode.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprEncode.c

${CC} -c -o solaris-i686-debug/obj/mprEpoll.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprEpoll.c

${CC} -c -o solaris-i686-debug/obj/mprEvent.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprEvent.c

${CC} -c -o solaris-i686-debug/obj/mprFile.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprFile.c

${CC} -c -o solaris-i686-debug/obj/mprFileSystem.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprFileSystem.c

${CC} -c -o solaris-i686-debug/obj/mprHash.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprHash.c

${CC} -c -o solaris-i686-debug/obj/mprJSON.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprJSON.c

${CC} -c -o solaris-i686-debug/obj/mprKqueue.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprKqueue.c

${CC} -c -o solaris-i686-debug/obj/mprList.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprList.c

${CC} -c -o solaris-i686-debug/obj/mprLock.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprLock.c

${CC} -c -o solaris-i686-debug/obj/mprLog.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprLog.c

${CC} -c -o solaris-i686-debug/obj/mprMem.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprMem.c

${CC} -c -o solaris-i686-debug/obj/mprMime.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprMime.c

${CC} -c -o solaris-i686-debug/obj/mprMixed.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprMixed.c

${CC} -c -o solaris-i686-debug/obj/mprModule.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprModule.c

${CC} -c -o solaris-i686-debug/obj/mprPath.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprPath.c

${CC} -c -o solaris-i686-debug/obj/mprPoll.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprPoll.c

${CC} -c -o solaris-i686-debug/obj/mprPrintf.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprPrintf.c

${CC} -c -o solaris-i686-debug/obj/mprRomFile.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprRomFile.c

${CC} -c -o solaris-i686-debug/obj/mprSelect.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprSelect.c

${CC} -c -o solaris-i686-debug/obj/mprSignal.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprSignal.c

${CC} -c -o solaris-i686-debug/obj/mprSocket.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprSocket.c

${CC} -c -o solaris-i686-debug/obj/mprString.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprString.c

${CC} -c -o solaris-i686-debug/obj/mprTest.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprTest.c

${CC} -c -o solaris-i686-debug/obj/mprThread.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprThread.c

${CC} -c -o solaris-i686-debug/obj/mprTime.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprTime.c

${CC} -c -o solaris-i686-debug/obj/mprUnix.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprUnix.c

${CC} -c -o solaris-i686-debug/obj/mprVxworks.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprVxworks.c

${CC} -c -o solaris-i686-debug/obj/mprWait.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprWait.c

${CC} -c -o solaris-i686-debug/obj/mprWide.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprWide.c

${CC} -c -o solaris-i686-debug/obj/mprWin.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprWin.c

${CC} -c -o solaris-i686-debug/obj/mprWince.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprWince.c

${CC} -c -o solaris-i686-debug/obj/mprXml.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/mprXml.c

${CC} -shared -o solaris-i686-debug/lib/libmpr.so ${LDFLAGS} solaris-i686-debug/obj/dtoa.o solaris-i686-debug/obj/mpr.o solaris-i686-debug/obj/mprAsync.o solaris-i686-debug/obj/mprAtomic.o solaris-i686-debug/obj/mprBuf.o solaris-i686-debug/obj/mprCache.o solaris-i686-debug/obj/mprCmd.o solaris-i686-debug/obj/mprCond.o solaris-i686-debug/obj/mprCrypt.o solaris-i686-debug/obj/mprDisk.o solaris-i686-debug/obj/mprDispatcher.o solaris-i686-debug/obj/mprEncode.o solaris-i686-debug/obj/mprEpoll.o solaris-i686-debug/obj/mprEvent.o solaris-i686-debug/obj/mprFile.o solaris-i686-debug/obj/mprFileSystem.o solaris-i686-debug/obj/mprHash.o solaris-i686-debug/obj/mprJSON.o solaris-i686-debug/obj/mprKqueue.o solaris-i686-debug/obj/mprList.o solaris-i686-debug/obj/mprLock.o solaris-i686-debug/obj/mprLog.o solaris-i686-debug/obj/mprMem.o solaris-i686-debug/obj/mprMime.o solaris-i686-debug/obj/mprMixed.o solaris-i686-debug/obj/mprModule.o solaris-i686-debug/obj/mprPath.o solaris-i686-debug/obj/mprPoll.o solaris-i686-debug/obj/mprPrintf.o solaris-i686-debug/obj/mprRomFile.o solaris-i686-debug/obj/mprSelect.o solaris-i686-debug/obj/mprSignal.o solaris-i686-debug/obj/mprSocket.o solaris-i686-debug/obj/mprString.o solaris-i686-debug/obj/mprTest.o solaris-i686-debug/obj/mprThread.o solaris-i686-debug/obj/mprTime.o solaris-i686-debug/obj/mprUnix.o solaris-i686-debug/obj/mprVxworks.o solaris-i686-debug/obj/mprWait.o solaris-i686-debug/obj/mprWide.o solaris-i686-debug/obj/mprWin.o solaris-i686-debug/obj/mprWince.o solaris-i686-debug/obj/mprXml.o ${LIBS}

${CC} -c -o solaris-i686-debug/obj/benchMpr.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/benchMpr.c

${CC} -o solaris-i686-debug/bin/benchMpr ${LDFLAGS} -Lsolaris-i686-debug/lib solaris-i686-debug/obj/benchMpr.o ${LIBS} -lmpr -Lsolaris-i686-debug/lib -g

${CC} -c -o solaris-i686-debug/obj/runProgram.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/runProgram.c

${CC} -o solaris-i686-debug/bin/runProgram ${LDFLAGS} -Lsolaris-i686-debug/lib solaris-i686-debug/obj/runProgram.o ${LIBS} -Lsolaris-i686-debug/lib -g

${CC} -c -o solaris-i686-debug/obj/testArgv.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testArgv.c

${CC} -c -o solaris-i686-debug/obj/testBuf.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testBuf.c

${CC} -c -o solaris-i686-debug/obj/testCmd.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testCmd.c

${CC} -c -o solaris-i686-debug/obj/testCond.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testCond.c

${CC} -c -o solaris-i686-debug/obj/testEvent.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testEvent.c

${CC} -c -o solaris-i686-debug/obj/testFile.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testFile.c

${CC} -c -o solaris-i686-debug/obj/testHash.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testHash.c

${CC} -c -o solaris-i686-debug/obj/testList.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testList.c

${CC} -c -o solaris-i686-debug/obj/testLock.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testLock.c

${CC} -c -o solaris-i686-debug/obj/testMem.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testMem.c

${CC} -c -o solaris-i686-debug/obj/testMpr.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testMpr.c

${CC} -c -o solaris-i686-debug/obj/testPath.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testPath.c

${CC} -c -o solaris-i686-debug/obj/testSocket.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testSocket.c

${CC} -c -o solaris-i686-debug/obj/testSprintf.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testSprintf.c

${CC} -c -o solaris-i686-debug/obj/testThread.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testThread.c

${CC} -c -o solaris-i686-debug/obj/testTime.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testTime.c

${CC} -c -o solaris-i686-debug/obj/testUnicode.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc test/testUnicode.c

${CC} -o solaris-i686-debug/bin/testMpr ${LDFLAGS} -Lsolaris-i686-debug/lib solaris-i686-debug/obj/testArgv.o solaris-i686-debug/obj/testBuf.o solaris-i686-debug/obj/testCmd.o solaris-i686-debug/obj/testCond.o solaris-i686-debug/obj/testEvent.o solaris-i686-debug/obj/testFile.o solaris-i686-debug/obj/testHash.o solaris-i686-debug/obj/testList.o solaris-i686-debug/obj/testLock.o solaris-i686-debug/obj/testMem.o solaris-i686-debug/obj/testMpr.o solaris-i686-debug/obj/testPath.o solaris-i686-debug/obj/testSocket.o solaris-i686-debug/obj/testSprintf.o solaris-i686-debug/obj/testThread.o solaris-i686-debug/obj/testTime.o solaris-i686-debug/obj/testUnicode.o ${LIBS} -lmpr -Lsolaris-i686-debug/lib -g

${CC} -c -o solaris-i686-debug/obj/manager.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/manager.c

${CC} -o solaris-i686-debug/bin/manager ${LDFLAGS} -Lsolaris-i686-debug/lib solaris-i686-debug/obj/manager.o ${LIBS} -lmpr -Lsolaris-i686-debug/lib -g

${CC} -c -o solaris-i686-debug/obj/makerom.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/utils/makerom.c

${CC} -o solaris-i686-debug/bin/makerom ${LDFLAGS} -Lsolaris-i686-debug/lib solaris-i686-debug/obj/makerom.o ${LIBS} -lmpr -Lsolaris-i686-debug/lib -g

${CC} -c -o solaris-i686-debug/obj/charGen.o ${CFLAGS} ${DFLAGS} -Isrc -Isolaris-i686-debug/inc src/utils/charGen.c

${CC} -o solaris-i686-debug/bin/chargen ${LDFLAGS} -Lsolaris-i686-debug/lib solaris-i686-debug/obj/charGen.o ${LIBS} -lmpr -Lsolaris-i686-debug/lib -g

