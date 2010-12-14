/*
    mprOs.h -- Include O/S headers and smooth out per-O/S differences

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/******************************* Documentation ********************************/

/*
    This header is part of the Multithreaded Portable Runtime and aims to include
    all necessary O/S headers and to unify the constants and declarations 
    required by Embedthis products. It can be included by C or C++ programs.
 */

/******************************************************************************/

#ifndef _h_MPR_OS_HDRS
#define _h_MPR_OS_HDRS 1

#include    "buildConfig.h"

/********************************* CPU Families *******************************/
/*
    CPU families
 */
#define MPR_CPU_UNKNOWN     0
#define MPR_CPU_IX86        1           /* X86 */
#define MPR_CPU_PPC         2           /* Power PC */
#define MPR_CPU_SPARC       3           /* Sparc */
#define MPR_CPU_XSCALE      4           /* XScale */
#define MPR_CPU_ARM         5           /* Arm */
#define MPR_CPU_MIPS        6           /* Mips */
#define MPR_CPU_68K         7           /* Motorola 68000 */
#define MPR_CPU_SIMNT       8           /* VxWorks NT simulator */
#define MPR_CPU_SIMSPARC    9           /* VxWorks sparc simulator */
#define MPR_CPU_IX64        10          /* AMD64 or EMT64 */
#define MPR_CPU_UNIVERSAL   11          /* MAC OS X universal binaries */
#define MPR_CPU_SH4         12

/********************************* O/S Includes *******************************/

#if WIN
    #define     _CRT_SECURE_NO_DEPRECATE 1
    #ifndef     _WIN32_WINNT
        #define _WIN32_WINNT 0x501
    #endif
#endif

#if BLD_WIN_LIKE
    #include    <winsock2.h>
    #include    <windows.h>
    #include    <winbase.h>
    #include    <winuser.h>
    #include    <shlobj.h>
    #include    <shellapi.h>
    #include    <wincrypt.h>
#endif
#if WIN
    #include    <ws2tcpip.h>
    #include    <conio.h>
    #include    <process.h>
    #include    <windows.h>
    #include    <shlobj.h>
    #if BLD_DEBUG
        #include <crtdbg.h>
    #endif
#endif
#undef     _WIN32_WINNT

#if UNUSED
    #include    <arpa/inet.h>
#endif
    #include    <ctype.h>
#if BLD_WIN_LIKE
    #include    <direct.h>
#else
    #include    <dirent.h>
    #include    <dlfcn.h>
#endif
    #include    <fcntl.h>
    #include    <errno.h>
#if BLD_FEATURE_FLOAT || 1
    #include    <float.h>
    #define __USE_ISOC99 1
    #include    <math.h>
#endif
#if !BLD_WIN_LIKE
    #include    <grp.h> 
#endif
#if BLD_WIN_LIKE
    #include    <io.h>
#endif
#if MACOSX
    #include    <libgen.h>
#endif
    #include    <limits.h>
#if UNUSED && BLD_WIN_LIKE
    #include    <malloc.h>
#endif
#if !BLD_WIN_LIKE
    #include    <netdb.h>
    #include    <net/if.h>
    #include    <netinet/in.h>
    #include    <netinet/tcp.h>
    #include    <netinet/ip.h>
    #include    <pthread.h> 
    #include    <pwd.h> 
#if !CYGWIN
    #include    <resolv.h>
#endif
#endif
    #include    <setjmp.h>
    #include    <signal.h>
    #include    <stdarg.h>
#if UNUSED && WINCE
    #include    <stddef.h>
#endif
#if BLD_UNIX_LIKE
    #include    <stdint.h>
#endif
    #include    <stdio.h>
    #include    <stdlib.h>
    #include    <string.h>
#if BLD_UNIX_LIKE
    #include    <syslog.h>
#endif
#if LINUX
    #include    <sys/epoll.h>
#endif
#if !BLD_WIN_LIKE
    #include    <sys/ioctl.h>
    #include    <sys/mman.h>
#if UNUSED && MACOSX
    #include    <sys/param.h>
#endif
    #include    <sys/poll.h>
#endif
    #include    <sys/stat.h>
#if LINUX
    #include    <sys/prctl.h>
#endif
    #include    <sys/types.h>
#if !BLD_WIN_LIKE
    #include    <sys/resource.h>
    #include    <sys/sem.h>
#if UNUSED
    #include    <sys/shm.h>
#endif
    #include    <sys/socket.h>
    #include    <sys/select.h>
    #include    <sys/time.h>
    #include    <sys/times.h>
    #include    <sys/utsname.h>
    #include    <sys/uio.h>
