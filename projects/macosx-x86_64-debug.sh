#
#   build.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

CC="/usr/bin/cc"
CFLAGS="-fPIC -Wall -g -Wshorten-64-to-32"
DFLAGS="-DPIC -DCPU=X86_64"
IFLAGS="-Imacosx-x86_64-debug/inc -Isrc"
LDFLAGS="-Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -L/Users/mob/git/mpr/macosx-x86_64-debug/lib -g -ldl"
LIBS="-lpthread -lm"

${CC} -c -o macosx-x86_64-debug/obj/dtoa.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/dtoa.c

${CC} -c -o macosx-x86_64-debug/obj/mpr.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mpr.c

${CC} -c -o macosx-x86_64-debug/obj/mprAsync.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprAsync.c

${CC} -c -o macosx-x86_64-debug/obj/mprAtomic.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprAtomic.c

${CC} -c -o macosx-x86_64-debug/obj/mprBuf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprBuf.c

${CC} -c -o macosx-x86_64-debug/obj/mprCache.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprCache.c

${CC} -c -o macosx-x86_64-debug/obj/mprCmd.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprCmd.c

${CC} -c -o macosx-x86_64-debug/obj/mprCond.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprCond.c

${CC} -c -o macosx-x86_64-debug/obj/mprCrypt.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprCrypt.c

${CC} -c -o macosx-x86_64-debug/obj/mprDisk.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprDisk.c

${CC} -c -o macosx-x86_64-debug/obj/mprDispatcher.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprDispatcher.c

${CC} -c -o macosx-x86_64-debug/obj/mprEncode.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprEncode.c

${CC} -c -o macosx-x86_64-debug/obj/mprEpoll.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprEpoll.c

${CC} -c -o macosx-x86_64-debug/obj/mprEvent.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprEvent.c

${CC} -c -o macosx-x86_64-debug/obj/mprFile.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprFile.c

${CC} -c -o macosx-x86_64-debug/obj/mprFileSystem.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprFileSystem.c

${CC} -c -o macosx-x86_64-debug/obj/mprHash.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprHash.c

${CC} -c -o macosx-x86_64-debug/obj/mprJSON.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprJSON.c

${CC} -c -o macosx-x86_64-debug/obj/mprKqueue.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprKqueue.c

${CC} -c -o macosx-x86_64-debug/obj/mprList.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprList.c

${CC} -c -o macosx-x86_64-debug/obj/mprLock.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprLock.c

${CC} -c -o macosx-x86_64-debug/obj/mprLog.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprLog.c

${CC} -c -o macosx-x86_64-debug/obj/mprMem.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprMem.c

${CC} -c -o macosx-x86_64-debug/obj/mprMime.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprMime.c

${CC} -c -o macosx-x86_64-debug/obj/mprMixed.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprMixed.c

${CC} -c -o macosx-x86_64-debug/obj/mprModule.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprModule.c

${CC} -c -o macosx-x86_64-debug/obj/mprPath.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprPath.c

${CC} -c -o macosx-x86_64-debug/obj/mprPoll.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprPoll.c

${CC} -c -o macosx-x86_64-debug/obj/mprPrintf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprPrintf.c

${CC} -c -o macosx-x86_64-debug/obj/mprRomFile.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprRomFile.c

${CC} -c -o macosx-x86_64-debug/obj/mprSelect.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprSelect.c

${CC} -c -o macosx-x86_64-debug/obj/mprSignal.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprSignal.c

${CC} -c -o macosx-x86_64-debug/obj/mprSocket.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprSocket.c

${CC} -c -o macosx-x86_64-debug/obj/mprString.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprString.c

${CC} -c -o macosx-x86_64-debug/obj/mprTest.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprTest.c

${CC} -c -o macosx-x86_64-debug/obj/mprThread.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprThread.c

${CC} -c -o macosx-x86_64-debug/obj/mprTime.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprTime.c

${CC} -c -o macosx-x86_64-debug/obj/mprUnix.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprUnix.c

${CC} -c -o macosx-x86_64-debug/obj/mprVxworks.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprVxworks.c

${CC} -c -o macosx-x86_64-debug/obj/mprWait.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprWait.c

${CC} -c -o macosx-x86_64-debug/obj/mprWide.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprWide.c

${CC} -c -o macosx-x86_64-debug/obj/mprWin.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprWin.c

${CC} -c -o macosx-x86_64-debug/obj/mprWince.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprWince.c

