#
#   build.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

CC="cc"
CFLAGS="-fPIC -Wall -g"
DFLAGS="-DPIC -DCPU=I686"
IFLAGS="-Isrc -Imacosx-i686-debug/inc"
LDFLAGS="-Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -L/Users/mob/git/mpr/macosx-i686-debug/lib -g"
LIBS="-lpthread -lm"

${CC} -c -o macosx-i686-debug/obj/dtoa.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/dtoa.c

${CC} -c -o macosx-i686-debug/obj/mpr.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mpr.c

${CC} -c -o macosx-i686-debug/obj/mprAsync.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprAsync.c

${CC} -c -o macosx-i686-debug/obj/mprAtomic.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprAtomic.c

${CC} -c -o macosx-i686-debug/obj/mprBuf.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprBuf.c

${CC} -c -o macosx-i686-debug/obj/mprCache.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprCache.c

${CC} -c -o macosx-i686-debug/obj/mprCmd.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprCmd.c

${CC} -c -o macosx-i686-debug/obj/mprCond.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprCond.c

${CC} -c -o macosx-i686-debug/obj/mprCrypt.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprCrypt.c

${CC} -c -o macosx-i686-debug/obj/mprDisk.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprDisk.c

${CC} -c -o macosx-i686-debug/obj/mprDispatcher.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprDispatcher.c

${CC} -c -o macosx-i686-debug/obj/mprEncode.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprEncode.c

${CC} -c -o macosx-i686-debug/obj/mprEpoll.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprEpoll.c

${CC} -c -o macosx-i686-debug/obj/mprEvent.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprEvent.c

${CC} -c -o macosx-i686-debug/obj/mprFile.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprFile.c

${CC} -c -o macosx-i686-debug/obj/mprFileSystem.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprFileSystem.c

${CC} -c -o macosx-i686-debug/obj/mprHash.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprHash.c

${CC} -c -o macosx-i686-debug/obj/mprJSON.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprJSON.c

${CC} -c -o macosx-i686-debug/obj/mprKqueue.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprKqueue.c

${CC} -c -o macosx-i686-debug/obj/mprList.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprList.c

${CC} -c -o macosx-i686-debug/obj/mprLock.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprLock.c

${CC} -c -o macosx-i686-debug/obj/mprLog.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprLog.c

${CC} -c -o macosx-i686-debug/obj/mprMem.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprMem.c

${CC} -c -o macosx-i686-debug/obj/mprMime.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprMime.c

${CC} -c -o macosx-i686-debug/obj/mprMixed.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprMixed.c

${CC} -c -o macosx-i686-debug/obj/mprModule.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprModule.c

${CC} -c -o macosx-i686-debug/obj/mprPath.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprPath.c

${CC} -c -o macosx-i686-debug/obj/mprPoll.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprPoll.c

${CC} -c -o macosx-i686-debug/obj/mprPrintf.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprPrintf.c

${CC} -c -o macosx-i686-debug/obj/mprRomFile.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprRomFile.c

${CC} -c -o macosx-i686-debug/obj/mprSelect.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprSelect.c

${CC} -c -o macosx-i686-debug/obj/mprSignal.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprSignal.c

${CC} -c -o macosx-i686-debug/obj/mprSocket.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprSocket.c

${CC} -c -o macosx-i686-debug/obj/mprString.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprString.c

${CC} -c -o macosx-i686-debug/obj/mprTest.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprTest.c

${CC} -c -o macosx-i686-debug/obj/mprThread.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprThread.c

${CC} -c -o macosx-i686-debug/obj/mprTime.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprTime.c

${CC} -c -o macosx-i686-debug/obj/mprUnix.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprUnix.c

${CC} -c -o macosx-i686-debug/obj/mprVxworks.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprVxworks.c

${CC} -c -o macosx-i686-debug/obj/mprWait.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprWait.c

${CC} -c -o macosx-i686-debug/obj/mprWide.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprWide.c

${CC} -c -o macosx-i686-debug/obj/mprWin.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprWin.c

${CC} -c -o macosx-i686-debug/obj/mprWince.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprWince.c

${CC} -c -o macosx-i686-debug/obj/mprXml.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/mprXml.c