#if UNUSED
    #include    <sys/un.h>
#endif
    #include    <sys/wait.h>
#endif
    #include    <time.h>
#if !BLD_WIN_LIKE
    #include    <unistd.h>
#endif
#if UNUSED && LINUX
    #include    <values.h>
#endif
    #include    <wchar.h>
#if LINUX && !__UCLIBC__
    #include    <sys/sendfile.h>
#endif

#if UNUSED && FREEBSD
    #include    <netinet/in_systm.h>
#endif

#if MACOSX
    #include    <mach-o/dyld.h>
    #include    <sys/sysctl.h>
    #include    <libkern/OSAtomic.h>
#endif

#if VXWORKS
    #include    <vxWorks.h>
    #include    <envLib.h>
    #include    <iosLib.h>
    #include    <loadLib.h>
    #include    <selectLib.h>
    #include    <sockLib.h>
    #include    <inetLib.h>
    #include    <ioLib.h>
    #include    <pipeDrv.h>
    #include    <hostLib.h>
    #include    <symSyncLib.h>
    #include    <sysSymTbl.h>
    #include    <sys/fcntlcom.h>
    #include    <tickLib.h>
    #include    <taskHookLib.h>
    #include    <unldLib.h>
#if _WRS_VXWORKS_MAJOR >= 6
    #include    <wait.h>
#endif
#endif /* VXWORKS */

/************************************** Defines *******************************/
/*
    Standard types
 */
#if __WORDSIZE == 64 || __amd64 || __x86_64 || __x86_64__ || _WIN64
    #define MPR_64_BIT 1
#else
    #define MPR_64_BIT 0
#endif

#if !MACOSX
    typedef int bool;
#endif

typedef unsigned char uchar;
typedef signed char schar;
typedef const char cchar;
typedef const unsigned char cuchar;
typedef unsigned short ushort;
typedef const unsigned short cushort;
typedef const void cvoid;
typedef int int32;
typedef unsigned int uint32;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef off_t MprOffset;

#if BLD_UNIX_LIKE
    typedef ssize_t MprSize;
    __extension__ typedef long long int int64;
    __extension__ typedef unsigned long long int uint64;
#elif BLD_WIN_LIKE
    typedef SSIZE_T MprSize;
    typedef __int64 int64;
    typedef unsigned __int64 uint64;
#else
    typedef long long int int64;
    typedef unsigned long long int uint64;
#endif

/* To stop MatrixSSL from defining int32 */
#define HAS_INT32 1

#ifndef BITSPERBYTE
    #define BITSPERBYTE     (8 * sizeof(char))
#endif
#ifndef BITS
    #define BITS(type)      (BITSPERBYTE * (int) sizeof(type))
#endif

#if BLD_FEATURE_FLOAT
    #ifndef MAXFLOAT
        #if BLD_WIN_LIKE
            #define MAXFLOAT        DBL_MAX
        #else
            #define MAXFLOAT        FLT_MAX
        #endif
    #endif
    #if BLD_WIN_LIKE
        #define isNan(f) (_isnan(f))
    #else
        #define isNan(f) (f == FP_NAN)
    #endif
#endif


#ifndef MAXINT
#if INT_MAX
    #define MAXINT      INT_MAX
#else
    #define MAXINT      0x7fffffff
#endif
#endif

#if SIZE_T_MAX
    #define MAXSIZE     SIZE_T_MAX
#elif MPR_64_BIT
    #define MAXSIZE     INT64(0x7fffffffffffffff)
#else
    #define MAXSIZE     MAXINT
#endif
#ifndef MAXINT64
    #define MAXINT64    INT64(0x7fffffffffffffff)
#endif

/*
    Word size and conversions between integer and pointer.
 */
#if MPR_64_BIT
    #define ITOP(i)     ((void*) ((int64) i))
    #define PTOI(i)     ((int) ((int64) i))
    #define LTOP(i)     ((void*) ((int64) i))
    #define PTOL(i)     ((int64) i)
#else
    #define ITOP(i)     ((void*) ((int) i))
    #define PTOI(i)     ((int) i)
    #define LTOP(i)     ((void*) ((int) i))
    #define PTOL(i)     ((int64) (int) i)
#endif

#if BLD_WIN_LIKE
    #define INT64(x)    (x##i64)
    #define UINT64(x)   (x##Ui64)
#else
    #define INT64(x)    (x##LL)
    #define UINT64(x)   (x##ULL)
#endif