${CC} -c -o macosx-x86_64-debug/obj/mprXml.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/mprXml.c

${CC} -dynamiclib -o macosx-x86_64-debug/lib/libmpr.dylib -arch x86_64 ${LDFLAGS} -install_name @rpath/libmpr.dylib macosx-x86_64-debug/obj/dtoa.o macosx-x86_64-debug/obj/mpr.o macosx-x86_64-debug/obj/mprAsync.o macosx-x86_64-debug/obj/mprAtomic.o macosx-x86_64-debug/obj/mprBuf.o macosx-x86_64-debug/obj/mprCache.o macosx-x86_64-debug/obj/mprCmd.o macosx-x86_64-debug/obj/mprCond.o macosx-x86_64-debug/obj/mprCrypt.o macosx-x86_64-debug/obj/mprDisk.o macosx-x86_64-debug/obj/mprDispatcher.o macosx-x86_64-debug/obj/mprEncode.o macosx-x86_64-debug/obj/mprEpoll.o macosx-x86_64-debug/obj/mprEvent.o macosx-x86_64-debug/obj/mprFile.o macosx-x86_64-debug/obj/mprFileSystem.o macosx-x86_64-debug/obj/mprHash.o macosx-x86_64-debug/obj/mprJSON.o macosx-x86_64-debug/obj/mprKqueue.o macosx-x86_64-debug/obj/mprList.o macosx-x86_64-debug/obj/mprLock.o macosx-x86_64-debug/obj/mprLog.o macosx-x86_64-debug/obj/mprMem.o macosx-x86_64-debug/obj/mprMime.o macosx-x86_64-debug/obj/mprMixed.o macosx-x86_64-debug/obj/mprModule.o macosx-x86_64-debug/obj/mprPath.o macosx-x86_64-debug/obj/mprPoll.o macosx-x86_64-debug/obj/mprPrintf.o macosx-x86_64-debug/obj/mprRomFile.o macosx-x86_64-debug/obj/mprSelect.o macosx-x86_64-debug/obj/mprSignal.o macosx-x86_64-debug/obj/mprSocket.o macosx-x86_64-debug/obj/mprString.o macosx-x86_64-debug/obj/mprTest.o macosx-x86_64-debug/obj/mprThread.o macosx-x86_64-debug/obj/mprTime.o macosx-x86_64-debug/obj/mprUnix.o macosx-x86_64-debug/obj/mprVxworks.o macosx-x86_64-debug/obj/mprWait.o macosx-x86_64-debug/obj/mprWide.o macosx-x86_64-debug/obj/mprWin.o macosx-x86_64-debug/obj/mprWince.o macosx-x86_64-debug/obj/mprXml.o ${LIBS}

${CC} -c -o macosx-x86_64-debug/obj/benchMpr.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/benchMpr.c

${CC} -o macosx-x86_64-debug/bin/benchMpr -arch x86_64 ${LDFLAGS} -Lmacosx-x86_64-debug/lib macosx-x86_64-debug/obj/benchMpr.o ${LIBS} -lmpr

${CC} -c -o macosx-x86_64-debug/obj/runProgram.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/runProgram.c

${CC} -o macosx-x86_64-debug/bin/runProgram -arch x86_64 ${LDFLAGS} -Lmacosx-x86_64-debug/lib macosx-x86_64-debug/obj/runProgram.o ${LIBS}

${CC} -c -o macosx-x86_64-debug/obj/mprMatrixssl.o -arch x86_64 ${CFLAGS} ${DFLAGS} -DPOSIX -DMATRIX_USE_FILE_SYSTEM -Imacosx-x86_64-debug/inc -Isrc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprMatrixssl.c

${CC} -c -o macosx-x86_64-debug/obj/mprOpenssl.o -arch x86_64 ${CFLAGS} ${DFLAGS} -DPOSIX -DMATRIX_USE_FILE_SYSTEM -Imacosx-x86_64-debug/inc -Isrc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprOpenssl.c

${CC} -c -o macosx-x86_64-debug/obj/mprSsl.o -arch x86_64 ${CFLAGS} ${DFLAGS} -DPOSIX -DMATRIX_USE_FILE_SYSTEM -Imacosx-x86_64-debug/inc -Isrc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprSsl.c

