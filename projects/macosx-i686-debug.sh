#
#   build.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

CC="cc"
CFLAGS="-fPIC -Wall -g"
DFLAGS="-DPIC -DCPU=I686"
IFLAGS="-Isrc -Imacosx-i686-debug\inc"
LDFLAGS="-Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -LC:\cygwin\home\mob\mpr\macosx-i686-debug\lib -g"
LIBS="-lpthread -lm"

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\dtoa.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\dtoa.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mpr.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mpr.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprAsync.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprAsync.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprAtomic.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprAtomic.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprBuf.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprBuf.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCache.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCache.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCmd.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCmd.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCond.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCond.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCrypt.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprCrypt.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprDisk.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprDisk.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprDispatcher.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprDispatcher.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEncode.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprEncode.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEpoll.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprEpoll.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEvent.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprEvent.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprFile.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprFile.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprFileSystem.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprFileSystem.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprHash.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprHash.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprJSON.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprJSON.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprKqueue.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprKqueue.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprList.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprList.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprLock.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprLock.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprLog.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprLog.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMem.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprMem.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMime.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprMime.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMixed.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprMixed.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprModule.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprModule.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPath.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprPath.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPoll.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprPoll.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPrintf.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprPrintf.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprRomFile.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprRomFile.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSelect.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprSelect.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSignal.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprSignal.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSocket.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprSocket.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprString.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprString.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprTest.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprTest.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprThread.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprThread.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprTime.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprTime.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprUnix.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprUnix.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprVxworks.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprVxworks.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWait.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWait.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWide.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWide.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWin.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWin.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWince.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprWince.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprXml.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\mprXml.c

${CC} -dynamiclib -o C:\cygwin\home\mob\mpr\macosx-i686-debug\lib\libmpr.dylib -arch i686 ${LDFLAGS} -install_name @rpath/libmpr.dylib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\dtoa.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mpr.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprAsync.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprAtomic.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprBuf.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCache.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCmd.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCond.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprCrypt.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprDisk.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprDispatcher.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEncode.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEpoll.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprEvent.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprFile.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprFileSystem.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprHash.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprJSON.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprKqueue.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprList.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprLock.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprLog.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMem.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMime.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprMixed.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprModule.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPath.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPoll.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprPrintf.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprRomFile.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSelect.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSignal.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprSocket.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprString.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprTest.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprThread.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprTime.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprUnix.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprVxworks.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWait.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWide.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWin.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprWince.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\mprXml.o ${LIBS}

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\benchMpr.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\benchMpr.c

${CC} -o C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\benchMpr -arch i686 ${LDFLAGS} -Lmacosx-i686-debug/lib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\benchMpr.o ${LIBS} -lmpr

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\runProgram.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\runProgram.c

${CC} -o C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\runProgram -arch i686 ${LDFLAGS} -Lmacosx-i686-debug/lib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\runProgram.o ${LIBS}

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testArgv.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testArgv.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testBuf.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testBuf.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testCmd.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testCmd.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testCond.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testCond.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testEvent.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testEvent.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testFile.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testFile.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testHash.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testHash.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testList.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testList.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testLock.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testLock.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testMem.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testMem.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testMpr.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testMpr.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testPath.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testPath.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testSocket.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testSocket.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testSprintf.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testSprintf.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testThread.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testThread.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testTime.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testTime.c

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testUnicode.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\test\testUnicode.c

${CC} -o C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\testMpr -arch i686 ${LDFLAGS} -Lmacosx-i686-debug/lib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testArgv.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testBuf.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testCmd.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testCond.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testEvent.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testFile.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testHash.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testList.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testLock.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testMem.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testMpr.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testPath.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testSocket.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testSprintf.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testThread.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testTime.o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\testUnicode.o ${LIBS} -lmpr

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\manager.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\manager.c

${CC} -o C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\manager -arch i686 ${LDFLAGS} -Lmacosx-i686-debug/lib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\manager.o ${LIBS} -lmpr

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\makerom.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\utils\makerom.c

${CC} -o C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\makerom -arch i686 ${LDFLAGS} -Lmacosx-i686-debug/lib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\makerom.o ${LIBS} -lmpr

${CC} -c -o C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\charGen.o -arch i686 ${CFLAGS} ${DFLAGS} -Isrc -Imacosx-i686-debug\inc C:\cygwin\home\mob\mpr\src\utils\charGen.c

${CC} -o C:\cygwin\home\mob\mpr\macosx-i686-debug\bin\chargen -arch i686 ${LDFLAGS} -Lmacosx-i686-debug/lib C:\cygwin\home\mob\mpr\macosx-i686-debug\obj\charGen.o ${LIBS} -lmpr