#ifndef max
    #define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
    #define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef PRINTF_ATTRIBUTE
    #if (__GNUC__ >= 3) && !DOXYGEN && BLD_DEBUG && UNUSED
        /** 
            Use gcc attribute to check printf fns.  a1 is the 1-based index of the parameter containing the format, 
            and a2 the index of the first argument. Note that some gcc 2.x versions don't handle this properly 
         */     
        #define PRINTF_ATTRIBUTE(a1, a2) __attribute__ ((format (__printf__, a1, a2)))
    #else
        #define PRINTF_ATTRIBUTE(a1, a2)
    #endif
#endif

/*
    Optimize expression evaluation code depending if the value is likely or not
 */
#undef likely
#undef unlikely
#if (__GNUC__ >= 3)
    #define likely(x)   __builtin_expect(!!(x), 1)
    #define unlikely(x) __builtin_expect(!!(x), 0)
#else
    #define likely(x)   (x)
    #define unlikely(x) (x)
#endif

#if !__UCLIBC__ && !CYGWIN && __USE_XOPEN2K
    #define BLD_HAS_SPINLOCK    1
#endif

#if BLD_CC_DOUBLE_BRACES
    #define  NULL_INIT    {{0}}
#else
    #define  NULL_INIT    {0}
#endif

#ifndef R_OK
    #define R_OK    4
    #define W_OK    2
#if BLD_WIN_LIKE
    #define X_OK    R_OK
#else
    #define X_OK    1
#endif
    #define F_OK    0
#endif

#if MACSOX
    #define LD_LIBRARY_PATH "DYLD_LIBRARY_PATH"
#else
    #define LD_LIBRARY_PATH "LD_LIBRARY_PATH"
#endif

/*********************************** Fixups ***********************************/

#if BLD_UNIX_LIKE
    #define closesocket(x)  close(x)
    #define MPR_BINARY      ""
    #define MPR_TEXT        ""
    #define O_BINARY        0
    #define O_TEXT          0
    #define SOCKET_ERROR    -1
    #ifndef PTHREAD_MUTEX_RECURSIVE_NP
        #define PTHREAD_MUTEX_RECURSIVE_NP PTHREAD_MUTEX_RECURSIVE
    #endif
    #ifndef PTHREAD_MUTEX_RECURSIVE
        #define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
    #endif
#endif

#if MACOSX || VXWORKS || CYGWIN || BLD_WIN_LIKE
    #define __WALL          0
    #define MSG_NOSIGNAL    0
#endif

#if FREEBSD
    #if UNUSED
        #define CLD_EXITED 1
        #define CLD_KILLED 2
    #endif
#endif /* FREEBSD */

#if MACOSX
    /*
        Fix for MAC OS X - getenv
     */
    #if !HAVE_DECL_ENVIRON
        #ifdef __APPLE__
            #include <crt_externs.h>
            #define environ (*_NSGetEnviron())
        #else
            extern char **environ;
        #endif
    #endif
#endif

#if SOLARIS
    #define INADDR_NONE     ((in_addr_t) 0xffffffff)
#endif

#if VXWORKS
    #ifndef SHUT_RDWR
        #define SHUT_RDWR 2
    #endif

    #define HAVE_SOCKLEN_T
    #define getpid mprGetpid

    #if _DIAB_TOOL
        #define inline __inline__
    #endif
#endif

#if BLD_WIN_LIKE
    typedef int     uid_t;
    typedef void    *handle;
    typedef char    *caddr_t;
    typedef long    pid_t;
    typedef int     gid_t;
    typedef ushort  mode_t;
    typedef void    *siginfo_t;
    typedef int     socklen_t;

    #define HAVE_SOCKLEN_T
    #define MSG_NOSIGNAL    0
    #define MPR_BINARY      "b"
    #define MPR_TEXT        "t"

    /*
        Error codes 
     */
    #define EPERM           1
    #define ENOENT          2
    #define ESRCH           3
    #define EINTR           4
    #define EIO             5
    #define ENXIO           6
    #define E2BIG           7
    #define ENOEXEC         8
    #define EBADF           9
    #define ECHILD          10
    #define EAGAIN          11
    #define ENOMEM          12
    #define EACCES          13
    #define EFAULT          14
    #define EOSERR          15
    #define EBUSY           16
    #define EEXIST          17
    #define EXDEV           18
    #define ENODEV          19
    #define ENOTDIR         20
    #define EISDIR          21
    #define EINVAL          22
    #define ENFILE          23
    #define EMFILE          24
    #define ENOTTY          25
    #define EFBIG           27
    #define ENOSPC          28
    #define ESPIPE          29
    #define EROFS           30
    #define EMLINK          31
    #define EPIPE           32
    #define EDOM            33
    #define ERANGE          34

    #ifndef EWOULDBLOCK
    #define EWOULDBLOCK     EAGAIN
    #define EINPROGRESS     36
    #define EALREADY        37
    #define ENETDOWN        43
    #define ECONNRESET      44
    #define ECONNREFUSED    45
    #define EADDRNOTAVAIL   49
    #define EISCONN         56
    #define EADDRINUSE      46
    #define ENETUNREACH     51
    #define ECONNABORTED    53
    #endif

    #undef SHUT_RDWR
    #define SHUT_RDWR       2
    
