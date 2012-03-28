#
#   macosx-x86_64-debug.sh -- Build It Shell Script to build Multithreaded Portable Runtime
#

PLATFORM="macosx-x86_64-debug"
CC="cc"
LD="ld"
CFLAGS="-fPIC -Wall -g"
DFLAGS="-DPIC -DCPU=X86_64"
IFLAGS="-Imacosx-x86_64-debug/inc"
LDFLAGS="-Wl,-rpath,@executable_path/../lib -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -L${PLATFORM}/lib -g -ldl"
LIBS="-lpthread -lm"

[ ! -x ${PLATFORM}/inc ] && mkdir -p ${PLATFORM}/inc ${PLATFORM}/obj ${PLATFORM}/lib ${PLATFORM}/bin
[ ! -f ${PLATFORM}/inc/buildConfig.h ] && cp projects/buildConfig.${PLATFORM} ${PLATFORM}/inc/buildConfig.h

rm -rf macosx-x86_64-debug/inc/mpr.h
cp -r src/mpr.h macosx-x86_64-debug/inc/mpr.h

${CC} -c -o ${PLATFORM}/obj/dtoa.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/dtoa.c

${CC} -c -o ${PLATFORM}/obj/mpr.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mpr.c

${CC} -c -o ${PLATFORM}/obj/mprAsync.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprAsync.c

${CC} -c -o ${PLATFORM}/obj/mprAtomic.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprAtomic.c

${CC} -c -o ${PLATFORM}/obj/mprBuf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprBuf.c

${CC} -c -o ${PLATFORM}/obj/mprCache.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprCache.c

${CC} -c -o ${PLATFORM}/obj/mprCmd.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprCmd.c

${CC} -c -o ${PLATFORM}/obj/mprCond.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprCond.c

${CC} -c -o ${PLATFORM}/obj/mprCrypt.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprCrypt.c

${CC} -c -o ${PLATFORM}/obj/mprDisk.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprDisk.c

${CC} -c -o ${PLATFORM}/obj/mprDispatcher.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprDispatcher.c

${CC} -c -o ${PLATFORM}/obj/mprEncode.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprEncode.c

${CC} -c -o ${PLATFORM}/obj/mprEpoll.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprEpoll.c

${CC} -c -o ${PLATFORM}/obj/mprEvent.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprEvent.c

${CC} -c -o ${PLATFORM}/obj/mprFile.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprFile.c

${CC} -c -o ${PLATFORM}/obj/mprFileSystem.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprFileSystem.c

${CC} -c -o ${PLATFORM}/obj/mprHash.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprHash.c

${CC} -c -o ${PLATFORM}/obj/mprJSON.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprJSON.c

${CC} -c -o ${PLATFORM}/obj/mprKqueue.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprKqueue.c

${CC} -c -o ${PLATFORM}/obj/mprList.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprList.c

${CC} -c -o ${PLATFORM}/obj/mprLock.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprLock.c

${CC} -c -o ${PLATFORM}/obj/mprLog.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprLog.c

${CC} -c -o ${PLATFORM}/obj/mprMem.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprMem.c

${CC} -c -o ${PLATFORM}/obj/mprMime.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprMime.c

${CC} -c -o ${PLATFORM}/obj/mprMixed.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprMixed.c

${CC} -c -o ${PLATFORM}/obj/mprModule.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprModule.c

${CC} -c -o ${PLATFORM}/obj/mprPath.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprPath.c

${CC} -c -o ${PLATFORM}/obj/mprPoll.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprPoll.c

${CC} -c -o ${PLATFORM}/obj/mprPrintf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprPrintf.c

${CC} -c -o ${PLATFORM}/obj/mprRomFile.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprRomFile.c

${CC} -c -o ${PLATFORM}/obj/mprSelect.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprSelect.c

${CC} -c -o ${PLATFORM}/obj/mprSignal.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprSignal.c

${CC} -c -o ${PLATFORM}/obj/mprSocket.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprSocket.c

${CC} -c -o ${PLATFORM}/obj/mprString.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprString.c

${CC} -c -o ${PLATFORM}/obj/mprTest.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprTest.c

${CC} -c -o ${PLATFORM}/obj/mprThread.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprThread.c

${CC} -c -o ${PLATFORM}/obj/mprTime.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprTime.c

${CC} -c -o ${PLATFORM}/obj/mprUnix.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprUnix.c

