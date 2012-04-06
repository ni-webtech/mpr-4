#
#   macosx-x86_64-debug.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

CONFIG="macosx-x86_64-debug"
CC="/usr/bin/cc"
LD="/usr/bin/ld"
CFLAGS="-fPIC -Wall -fast -Wshorten-64-to-32"
DFLAGS="-DPIC -DCPU=X86_64"
IFLAGS="-Imacosx-x86_64-debug/inc -Imacosx-x86_64-debug/inc"
LDFLAGS="-Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/"
LIBPATHS="-L${CONFIG}/lib"
LIBS="-lpthread -lm -ldl"

[ ! -x ${CONFIG}/inc ] && mkdir -p ${CONFIG}/inc ${CONFIG}/obj ${CONFIG}/lib ${CONFIG}/bin
cp projects/buildConfig.${CONFIG} ${CONFIG}/inc/buildConfig.h

rm -rf macosx-x86_64-debug/inc/mpr.h
cp -r src/mpr.h macosx-x86_64-debug/inc/mpr.h

${CC} -c -o ${CONFIG}/obj/dtoa.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/dtoa.c

${CC} -c -o ${CONFIG}/obj/mpr.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mpr.c

${CC} -c -o ${CONFIG}/obj/mprAsync.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprAsync.c

${CC} -c -o ${CONFIG}/obj/mprAtomic.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprAtomic.c

${CC} -c -o ${CONFIG}/obj/mprBuf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprBuf.c

${CC} -c -o ${CONFIG}/obj/mprCache.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprCache.c

${CC} -c -o ${CONFIG}/obj/mprCmd.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprCmd.c

${CC} -c -o ${CONFIG}/obj/mprCond.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprCond.c

${CC} -c -o ${CONFIG}/obj/mprCrypt.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprCrypt.c

${CC} -c -o ${CONFIG}/obj/mprDisk.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprDisk.c

${CC} -c -o ${CONFIG}/obj/mprDispatcher.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprDispatcher.c

${CC} -c -o ${CONFIG}/obj/mprEncode.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprEncode.c

${CC} -c -o ${CONFIG}/obj/mprEpoll.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprEpoll.c

${CC} -c -o ${CONFIG}/obj/mprEvent.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprEvent.c

${CC} -c -o ${CONFIG}/obj/mprFile.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprFile.c

${CC} -c -o ${CONFIG}/obj/mprFileSystem.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprFileSystem.c

${CC} -c -o ${CONFIG}/obj/mprHash.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprHash.c

${CC} -c -o ${CONFIG}/obj/mprJSON.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprJSON.c

${CC} -c -o ${CONFIG}/obj/mprKqueue.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprKqueue.c

${CC} -c -o ${CONFIG}/obj/mprList.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprList.c

${CC} -c -o ${CONFIG}/obj/mprLock.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprLock.c

${CC} -c -o ${CONFIG}/obj/mprLog.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprLog.c

${CC} -c -o ${CONFIG}/obj/mprMem.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprMem.c

${CC} -c -o ${CONFIG}/obj/mprMime.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprMime.c

${CC} -c -o ${CONFIG}/obj/mprMixed.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprMixed.c

${CC} -c -o ${CONFIG}/obj/mprModule.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprModule.c

${CC} -c -o ${CONFIG}/obj/mprPath.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprPath.c

${CC} -c -o ${CONFIG}/obj/mprPoll.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprPoll.c

${CC} -c -o ${CONFIG}/obj/mprPrintf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprPrintf.c

${CC} -c -o ${CONFIG}/obj/mprRomFile.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprRomFile.c

${CC} -c -o ${CONFIG}/obj/mprSelect.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprSelect.c

${CC} -c -o ${CONFIG}/obj/mprSignal.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprSignal.c

${CC} -c -o ${CONFIG}/obj/mprSocket.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprSocket.c

${CC} -c -o ${CONFIG}/obj/mprString.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprString.c

${CC} -c -o ${CONFIG}/obj/mprTest.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprTest.c

${CC} -c -o ${CONFIG}/obj/mprThread.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprThread.c