#if UNUSED
    #ifndef FILE_FLAG_FIRST_PIPE_INSTANCE
        #define FILE_FLAG_FIRST_PIPE_INSTANCE   0x00080000
    #endif
#endif
    #define TIME_GENESIS UINT64(11644473600000000)
    #define va_copy(d, s) ((d) = (s))

    #if !WINCE
    #define access      _access
    #define chdir       _chdir
    #define chmod       _chmod
    #define close       _close
    #define fileno      _fileno
    #define fstat       _fstat
    #define getcwd      _getcwd
    #define getpid      _getpid
    #define gettimezone _gettimezone
    #define lseek       _lseek
    #define mkdir(a,b)  _mkdir(a)
    #define open        _open
    #define putenv      _putenv
    #define read        _read
    #define rmdir(a)    _rmdir(a)
    #define stat        _stat
    #define strdup      _strdup
    #define umask       _umask
    #define unlink      _unlink
    #define write       _write
    #endif
#endif /* WIN_LIKE */

#if WINCE
    typedef void FILE;
    typedef int off_t;

    #ifndef EOF
        #define EOF        -1
    #endif

    #define O_RDONLY        0
    #define O_WRONLY        1
    #define O_RDWR          2
    #define O_NDELAY        0x4
    #define O_NONBLOCK      0x4
    #define O_APPEND        0x8
    #define O_CREAT         0x100
    #define O_TRUNC         0x200
    #define O_TEXT          0x400
    #define O_EXCL          0x800
    #define O_BINARY        0x1000

    /*
        stat flags
     */
    #define S_IFMT          0170000 
    #define S_IFDIR         0040000
    #define S_IFCHR         0020000         /* character special */
    #define S_IFIFO         0010000
    #define S_IFREG         0100000
    #define S_IREAD         0000400
    #define S_IWRITE        0000200
    #define S_IEXEC         0000100

    #ifndef S_ISDIR
        #define S_ISDIR(X) (((X) & S_IFMT) == S_IFDIR)
    #endif
    #ifndef S_ISREG
        #define S_ISREG(X) (((X) & S_IFMT) == S_IFREG)
    #endif

    #define STARTF_USESHOWWINDOW 0
    #define STARTF_USESTDHANDLES 0

    #define BUFSIZ   MPR_BUFSIZE
    #define PATHSIZE MPR_MAX_PATH
    #define gethostbyname2(a,b) gethostbyname(a)
#endif /* WINCE */

/*********************************** Externs **********************************/

#ifdef __cplusplus
extern "C" {
#endif

#if LINUX
    /*
        For some reason it is removed from fedora 6 pthreads.h and only comes in for UNIX96
     */
    extern int pthread_mutexattr_gettype (__const pthread_mutexattr_t *__restrict
        __attr, int *__restrict __kind) __THROW;
    /* 
        Set the mutex kind attribute in *ATTR to KIND (either PTHREAD_MUTEX_NORMAL,
        PTHREAD_MUTEX_RECURSIVE, PTHREAD_MUTEX_ERRORCHECK, or PTHREAD_MUTEX_DEFAULT).  
     */
    extern int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind) __THROW;
    extern char **environ;
#endif

#if VXWORKS
    extern int gettimeofday(struct timeval *tv, struct timezone *tz);
    extern uint mprGetpid();
    extern char *strdup(const char *);
    extern int sysClkRateGet();

    #if _WRS_VXWORKS_MAJOR < 6
        #define NI_MAXHOST      128
        extern STATUS access(cchar *path, int mode);
        typedef int     socklen_t;
        struct sockaddr_storage {
            char        pad[1024];
        };
    #else
        /*
            This may or may not be necessary - let us know dev@embedthis.com if your system needs this (and why).
         */
        #if _DIAB_TOOL
            #if BLD_HOST_CPU_ARCH == MPR_CPU_PPC
                // #define __va_copy(dest, src) *(dest) = *(src)
                #define __va_copy(dest, src) memcpy((dest), (src), sizeof(va_list))
            #endif
        #endif
        #define HAVE_SOCKLEN_T
    #endif
