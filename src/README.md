Multithreaded Portable Runtime (MPR)
====================================

The Multithreaded Portable Runtime provides a high performance, multithreaded, cross-platform abstraction to hide per operating/system dependencies. Implemented in "C", it can be used in both C and C++ programs. It implements an interface that is sufficient to support event based programs and does not try to be a complete cross-platform API.

Key Directories
---------------

* samples         - Sample source code for the MPR.
* test            - MPR unit tests.
* UNIX            - UNIX specific code.
* WIN             - Windows specific code.


Key Files
---------
* Makefile        - MPR Makefile.
* manager.c       - Source for the Manager (watchdog) program.
* mpr.h           - Primary MPR header.
* mprSsl.h
* mpr.c
* mprAsync.c
* mprAtomic.c
* mprBuf.c           - The buffer class. Used for expandable ring queues.
* mprCache.c      - Safe embedded routines including safe string handling.
* mprCmd.c
* mprCond.c
* mprCrypt.c
* mprDisk.c
* mprDispatcher.c
* mprEncode.c
* mprEvent.c 
* mprFile.c       - Virtual file system class.
* mprFileSystem.c - 
* mprHash.c       - The Hash class. Used for general hash indexing.
* mprKqueue.c     -
* mprList.c       - List base class. Used extensively by factory objects. 
* mprLock.c
* mprLog.c        - MPR logging and debug trace class.
* mprMem.c
* mprMime.c
* mprMixed.c
* mprModule.c
* mprPath.c
* mprPoll.c
* mprPrintf.c
* mprRomFile.c
* mprSelect.c     - Posix select
* mprSignal
* mprSocket.c
* mprString.c
* mprTest.c
* mprThread.c
* mprTime.c
* mprUnix.c
* mprVxWorks.c
* mprWait.c
* mprWide.c
* mprWin.c
* mprWince.c
* mprXml.c

WIN/UNIX/MACOSX
daemon.c        - Code to run MPR as a system service / daemon
os.c            - O/S dependent code
thread.c        - Thread handling code

--------------------------------------------------------------------------------

Copyright (c) 2003-2012 Embedthis Software, LLC. All Rights Reserved.
Embedthis and AppWeb are trademarks of Embedthis Software, LLC. Other 
brands and their products are trademarks of their respective holders.

See LICENSE.TXT for software license details.