${CC} -c -o ${CONFIG}/obj/mprTime.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprTime.c

${CC} -c -o ${CONFIG}/obj/mprUnix.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprUnix.c

${CC} -c -o ${CONFIG}/obj/mprVxworks.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprVxworks.c

${CC} -c -o ${CONFIG}/obj/mprWait.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprWait.c

${CC} -c -o ${CONFIG}/obj/mprWide.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprWide.c

${CC} -c -o ${CONFIG}/obj/mprWin.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprWin.c

${CC} -c -o ${CONFIG}/obj/mprWince.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprWince.c

${CC} -c -o ${CONFIG}/obj/mprXml.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/mprXml.c

${CC} -dynamiclib -o ${CONFIG}/lib/libmpr.dylib -arch x86_64 ${LDFLAGS} ${LIBPATHS} -install_name @rpath/libmpr.dylib ${CONFIG}/obj/dtoa.o ${CONFIG}/obj/mpr.o ${CONFIG}/obj/mprAsync.o ${CONFIG}/obj/mprAtomic.o ${CONFIG}/obj/mprBuf.o ${CONFIG}/obj/mprCache.o ${CONFIG}/obj/mprCmd.o ${CONFIG}/obj/mprCond.o ${CONFIG}/obj/mprCrypt.o ${CONFIG}/obj/mprDisk.o ${CONFIG}/obj/mprDispatcher.o ${CONFIG}/obj/mprEncode.o ${CONFIG}/obj/mprEpoll.o ${CONFIG}/obj/mprEvent.o ${CONFIG}/obj/mprFile.o ${CONFIG}/obj/mprFileSystem.o ${CONFIG}/obj/mprHash.o ${CONFIG}/obj/mprJSON.o ${CONFIG}/obj/mprKqueue.o ${CONFIG}/obj/mprList.o ${CONFIG}/obj/mprLock.o ${CONFIG}/obj/mprLog.o ${CONFIG}/obj/mprMem.o ${CONFIG}/obj/mprMime.o ${CONFIG}/obj/mprMixed.o ${CONFIG}/obj/mprModule.o ${CONFIG}/obj/mprPath.o ${CONFIG}/obj/mprPoll.o ${CONFIG}/obj/mprPrintf.o ${CONFIG}/obj/mprRomFile.o ${CONFIG}/obj/mprSelect.o ${CONFIG}/obj/mprSignal.o ${CONFIG}/obj/mprSocket.o ${CONFIG}/obj/mprString.o ${CONFIG}/obj/mprTest.o ${CONFIG}/obj/mprThread.o ${CONFIG}/obj/mprTime.o ${CONFIG}/obj/mprUnix.o ${CONFIG}/obj/mprVxworks.o ${CONFIG}/obj/mprWait.o ${CONFIG}/obj/mprWide.o ${CONFIG}/obj/mprWin.o ${CONFIG}/obj/mprWince.o ${CONFIG}/obj/mprXml.o ${LIBS}

${CC} -c -o ${CONFIG}/obj/benchMpr.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/benchMpr.c

${CC} -o ${CONFIG}/bin/benchMpr -arch x86_64 ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/benchMpr.o ${LIBS} -lmpr

${CC} -c -o ${CONFIG}/obj/runProgram.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/runProgram.c

${CC} -o ${CONFIG}/bin/runProgram -arch x86_64 ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/runProgram.o ${LIBS}

${CC} -c -o ${CONFIG}/obj/mprMatrixssl.o -arch x86_64 ${CFLAGS} ${DFLAGS} -DPOSIX -DMATRIX_USE_FILE_SYSTEM -I${CONFIG}/inc -I${CONFIG}/inc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprMatrixssl.c

${CC} -c -o ${CONFIG}/obj/mprOpenssl.o -arch x86_64 ${CFLAGS} ${DFLAGS} -DPOSIX -DMATRIX_USE_FILE_SYSTEM -I${CONFIG}/inc -I${CONFIG}/inc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprOpenssl.c

