#
#   build.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

PLATFORM="macosx-x86_64-debug"
CC="/usr/bin/cc"
CFLAGS="-fPIC -Wall -g -Wshorten-64-to-32"
DFLAGS="-DPIC -DCPU=X86_64"
IFLAGS="-Imacosx-x86_64-debug/inc -Isrc"
LDFLAGS="-Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -L/Users/mob/git/mpr/${PLATFORM}/lib -g -ldl"
LIBS="-lpthread -lm"

${CC} -c -o ${PLATFORM}/obj/dtoa.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/dtoa.c

${CC} -c -o ${PLATFORM}/obj/mpr.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mpr.c

${CC} -c -o ${PLATFORM}/obj/mprAsync.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprAsync.c

${CC} -c -o ${PLATFORM}/obj/mprAtomic.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprAtomic.c

${CC} -c -o ${PLATFORM}/obj/mprBuf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprBuf.c

${CC} -c -o ${PLATFORM}/obj/mprCache.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprCache.c

${CC} -c -o ${PLATFORM}/obj/mprCmd.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprCmd.c

${CC} -c -o ${PLATFORM}/obj/mprCond.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprCond.c

${CC} -c -o ${PLATFORM}/obj/mprCrypt.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprCrypt.c

${CC} -c -o ${PLATFORM}/obj/mprDisk.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprDisk.c

${CC} -c -o ${PLATFORM}/obj/mprDispatcher.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprDispatcher.c

${CC} -c -o ${PLATFORM}/obj/mprEncode.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprEncode.c

${CC} -c -o ${PLATFORM}/obj/mprEpoll.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprEpoll.c

${CC} -c -o ${PLATFORM}/obj/mprEvent.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprEvent.c

${CC} -c -o ${PLATFORM}/obj/mprFile.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprFile.c

${CC} -c -o ${PLATFORM}/obj/mprFileSystem.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprFileSystem.c

${CC} -c -o ${PLATFORM}/obj/mprHash.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprHash.c

${CC} -c -o ${PLATFORM}/obj/mprJSON.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprJSON.c

${CC} -c -o ${PLATFORM}/obj/mprKqueue.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprKqueue.c

${CC} -c -o ${PLATFORM}/obj/mprList.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprList.c

${CC} -c -o ${PLATFORM}/obj/mprLock.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprLock.c

${CC} -c -o ${PLATFORM}/obj/mprLog.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprLog.c

${CC} -c -o ${PLATFORM}/obj/mprMem.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprMem.c

${CC} -c -o ${PLATFORM}/obj/mprMime.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprMime.c

${CC} -c -o ${PLATFORM}/obj/mprMixed.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprMixed.c

${CC} -c -o ${PLATFORM}/obj/mprModule.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprModule.c

${CC} -c -o ${PLATFORM}/obj/mprPath.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprPath.c

${CC} -c -o ${PLATFORM}/obj/mprPoll.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprPoll.c

${CC} -c -o ${PLATFORM}/obj/mprPrintf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprPrintf.c

${CC} -c -o ${PLATFORM}/obj/mprRomFile.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprRomFile.c

${CC} -c -o ${PLATFORM}/obj/mprSelect.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprSelect.c

${CC} -c -o ${PLATFORM}/obj/mprSignal.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprSignal.c

${CC} -c -o ${PLATFORM}/obj/mprSocket.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprSocket.c

${CC} -c -o ${PLATFORM}/obj/mprString.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprString.c

${CC} -c -o ${PLATFORM}/obj/mprTest.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprTest.c

${CC} -c -o ${PLATFORM}/obj/mprThread.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprThread.c

${CC} -c -o ${PLATFORM}/obj/mprTime.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprTime.c

${CC} -c -o ${PLATFORM}/obj/mprUnix.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprUnix.c

${CC} -c -o ${PLATFORM}/obj/mprVxworks.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprVxworks.c

${CC} -c -o ${PLATFORM}/obj/mprWait.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprWait.c

${CC} -c -o ${PLATFORM}/obj/mprWide.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprWide.c

${CC} -c -o ${PLATFORM}/obj/mprWin.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprWin.c

${CC} -c -o ${PLATFORM}/obj/mprWince.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprWince.c

${CC} -c -o ${PLATFORM}/obj/mprXml.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/mprXml.c