${CC} -c -o ${PLATFORM}/obj/mprVxworks.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprVxworks.c

${CC} -c -o ${PLATFORM}/obj/mprWait.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprWait.c

${CC} -c -o ${PLATFORM}/obj/mprWide.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprWide.c

${CC} -c -o ${PLATFORM}/obj/mprWin.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprWin.c

${CC} -c -o ${PLATFORM}/obj/mprWince.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprWince.c

${CC} -c -o ${PLATFORM}/obj/mprXml.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/mprXml.c

${CC} -dynamiclib -o ${PLATFORM}/lib/libmpr.dylib -arch x86_64 ${LDFLAGS} -install_name @rpath/libmpr.dylib ${PLATFORM}/obj/dtoa.o ${PLATFORM}/obj/mpr.o ${PLATFORM}/obj/mprAsync.o ${PLATFORM}/obj/mprAtomic.o ${PLATFORM}/obj/mprBuf.o ${PLATFORM}/obj/mprCache.o ${PLATFORM}/obj/mprCmd.o ${PLATFORM}/obj/mprCond.o ${PLATFORM}/obj/mprCrypt.o ${PLATFORM}/obj/mprDisk.o ${PLATFORM}/obj/mprDispatcher.o ${PLATFORM}/obj/mprEncode.o ${PLATFORM}/obj/mprEpoll.o ${PLATFORM}/obj/mprEvent.o ${PLATFORM}/obj/mprFile.o ${PLATFORM}/obj/mprFileSystem.o ${PLATFORM}/obj/mprHash.o ${PLATFORM}/obj/mprJSON.o ${PLATFORM}/obj/mprKqueue.o ${PLATFORM}/obj/mprList.o ${PLATFORM}/obj/mprLock.o ${PLATFORM}/obj/mprLog.o ${PLATFORM}/obj/mprMem.o ${PLATFORM}/obj/mprMime.o ${PLATFORM}/obj/mprMixed.o ${PLATFORM}/obj/mprModule.o ${PLATFORM}/obj/mprPath.o ${PLATFORM}/obj/mprPoll.o ${PLATFORM}/obj/mprPrintf.o ${PLATFORM}/obj/mprRomFile.o ${PLATFORM}/obj/mprSelect.o ${PLATFORM}/obj/mprSignal.o ${PLATFORM}/obj/mprSocket.o ${PLATFORM}/obj/mprString.o ${PLATFORM}/obj/mprTest.o ${PLATFORM}/obj/mprThread.o ${PLATFORM}/obj/mprTime.o ${PLATFORM}/obj/mprUnix.o ${PLATFORM}/obj/mprVxworks.o ${PLATFORM}/obj/mprWait.o ${PLATFORM}/obj/mprWide.o ${PLATFORM}/obj/mprWin.o ${PLATFORM}/obj/mprWince.o ${PLATFORM}/obj/mprXml.o ${LIBS}

${CC} -c -o ${PLATFORM}/obj/benchMpr.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/benchMpr.c

${CC} -o ${PLATFORM}/bin/benchMpr -arch x86_64 ${LDFLAGS} -L${PLATFORM}/lib ${PLATFORM}/obj/benchMpr.o ${LIBS} -lmpr

${CC} -c -o ${PLATFORM}/obj/runProgram.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/runProgram.c

${CC} -o ${PLATFORM}/bin/runProgram -arch x86_64 ${LDFLAGS} -L${PLATFORM}/lib ${PLATFORM}/obj/runProgram.o ${LIBS}

${CC} -c -o ${PLATFORM}/obj/mprMatrixssl.o -arch x86_64 ${CFLAGS} ${DFLAGS} -DPOSIX -DMATRIX_USE_FILE_SYSTEM -I${PLATFORM}/inc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprMatrixssl.c

${CC} -c -o ${PLATFORM}/obj/mprOpenssl.o -arch x86_64 ${CFLAGS} ${DFLAGS} -DPOSIX -DMATRIX_USE_FILE_SYSTEM -I${PLATFORM}/inc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprOpenssl.c

${CC} -c -o ${PLATFORM}/obj/mprSsl.o -arch x86_64 ${CFLAGS} ${DFLAGS} -DPOSIX -DMATRIX_USE_FILE_SYSTEM -I${PLATFORM}/inc -I../packages-macosx-x86_64/openssl/openssl-1.0.0d/include -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open/matrixssl -I../packages-macosx-x86_64/matrixssl/matrixssl-3-3-open src/mprSsl.c