#endif  /* VXWORKS */

#if BLD_WIN_LIKE
    struct timezone {
      int  tz_minuteswest;      /* minutes W of Greenwich */
      int  tz_dsttime;          /* type of dst correction */
    };
    extern int  getuid(void);
    extern int  geteuid(void);
    extern int  gettimeofday(struct timeval *tv, struct timezone *tz);
    extern long lrand48(void);
    extern long nap(long);
    extern void srand48(long);
    extern long ulimit(int, ...);
#endif

#if WINCE
    struct stat {
        int     st_dev;
        int     st_ino;
        ushort  st_mode;
        short   st_nlink;
        short   st_uid;
        short   st_gid;
        int     st_rdev;
        long    st_size;
        time_t  st_atime;
        time_t  st_mtime;
        time_t  st_ctime;
    };

    extern int access(cchar *filename, int flags);
    extern int chdir(cchar   dirname);
    extern int chmod(cchar *path, int mode);
    extern int close(int handle);
    extern void exit(int status);
    extern long _get_osfhandle(int handle);
    extern char *getcwd(char* buffer, int maxlen);
    extern char *getenv(cchar *charstuff);
    extern uint getpid();
    extern long lseek(int handle, long offset, int origin);
    extern int mkdir(cchar *dir, int mode);
    extern time_t mktime(struct tm *pt);
    extern int _open_osfhandle(int *handle, int flags);
    extern uint open(cchar *file, int mode,...);
    extern int read(int handle, void *buffer, uint count);
    extern int rename(cchar *from, cchar *to);
    extern int rmdir(cchar   dir);
    extern uint sleep(uint secs);
    extern int stat(cchar *path, struct stat *stat);
    extern char *strdup(char *s);
    extern int write(int handle, cvoid *buffer, uint count);
    extern int umask(int mode);
    extern int unlink(cchar *path);
    extern int errno;

    #undef CreateFile
    #define CreateFile CreateFileA
    WINBASEAPI HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
        HANDLE hTemplateFile);

    #undef CreateProcess
    #define CreateProcess CreateProcessA

    #undef FindFirstFile
    #define FindFirstFile FindFirstFileA
    WINBASEAPI HANDLE WINAPI FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);

    #undef FindNextFile
    #define FindNextFile FindNextFileA
    WINBASEAPI BOOL WINAPI FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);

    #undef GetModuleFileName
    #define GetModuleFileName GetModuleFileNameA
    WINBASEAPI DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);

    #undef GetModuleHandle
    #define GetModuleHandle GetModuleHandleA
    WINBASEAPI HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName);

    #undef GetProcAddress
    #define GetProcAddress GetProcAddressA

    #undef GetFileAttributes
    #define GetFileAttributes GetFileAttributesA
    extern DWORD GetFileAttributesA(cchar *path);

    extern void GetSystemTimeAsFileTime(FILETIME *ft);

    #undef LoadLibrary
    #define LoadLibrary LoadLibraryA
    HINSTANCE WINAPI LoadLibraryA(LPCSTR lpLibFileName);

    #define WSAGetLastError GetLastError

    #define _get_timezone getTimezone
    extern int getTimezone(int *secs);

    extern struct tm *localtime_r(const time_t *when, struct tm *tp);
    extern struct tm *gmtime_r(const time_t *t, struct tm *tp);
#endif /* WINCE */

#ifdef __cplusplus
}
#endif

#endif /* _h_MPR_OS_HDRS */

/*
    @copy   default
    
    Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.
    
    This software is distributed under commercial and open source licenses.
    You may use the GPL open source license described below or you may acquire 
    a commercial license from Embedthis Software. You agree to be fully bound 
    by the terms of either license. Consult the LICENSE.TXT distributed with 
    this software for full details.
    
    This software is open source; you can redistribute it and/or modify it 
    under the terms of the GNU General Public License as published by the 
    Free Software Foundation; either version 2 of the License, or (at your 
    option) any later version. See the GNU General Public License for more 
    details at: http://www.embedthis.com/downloads/gplLicense.html
    
    This program is distributed WITHOUT ANY WARRANTY; without even the 
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
    
    This GPL license does NOT permit incorporating this software into 
    proprietary programs. If you are unable to comply with the GPL, you must
    acquire a commercial license to use this software. Commercial licenses 
    for this software and support services are available from Embedthis 
    Software at http://www.embedthis.com 
    
    @end
 */