${CC} -dynamiclib -o ${PLATFORM}/lib/libmpr.dylib -arch x86_64 -Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -L${PLATFORM}/lib -g -ldl -install_name @rpath/libmpr.dylib ${PLATFORM}/obj/dtoa.o ${PLATFORM}/obj/mpr.o ${PLATFORM}/obj/mprAsync.o ${PLATFORM}/obj/mprAtomic.o ${PLATFORM}/obj/mprBuf.o ${PLATFORM}/obj/mprCache.o ${PLATFORM}/obj/mprCmd.o ${PLATFORM}/obj/mprCond.o ${PLATFORM}/obj/mprCrypt.o ${PLATFORM}/obj/mprDisk.o ${PLATFORM}/obj/mprDispatcher.o ${PLATFORM}/obj/mprEncode.o ${PLATFORM}/obj/mprEpoll.o ${PLATFORM}/obj/mprEvent.o ${PLATFORM}/obj/mprFile.o ${PLATFORM}/obj/mprFileSystem.o ${PLATFORM}/obj/mprHash.o ${PLATFORM}/obj/mprJSON.o ${PLATFORM}/obj/mprKqueue.o ${PLATFORM}/obj/mprList.o ${PLATFORM}/obj/mprLock.o ${PLATFORM}/obj/mprLog.o ${PLATFORM}/obj/mprMem.o ${PLATFORM}/obj/mprMime.o ${PLATFORM}/obj/mprMixed.o ${PLATFORM}/obj/mprModule.o ${PLATFORM}/obj/mprPath.o ${PLATFORM}/obj/mprPoll.o ${PLATFORM}/obj/mprPrintf.o ${PLATFORM}/obj/mprRomFile.o ${PLATFORM}/obj/mprSelect.o ${PLATFORM}/obj/mprSignal.o ${PLATFORM}/obj/mprSocket.o ${PLATFORM}/obj/mprString.o ${PLATFORM}/obj/mprTest.o ${PLATFORM}/obj/mprThread.o ${PLATFORM}/obj/mprTime.o ${PLATFORM}/obj/mprUnix.o ${PLATFORM}/obj/mprVxworks.o ${PLATFORM}/obj/mprWait.o ${PLATFORM}/obj/mprWide.o ${PLATFORM}/obj/mprWin.o ${PLATFORM}/obj/mprWince.o ${PLATFORM}/obj/mprXml.o ${LIBS}

${CC} -c -o ${PLATFORM}/obj/benchMpr.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/benchMpr.c

${CC} -o ${PLATFORM}/bin/benchMpr -arch x86_64 -Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -L${PLATFORM}/lib -g -ldl -L${PLATFORM}/lib ${PLATFORM}/obj/benchMpr.o ${LIBS} -lmpr

${CC} -c -o ${PLATFORM}/obj/runProgram.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/runProgram.c

${CC} -o ${PLATFORM}/bin/runProgram -arch x86_64 -Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -L${PLATFORM}/lib -g -ldl -L${PLATFORM}/lib ${PLATFORM}/obj/runProgram.o ${LIBS}

${CC} -c -o ${PLATFORM}/obj/testArgv.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testArgv.c

${CC} -c -o ${PLATFORM}/obj/testBuf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testBuf.c

${CC} -c -o ${PLATFORM}/obj/testCmd.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testCmd.c

${CC} -c -o ${PLATFORM}/obj/testCond.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testCond.c

${CC} -c -o ${PLATFORM}/obj/testEvent.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testEvent.c

${CC} -c -o ${PLATFORM}/obj/testFile.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testFile.c

${CC} -c -o ${PLATFORM}/obj/testHash.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testHash.c

${CC} -c -o ${PLATFORM}/obj/testList.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testList.c

${CC} -c -o ${PLATFORM}/obj/testLock.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testLock.c

${CC} -c -o ${PLATFORM}/obj/testMem.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testMem.c

${CC} -c -o ${PLATFORM}/obj/testMpr.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testMpr.c

${CC} -c -o ${PLATFORM}/obj/testPath.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testPath.c

${CC} -c -o ${PLATFORM}/obj/testSocket.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testSocket.c

${CC} -c -o ${PLATFORM}/obj/testSprintf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testSprintf.c

${CC} -c -o ${PLATFORM}/obj/testThread.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testThread.c

${CC} -c -o ${PLATFORM}/obj/testTime.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testTime.c

${CC} -c -o ${PLATFORM}/obj/testUnicode.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc test/testUnicode.c

${CC} -o ${PLATFORM}/bin/testMpr -arch x86_64 -Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -L${PLATFORM}/lib -g -ldl -L${PLATFORM}/lib ${PLATFORM}/obj/testArgv.o ${PLATFORM}/obj/testBuf.o ${PLATFORM}/obj/testCmd.o ${PLATFORM}/obj/testCond.o ${PLATFORM}/obj/testEvent.o ${PLATFORM}/obj/testFile.o ${PLATFORM}/obj/testHash.o ${PLATFORM}/obj/testList.o ${PLATFORM}/obj/testLock.o ${PLATFORM}/obj/testMem.o ${PLATFORM}/obj/testMpr.o ${PLATFORM}/obj/testPath.o ${PLATFORM}/obj/testSocket.o ${PLATFORM}/obj/testSprintf.o ${PLATFORM}/obj/testThread.o ${PLATFORM}/obj/testTime.o ${PLATFORM}/obj/testUnicode.o ${LIBS} -lmpr

${CC} -c -o ${PLATFORM}/obj/manager.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/manager.c

${CC} -o ${PLATFORM}/bin/manager -arch x86_64 -Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -L${PLATFORM}/lib -g -ldl -L${PLATFORM}/lib ${PLATFORM}/obj/manager.o ${LIBS} -lmpr

${CC} -c -o ${PLATFORM}/obj/makerom.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/utils/makerom.c

${CC} -o ${PLATFORM}/bin/makerom -arch x86_64 -Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -L${PLATFORM}/lib -g -ldl -L${PLATFORM}/lib ${PLATFORM}/obj/makerom.o ${LIBS} -lmpr

${CC} -c -o ${PLATFORM}/obj/charGen.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc -Isrc src/utils/charGen.c

${CC} -o ${PLATFORM}/bin/chargen -arch x86_64 -Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -L${PLATFORM}/lib -g -ldl -L${PLATFORM}/lib ${PLATFORM}/obj/charGen.o ${LIBS} -lmpr