${CC} -dynamiclib -o macosx-x86_64-debug/lib/libmprssl.dylib -arch x86_64 ${LDFLAGS} -L/Users/mob/git/packages-macosx-x86_64/openssl/openssl-1.0.0d -L/Users/mob/git/packages-macosx-x86_64/matrixssl/matrixssl-3-3-open -install_name @rpath/libmprssl.dylib macosx-x86_64-debug/obj/mprMatrixssl.o macosx-x86_64-debug/obj/mprOpenssl.o macosx-x86_64-debug/obj/mprSsl.o ${LIBS} -lmpr -lssl -lcrypto -lmatrixssl

${CC} -c -o macosx-x86_64-debug/obj/testArgv.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testArgv.c

${CC} -c -o macosx-x86_64-debug/obj/testBuf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testBuf.c

${CC} -c -o macosx-x86_64-debug/obj/testCmd.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testCmd.c

${CC} -c -o macosx-x86_64-debug/obj/testCond.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testCond.c

${CC} -c -o macosx-x86_64-debug/obj/testEvent.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testEvent.c

${CC} -c -o macosx-x86_64-debug/obj/testFile.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testFile.c

${CC} -c -o macosx-x86_64-debug/obj/testHash.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testHash.c

${CC} -c -o macosx-x86_64-debug/obj/testList.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testList.c

${CC} -c -o macosx-x86_64-debug/obj/testLock.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testLock.c

${CC} -c -o macosx-x86_64-debug/obj/testMem.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testMem.c

${CC} -c -o macosx-x86_64-debug/obj/testMpr.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testMpr.c

${CC} -c -o macosx-x86_64-debug/obj/testPath.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testPath.c

${CC} -c -o macosx-x86_64-debug/obj/testSocket.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testSocket.c

${CC} -c -o macosx-x86_64-debug/obj/testSprintf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testSprintf.c

${CC} -c -o macosx-x86_64-debug/obj/testThread.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testThread.c

${CC} -c -o macosx-x86_64-debug/obj/testTime.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testTime.c

${CC} -c -o macosx-x86_64-debug/obj/testUnicode.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc test/testUnicode.c

${CC} -o macosx-x86_64-debug/bin/testMpr -arch x86_64 ${LDFLAGS} -L/Users/mob/git/packages-macosx-x86_64/openssl/openssl-1.0.0d -L/Users/mob/git/packages-macosx-x86_64/matrixssl/matrixssl-3-3-open -Lmacosx-x86_64-debug/lib macosx-x86_64-debug/obj/testArgv.o macosx-x86_64-debug/obj/testBuf.o macosx-x86_64-debug/obj/testCmd.o macosx-x86_64-debug/obj/testCond.o macosx-x86_64-debug/obj/testEvent.o macosx-x86_64-debug/obj/testFile.o macosx-x86_64-debug/obj/testHash.o macosx-x86_64-debug/obj/testList.o macosx-x86_64-debug/obj/testLock.o macosx-x86_64-debug/obj/testMem.o macosx-x86_64-debug/obj/testMpr.o macosx-x86_64-debug/obj/testPath.o macosx-x86_64-debug/obj/testSocket.o macosx-x86_64-debug/obj/testSprintf.o macosx-x86_64-debug/obj/testThread.o macosx-x86_64-debug/obj/testTime.o macosx-x86_64-debug/obj/testUnicode.o ${LIBS} -lmpr -lmprssl -lssl -lcrypto -lmatrixssl

${CC} -c -o macosx-x86_64-debug/obj/manager.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/manager.c

${CC} -o macosx-x86_64-debug/bin/manager -arch x86_64 ${LDFLAGS} -Lmacosx-x86_64-debug/lib macosx-x86_64-debug/obj/manager.o ${LIBS} -lmpr

${CC} -c -o macosx-x86_64-debug/obj/makerom.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/utils/makerom.c

${CC} -o macosx-x86_64-debug/bin/makerom -arch x86_64 ${LDFLAGS} -Lmacosx-x86_64-debug/lib macosx-x86_64-debug/obj/makerom.o ${LIBS} -lmpr

${CC} -c -o macosx-x86_64-debug/obj/charGen.o -arch x86_64 ${CFLAGS} ${DFLAGS} -Imacosx-x86_64-debug/inc -Isrc src/utils/charGen.c

${CC} -o macosx-x86_64-debug/bin/chargen -arch x86_64 ${LDFLAGS} -Lmacosx-x86_64-debug/lib macosx-x86_64-debug/obj/charGen.o ${LIBS} -lmpr