${CC} -dynamiclib -o ${PLATFORM}/lib/libmprssl.dylib -arch x86_64 ${LDFLAGS} -L/Users/mob/git/packages-macosx-x86_64/openssl/openssl-1.0.0d -L/Users/mob/git/packages-macosx-x86_64/matrixssl/matrixssl-3-3-open -install_name @rpath/libmprssl.dylib ${PLATFORM}/obj/mprMatrixssl.o ${PLATFORM}/obj/mprOpenssl.o ${PLATFORM}/obj/mprSsl.o ${LIBS} -lmpr -lssl -lcrypto -lmatrixssl

${CC} -c -o ${PLATFORM}/obj/testArgv.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testArgv.c

${CC} -c -o ${PLATFORM}/obj/testBuf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testBuf.c

${CC} -c -o ${PLATFORM}/obj/testCmd.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testCmd.c

${CC} -c -o ${PLATFORM}/obj/testCond.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testCond.c

${CC} -c -o ${PLATFORM}/obj/testEvent.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testEvent.c

${CC} -c -o ${PLATFORM}/obj/testFile.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testFile.c

${CC} -c -o ${PLATFORM}/obj/testHash.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testHash.c

${CC} -c -o ${PLATFORM}/obj/testList.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testList.c

${CC} -c -o ${PLATFORM}/obj/testLock.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testLock.c

${CC} -c -o ${PLATFORM}/obj/testMem.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testMem.c

${CC} -c -o ${PLATFORM}/obj/testMpr.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testMpr.c

${CC} -c -o ${PLATFORM}/obj/testPath.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testPath.c

${CC} -c -o ${PLATFORM}/obj/testSocket.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testSocket.c

${CC} -c -o ${PLATFORM}/obj/testSprintf.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testSprintf.c

${CC} -c -o ${PLATFORM}/obj/testThread.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testThread.c

${CC} -c -o ${PLATFORM}/obj/testTime.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testTime.c

${CC} -c -o ${PLATFORM}/obj/testUnicode.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc test/testUnicode.c

${CC} -o ${PLATFORM}/bin/testMpr -arch x86_64 ${LDFLAGS} -L/Users/mob/git/packages-macosx-x86_64/openssl/openssl-1.0.0d -L/Users/mob/git/packages-macosx-x86_64/matrixssl/matrixssl-3-3-open -L${PLATFORM}/lib ${PLATFORM}/obj/testArgv.o ${PLATFORM}/obj/testBuf.o ${PLATFORM}/obj/testCmd.o ${PLATFORM}/obj/testCond.o ${PLATFORM}/obj/testEvent.o ${PLATFORM}/obj/testFile.o ${PLATFORM}/obj/testHash.o ${PLATFORM}/obj/testList.o ${PLATFORM}/obj/testLock.o ${PLATFORM}/obj/testMem.o ${PLATFORM}/obj/testMpr.o ${PLATFORM}/obj/testPath.o ${PLATFORM}/obj/testSocket.o ${PLATFORM}/obj/testSprintf.o ${PLATFORM}/obj/testThread.o ${PLATFORM}/obj/testTime.o ${PLATFORM}/obj/testUnicode.o ${LIBS} -lmpr -lmprssl -lssl -lcrypto -lmatrixssl

${CC} -c -o ${PLATFORM}/obj/manager.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/manager.c

${CC} -o ${PLATFORM}/bin/manager -arch x86_64 ${LDFLAGS} -L${PLATFORM}/lib ${PLATFORM}/obj/manager.o ${LIBS} -lmpr

${CC} -c -o ${PLATFORM}/obj/makerom.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/utils/makerom.c

${CC} -o ${PLATFORM}/bin/makerom -arch x86_64 ${LDFLAGS} -L${PLATFORM}/lib ${PLATFORM}/obj/makerom.o ${LIBS} -lmpr

${CC} -c -o ${PLATFORM}/obj/charGen.o -arch x86_64 ${CFLAGS} ${DFLAGS} -I${PLATFORM}/inc src/utils/charGen.c

${CC} -o ${PLATFORM}/bin/chargen -arch x86_64 ${LDFLAGS} -L${PLATFORM}/lib ${PLATFORM}/obj/charGen.o ${LIBS} -lmpr