${CC} -dynamiclib -o macosx-i686-debug/lib/libmpr.dylib -arch i686 ${LDFLAGS} -install_name @rpath/libmpr.dylib macosx-i686-debug/obj/dtoa.o macosx-i686-debug/obj/mpr.o macosx-i686-debug/obj/mprAsync.o macosx-i686-debug/obj/mprAtomic.o macosx-i686-debug/obj/mprBuf.o macosx-i686-debug/obj/mprCache.o macosx-i686-debug/obj/mprCmd.o macosx-i686-debug/obj/mprCond.o macosx-i686-debug/obj/mprCrypt.o macosx-i686-debug/obj/mprDisk.o macosx-i686-debug/obj/mprDispatcher.o macosx-i686-debug/obj/mprEncode.o macosx-i686-debug/obj/mprEpoll.o macosx-i686-debug/obj/mprEvent.o macosx-i686-debug/obj/mprFile.o macosx-i686-debug/obj/mprFileSystem.o macosx-i686-debug/obj/mprHash.o macosx-i686-debug/obj/mprJSON.o macosx-i686-debug/obj/mprKqueue.o macosx-i686-debug/obj/mprList.o macosx-i686-debug/obj/mprLock.o macosx-i686-debug/obj/mprLog.o macosx-i686-debug/obj/mprMem.o macosx-i686-debug/obj/mprMime.o macosx-i686-debug/obj/mprMixed.o macosx-i686-debug/obj/mprModule.o macosx-i686-debug/obj/mprPath.o macosx-i686-debug/obj/mprPoll.o macosx-i686-debug/obj/mprPrintf.o macosx-i686-debug/obj/mprRomFile.o macosx-i686-debug/obj/mprSelect.o macosx-i686-debug/obj/mprSignal.o macosx-i686-debug/obj/mprSocket.o macosx-i686-debug/obj/mprString.o macosx-i686-debug/obj/mprTest.o macosx-i686-debug/obj/mprThread.o macosx-i686-debug/obj/mprTime.o macosx-i686-debug/obj/mprUnix.o macosx-i686-debug/obj/mprVxworks.o macosx-i686-debug/obj/mprWait.o macosx-i686-debug/obj/mprWide.o macosx-i686-debug/obj/mprWin.o macosx-i686-debug/obj/mprWince.o macosx-i686-debug/obj/mprXml.o ${LIBS}

${CC} -c -o macosx-i686-debug/obj/benchMpr.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/benchMpr.c

${CC} -o macosx-i686-debug/bin/benchMpr -arch i686 ${LDFLAGS} -Lmacosx-i686-debug/lib macosx-i686-debug/obj/benchMpr.o ${LIBS} -lmpr

${CC} -c -o macosx-i686-debug/obj/runProgram.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/runProgram.c

${CC} -o macosx-i686-debug/bin/runProgram -arch i686 ${LDFLAGS} -Lmacosx-i686-debug/lib macosx-i686-debug/obj/runProgram.o ${LIBS}

${CC} -c -o macosx-i686-debug/obj/testArgv.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testArgv.c

${CC} -c -o macosx-i686-debug/obj/testBuf.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testBuf.c

${CC} -c -o macosx-i686-debug/obj/testCmd.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testCmd.c

${CC} -c -o macosx-i686-debug/obj/testCond.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testCond.c

${CC} -c -o macosx-i686-debug/obj/testEvent.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testEvent.c

${CC} -c -o macosx-i686-debug/obj/testFile.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testFile.c

${CC} -c -o macosx-i686-debug/obj/testHash.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testHash.c

${CC} -c -o macosx-i686-debug/obj/testList.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testList.c

${CC} -c -o macosx-i686-debug/obj/testLock.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testLock.c

${CC} -c -o macosx-i686-debug/obj/testMem.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testMem.c

${CC} -c -o macosx-i686-debug/obj/testMpr.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testMpr.c

${CC} -c -o macosx-i686-debug/obj/testPath.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testPath.c

${CC} -c -o macosx-i686-debug/obj/testSocket.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testSocket.c

${CC} -c -o macosx-i686-debug/obj/testSprintf.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testSprintf.c

${CC} -c -o macosx-i686-debug/obj/testThread.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testThread.c

${CC} -c -o macosx-i686-debug/obj/testTime.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testTime.c

${CC} -c -o macosx-i686-debug/obj/testUnicode.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc test/testUnicode.c

${CC} -o macosx-i686-debug/bin/testMpr -arch i686 ${LDFLAGS} -Lmacosx-i686-debug/lib macosx-i686-debug/obj/testArgv.o macosx-i686-debug/obj/testBuf.o macosx-i686-debug/obj/testCmd.o macosx-i686-debug/obj/testCond.o macosx-i686-debug/obj/testEvent.o macosx-i686-debug/obj/testFile.o macosx-i686-debug/obj/testHash.o macosx-i686-debug/obj/testList.o macosx-i686-debug/obj/testLock.o macosx-i686-debug/obj/testMem.o macosx-i686-debug/obj/testMpr.o macosx-i686-debug/obj/testPath.o macosx-i686-debug/obj/testSocket.o macosx-i686-debug/obj/testSprintf.o macosx-i686-debug/obj/testThread.o macosx-i686-debug/obj/testTime.o macosx-i686-debug/obj/testUnicode.o ${LIBS} -lmpr

${CC} -c -o macosx-i686-debug/obj/manager.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/manager.c

${CC} -o macosx-i686-debug/bin/manager -arch i686 ${LDFLAGS} -Lmacosx-i686-debug/lib macosx-i686-debug/obj/manager.o ${LIBS} -lmpr

${CC} -c -o macosx-i686-debug/obj/makerom.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/utils/makerom.c

${CC} -o macosx-i686-debug/bin/makerom -arch i686 ${LDFLAGS} -Lmacosx-i686-debug/lib macosx-i686-debug/obj/makerom.o ${LIBS} -lmpr

${CC} -c -o macosx-i686-debug/obj/charGen.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug/inc src/utils/charGen.c

${CC} -o macosx-i686-debug/bin/chargen -arch i686 ${LDFLAGS} -Lmacosx-i686-debug/lib macosx-i686-debug/obj/charGen.o ${LIBS} -lmpr