${CC} -c -o ${CONFIG}/obj/mprSsl.o -arch x86_64 ${CFLAGS} ${DFLAGS} -DPOSIX -DMATRIX_USE_FILE_SYSTEM -I${CONFIG}/inc -I${CONFIG}/inc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprSsl.c

${CC} -dynamiclib -o ${CONFIG}/lib/libmprssl.dylib -arch x86_64 ${LDFLAGS} ${LIBPATHS} -L/Users/mob/git/packages-macosx-x86_64/openssl/openssl-1.0.0d -L/Users/mob/git/packages-macosx-x86_64/matrixssl/matrixssl-3-3-open -install_name @rpath/libmprssl.dylib ${CONFIG}/obj/mprMatrixssl.o ${CONFIG}/obj/mprOpenssl.o ${CONFIG}/obj/mprSsl.o ${LIBS} -lmpr -lssl -lcrypto -lmatrixssl

${CC} -c -o ${CONFIG}/obj/testArgv.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testArgv.c

${CC} -c -o ${CONFIG}/obj/testBuf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testBuf.c

${CC} -c -o ${CONFIG}/obj/testCmd.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testCmd.c

${CC} -c -o ${CONFIG}/obj/testCond.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testCond.c

${CC} -c -o ${CONFIG}/obj/testEvent.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testEvent.c

${CC} -c -o ${CONFIG}/obj/testFile.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testFile.c

${CC} -c -o ${CONFIG}/obj/testHash.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testHash.c

${CC} -c -o ${CONFIG}/obj/testList.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testList.c

${CC} -c -o ${CONFIG}/obj/testLock.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testLock.c

${CC} -c -o ${CONFIG}/obj/testMem.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testMem.c

${CC} -c -o ${CONFIG}/obj/testMpr.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testMpr.c

${CC} -c -o ${CONFIG}/obj/testPath.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testPath.c

${CC} -c -o ${CONFIG}/obj/testSocket.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testSocket.c

${CC} -c -o ${CONFIG}/obj/testSprintf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testSprintf.c

${CC} -c -o ${CONFIG}/obj/testThread.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testThread.c

${CC} -c -o ${CONFIG}/obj/testTime.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testTime.c

${CC} -c -o ${CONFIG}/obj/testUnicode.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc test/testUnicode.c

${CC} -o ${CONFIG}/bin/testMpr -arch x86_64 ${LDFLAGS} ${LIBPATHS} -L/Users/mob/git/packages-macosx-x86_64/openssl/openssl-1.0.0d -L/Users/mob/git/packages-macosx-x86_64/matrixssl/matrixssl-3-3-open ${CONFIG}/obj/testArgv.o ${CONFIG}/obj/testBuf.o ${CONFIG}/obj/testCmd.o ${CONFIG}/obj/testCond.o ${CONFIG}/obj/testEvent.o ${CONFIG}/obj/testFile.o ${CONFIG}/obj/testHash.o ${CONFIG}/obj/testList.o ${CONFIG}/obj/testLock.o ${CONFIG}/obj/testMem.o ${CONFIG}/obj/testMpr.o ${CONFIG}/obj/testPath.o ${CONFIG}/obj/testSocket.o ${CONFIG}/obj/testSprintf.o ${CONFIG}/obj/testThread.o ${CONFIG}/obj/testTime.o ${CONFIG}/obj/testUnicode.o ${LIBS} -lmpr -lmprssl -lssl -lcrypto -lmatrixssl

${CC} -c -o ${CONFIG}/obj/manager.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/manager.c

${CC} -o ${CONFIG}/bin/manager -arch x86_64 ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/manager.o ${LIBS} -lmpr

${CC} -c -o ${CONFIG}/obj/makerom.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/utils/makerom.c

${CC} -o ${CONFIG}/bin/makerom -arch x86_64 ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/makerom.o ${LIBS} -lmpr

${CC} -c -o ${CONFIG}/obj/charGen.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${CONFIG}/inc -I${CONFIG}/inc src/utils/charGen.c

${CC} -o ${CONFIG}/bin/chargen -arch x86_64 ${LDFLAGS} ${LIBPATHS} ${CONFIG}/obj/charGen.o ${LIBS} -lmpr

