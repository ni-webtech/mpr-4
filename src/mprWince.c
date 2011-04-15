/**
    mprWince.c - Windows CE platform specific code.

    Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.
 */

/********************************************** Includes ********************************************/

#include    "mpr.h"

#if WINCE
/******************************************** Locals and Defines ************************************/
/*
    Windows file time is in 100 ns units starting 1601
    Unix (time_t) time is in sec units starting 1970
    MprTime time is in msec units starting 1970
 */
#define WIN_TICKS         10000000      /* Number of windows units in a second */
#define ORIGIN_GAP        11644473600   /* Gap in seconds between 1601 and 1970 */
#define fileTimeToTime(f) ((((((uint64) ((f).dwHighDateTime)) << 32) | (f).dwLowDateTime) / WIN_TICKS) - ORIGIN_GAP)

static char     *currentDir;            /* Current working directory */
static MprList  *files;                 /* List of open files */
int             errno;                  /* Last error */
static char     timzeone[2][32];        /* Standard and daylight savings zones */

/*
    Adjust by seconds between 1601 and 1970
 */
#define WIN_TICKS_TO_MPR  (WIN_TICKS / MPR_TICKS_PER_SEC)

/********************************************** Forwards ********************************************/

static HANDLE getHandle(int fd);
static long getJulianDays(SYSTEMTIME *when);
static void timeToFileTime(uint64 t, FILETIME *ft);

/************************************************ Code **********************************************/

int mprCreateOsService()
{
    files = mprCreateList();
    currentDir = sclone("/");
    return 0;
}


int mprStartOsService()
{
    WSADATA     wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return -1;
    }
    return 0;
}


void mprStopOsService()
{
    WSACleanup();
}


int mprGetRandomBytes(char *buf, int length, int block)
{
    HCRYPTPROV      prov;
    int             rc;

    rc = 0;

    if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | 0x40)) {
        return mprGetError();
    }
    if (!CryptGenRandom(prov, length, buf)) {
        rc = mprGetError();
    }
    CryptReleaseContext(prov, 0);
    return rc;
}


int mprLoadModule(MprModule *mp)
    cchar *moduleName, cchar *initFunction)
{
    MprModuleEntry  fn;
    void            *handle;
    char            *baseName;

    mprAssert(moduleName && *moduleName);

    baseName = mprGetPathBase(mp->path);
    if ((handle = GetModuleHandle(baseName)) == 0 && (handle = LoadLibrary(mp->path)) == 0) {
        mprError("Can't load module %s\nReason: \"%d\"\n", mp->path, mprGetOsError());
        return MPR_ERR_CANT_READ;
    } 
    mp->handle = handle;
    if (mp->entry) {
        if ((fn = (MprModuleEntry) GetProcAddress((HINSTANCE) handle, mp->entry)) != 0) {
            if ((fn)(mp->moduleData, mp)) < 0) {
                mprError("Initialization for module %s failed", mp->name);
                FreeLibrary((HINSTANCE) handle);
                return MPR_ERR_CANT_ACCESS;
            }
        } else {
            mprError("Can't load module %s\nReason: can't find function \"%s\"\n", mp->name, mp->entry);
            FreeLibrary((HINSTANCE) handle);
            return MPR_ERR_CANT_INITIALIZE;
        }
    }
    return mp;
}


#if KEEP
/*
    Determine the registry hive by the first portion of the path. Return 
    a pointer to the rest of key path after the hive portion.
 */ 
static cchar *getHive(cchar *keyPath, HKEY *hive)
{
    char    key[MPR_MAX_STRING], *cp;
    int     len;

    mprAssert(keyPath && *keyPath);

    *hive = 0;

    scopy(key, sizeof(key), keyPath);
    key[sizeof(key) - 1] = '\0';

    if (cp = strchr(key, '\\')) {
        *cp++ = '\0';
    }
    if (cp == 0 || *cp == '\0') {
        return 0;
    }
    if (!mprStrcmpAnyCase(key, "HKEY_LOCAL_MACHINE")) {
        *hive = HKEY_LOCAL_MACHINE;
    } else if (!mprStrcmpAnyCase(key, "HKEY_CURRENT_USER")) {
        *hive = HKEY_CURRENT_USER;
    } else if (!mprStrcmpAnyCase(key, "HKEY_USERS")) {
        *hive = HKEY_USERS;
    } else if (!mprStrcmpAnyCase(key, "HKEY_CLASSES_ROOT")) {
        *hive = HKEY_CLASSES_ROOT;
    } else {
        return 0;
    }
    if (*hive == 0) {
        return 0;
    }
    len = strlen(key) + 1;
    return keyPath + len;
}


int mprReadRegistry(char **buf, int max, cchar *key, cchar *name)
{
    HKEY        top, h;
    LPWSTR      wkey, wname;
    char        *value;
    ulong       type, size;

    mprAssert(key && *key);
    mprAssert(buf);

    if ((key = getHive(key, &top)) == 0) {
        return MPR_ERR_CANT_ACCESS;
    }
    wkey = mprToUni(key);
    if (RegOpenKeyEx(top, wkey, 0, KEY_READ, &h) != ERROR_SUCCESS) {
        return MPR_ERR_CANT_ACCESS;
    }

    /*
        Get the type
     */
    wname = mprToUni(name);
    if (RegQueryValueEx(h, wname, 0, &type, 0, &size) != ERROR_SUCCESS) {
        RegCloseKey(h);
        return MPR_ERR_CANT_READ;
    }
    if (type != REG_SZ && type != REG_EXPAND_SZ) {
        RegCloseKey(h);
        return MPR_ERR_BAD_TYPE;
    }
    value = mprAlloc(size);
    if ((int) size > max) {
        RegCloseKey(h);
        mprAssert(!MPR_ERR_WONT_FIT);
        return MPR_ERR_WONT_FIT;
    }
    if (RegQueryValueEx(h, wname, 0, &type, (uchar*) value, &size) != ERROR_SUCCESS) {
        RegCloseKey(h);
        return MPR_ERR_CANT_READ;
    }
    RegCloseKey(h);
    *buf = value;
    return 0;
}


void mprSetInst(Mpr *mpr, long inst)
{
    mpr->appInstance = inst;
}


void mprSetHwnd(HWND h)
{
    MPR->service->hwnd = h;
}


void mprSetSocketMessage(int socketMessage)
{
    MPR->service->socketMessage = socketMessage;
}
#endif /* WINCE */


void mprSleep(MprTime milliseconds)
{
    Sleep((int) milliseconds);
}


void mprUnloadModule(MprModule *mp)
{
    mprAssert(mp->handle);

    mprStopModule(mp);
    mprRemoveItem(MPR->moduleService->modules, mp);
    FreeLibrary((HINSTANCE) mp->handle);
}


#if KEEP
void mprWriteToOsLog(cchar *message, int flags, int level)
{
    HKEY        hkey;
    void        *event;
    long        errorType;
    ulong       exists;
    char        buf[MPR_MAX_STRING], logName[MPR_MAX_STRING], *lines[9], *cp, *value;
    int         type;
    static int  once = 0;

    scopy(buf, sizeof(buf), message);
    cp = &buf[strlen(buf) - 1];
    while (*cp == '\n' && cp > buf) {
        *cp-- = '\0';
    }

    type = EVENTLOG_ERROR_TYPE;

    lines[0] = buf;
    lines[1] = 0;
    lines[2] = lines[3] = lines[4] = lines[5] = 0;
    lines[6] = lines[7] = lines[8] = 0;

    if (once == 0) {
        /*  Initialize the registry */
        once = 1;
        mprSprintf(logName, sizeof(logName), "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s",
            mprGetAppName());
        hkey = 0;

        if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, logName, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkey, &exists) == ERROR_SUCCESS) {
            value = "%SystemRoot%\\System32\\netmsg.dll";
            if (RegSetValueEx(hkey, "EventMessageFile", 0, REG_EXPAND_SZ, 
                    (uchar*) value, strlen(value) + 1) != ERROR_SUCCESS) {
                RegCloseKey(hkey);
                return;
            }
            errorType = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
            if (RegSetValueEx(hkey, "TypesSupported", 0, REG_DWORD, (uchar*) &errorType, sizeof(DWORD)) != ERROR_SUCCESS) {
                RegCloseKey(hkey);
                return;
            }
            RegCloseKey(hkey);
        }
    }
    event = RegisterEventSource(0, mprGetAppName());
    if (event) {
        /*
            3299 is the event number for the generic message in netmsg.dll.
            "%1 %2 %3 %4 %5 %6 %7 %8 %9" -- thanks Apache for the tip
         */
        ReportEvent(event, EVENTLOG_ERROR_TYPE, 0, 3299, NULL, sizeof(lines) / sizeof(char*), 0, (LPCSTR*) lines, 0);
        DeregisterEventSource(event);
    }
}

int mprWriteRegistry(cchar *key, cchar *name, cchar *value)
{
    HKEY    top, h, subHandle;
    ulong   disposition;

    mprAssert(key && *key);
    mprAssert(name && *name);
    mprAssert(value && *value);

    /*
        Get the registry hive
     */
    if ((key = getHive(key, &top)) == 0) {
        return MPR_ERR_CANT_ACCESS;
    }

    if (name) {
        /*
            Write a registry string value
         */
        if (RegOpenKeyEx(top, key, 0, KEY_ALL_ACCESS, &h) != ERROR_SUCCESS) {
            return MPR_ERR_CANT_ACCESS;
        }
        if (RegSetValueEx(h, name, 0, REG_SZ, value, strlen(value) + 1) != ERROR_SUCCESS) {
            RegCloseKey(h);
            return MPR_ERR_CANT_READ;
        }

    } else {
        /*
            Create a new sub key
         */
        if (RegOpenKeyEx(top, key, 0, KEY_CREATE_SUB_KEY, &h) != ERROR_SUCCESS){
            return MPR_ERR_CANT_ACCESS;
        }
        if (RegCreateKeyEx(h, name, 0, NULL, REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS, NULL, &subHandle, &disposition) != ERROR_SUCCESS) {
            return MPR_ERR_CANT_ACCESS;
        }
        RegCloseKey(subHandle);
    }
    RegCloseKey(h);
    return 0;
}
#endif


/******************************************* Posix Layer ********************************/

int access(cchar *path, int flags)
{
    char    *tmpPath;
    int     rc;

    if (!mprIsAbsPath(MPR, path)) {
        path = (cchar*) tmpPath = mprJoinPath(MPR, currentDir, path);
    } else {
        tmpPath = 0;
    }
    rc = GetFileAttributesA(path) != -1 ? 0 : -1;
    return rc;
}


int chdir(cchar *dir)
{
    currentDir = mprGetAbsPath(MPR, dir);
    return 0;
}


int chmod(cchar *path, int mode)
{
    /* CE has no such permissions */
    return 0;
}


int close(int fd)
{
    int     rc;

    //  LOCKING
    rc = CloseHandle(getHandle(fd));
    mprSetItem(files, fd, NULL);
    return (rc != 0) ? 0 : -1;
}


long _get_osfhandle(int handle)
{
    return (long) handle;
}


char *getenv(cchar *key)
{
    return 0;
}


char *getcwd(char *buf, int size)
{
    scopy(buf, size, currentDir);
    return buf;
}


uint getpid() {
    return 0;
}


long lseek(int handle, long offset, int origin)
{
    switch (origin) {
        case SEEK_SET: offset = FILE_BEGIN; break;
        case SEEK_CUR: offset = FILE_CURRENT; break;
        case SEEK_END: offset = FILE_END; break;
    }
    return SetFilePointer((HANDLE) handle, offset, NULL, origin);
}


int mkdir(cchar *dir, int mode)
{
    char    *tmpDir;
    uni     *wdir;
    int     rc;

    if (!mprIsAbsPath(MPR, dir)) {
        dir = (cchar*) tmpDir = mprJoinPath(MPR, currentDir, dir);
    } else {
        tmpDir = 0;
    }

    wdir = mprToUni(MPR, dir);
    rc = CreateDirectoryW(wdir, NULL);
    return (rc != 0) ? 0 : -1;
}


static HANDLE getHandle(int fd)
{
    //  LOCKING
    return (HANDLE) mprGetItem(files, fd);
}


static int addHandle(HANDLE h)
{
    int     i;

    //  LOCKING
    for (i = 0; i < files->length; i++) {
        if (files->items[i] == 0) {
            mprSetItem(files, i, h);
            return i;
        }
    }
    return mprAddItem(files, h);
}


int _open_osfhandle(int *handle, int flags)
{
    return addHandle((HANDLE) handle);
}


uint open(cchar *path, int mode, va_list arg)
{
    uni     *wpath;
    char    *tmpPath;
    DWORD   accessFlags, shareFlags, createFlags;
    HANDLE  h;

    if (!mprIsAbsPath(MPR, path)) {
        path = (cchar*) tmpPath = mprGetAbsPath(MPR, path);
    } else {
        tmpPath = 0;
    }

    shareFlags = FILE_SHARE_READ;
    accessFlags = 0;
    createFlags = 0;

    if ((mode & O_RDWR) != 0) {
        accessFlags = GENERIC_READ | GENERIC_WRITE;
    } else if ((mode & O_WRONLY) != 0) {
        accessFlags = GENERIC_WRITE;
    } else {
        accessFlags = GENERIC_READ;
    }
    if ((mode & O_CREAT) != 0) {
        createFlags = CREATE_ALWAYS;
    } else {
        createFlags = OPEN_EXISTING;
    }
    wpath = mprToUni(MPR, path);
    h = CreateFileW(wpath, accessFlags, shareFlags, NULL, createFlags, FILE_ATTRIBUTE_NORMAL, NULL);

    return h == INVALID_HANDLE_VALUE ? -1 : addHandle(h);
}


int read(int fd, void *buffer, uint length)
{
    DWORD   dw;

    ReadFile(getHandle(fd), buffer, length, &dw, NULL);
    return (int) dw;
}


int rename(cchar *oldname, cchar *newname)
{
    uni     *from, *to;
    char    *tmpOld, *tmpNew;
    int     rc;

    if (!mprIsAbsPath(MPR, oldname)) {
        oldname = (cchar*) tmpOld = mprJoinPath(MPR, currentDir, oldname);
    } else {
        tmpOld = 0;
    }
    if (!mprIsAbsPath(MPR, newname)) {
        newname = (cchar*) tmpNew = mprJoinPath(MPR, currentDir, newname);
    } else {
        tmpNew = 0;
    }
    from = mprToUni(MPR, oldname);
    to = mprToUni(MPR, newname);
    rc = MoveFileW(from, to);
    return rc == 0 ? 0 : -1;
}


int rmdir(cchar *dir)
{
    uni     *wdir;
    char    *tmpDir;
    int     rc;

    if (!mprIsAbsPath(MPR, dir)) {
        dir = (cchar*) tmpDir = mprJoinPath(MPR, currentDir, dir);
    } else {
        tmpDir = 0;
    }
    wdir = mprToUni(MPR, dir);
    rc = RemoveDirectoryW(wdir);
    return rc == 0 ? 0 : -1;
}


int stat(cchar *path, struct stat *sbuf)
{
    WIN32_FIND_DATAW    fd;
    DWORD               attributes;
    HANDLE              h;
    DWORD               dwSizeLow, dwSizeHigh, dwError;
    char                *tmpPath;
    uni                 *wpath;

    dwSizeLow = 0;
    dwSizeHigh = 0;
    dwError = 0;

    memset(sbuf, 0, sizeof(struct stat));

    if (!mprIsAbsPath(MPR, path)) {
        path = (cchar*) tmpPath = mprJoinPath(MPR, currentDir, path);
    } else {
        tmpPath = 0;
    }
    wpath = mprToUni(MPR, path);

    attributes = GetFileAttributesW(wpath);
    if (attributes == -1) {
        return -1;
    }
    if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        sbuf->st_mode += S_IFDIR;
    } else {
        sbuf->st_mode += S_IFREG;
    }

    h = FindFirstFileW(wpath, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        if (wpath[wcslen(wpath)-1]  == L'\\') {
            wpath[wcslen(wpath)-1] = L'\0';
            h = FindFirstFileW(wpath, &fd);
            if (h == INVALID_HANDLE_VALUE) {
                return 0;
            }
        } else {
            return 0;
        }
    }
    sbuf->st_atime = (time_t) fileTimeToTime(fd.ftLastAccessTime);
    sbuf->st_mtime = (time_t) fileTimeToTime(fd.ftLastWriteTime);
    sbuf->st_ctime = (time_t) fileTimeToTime(fd.ftCreationTime);
    sbuf->st_size  = fd.nFileSizeLow;

    FindClose(h);
    return 0;
}


/*
    Convert time in seconds to a file time
 */
static void timeToFileTime(uint64 t, FILETIME *ft)
{
    t += ORIGIN_GAP;
    t *= WIN_TICKS;
    ft->dwHighDateTime = (DWORD) ((t >> 32) & 0xFFFFFFFF);
    ft->dwLowDateTime  = (DWORD) (t & 0xFFFFFFFF);
}


/*
    Get the Julian current year day.

    General Julian Day formula:
        a = (14 - month) / 12;
        y = year + 4800 - a;
        m = month + 12 * a - 3;
        jd = day + (y * 365) + (y / 4) - (y / 100) + (y / 400) + (m * 153 + 2) / 5 - 32045;
 */
static long getJulianDays(SYSTEMTIME *when)
{
    int     y, m, d, a, day, startYearDay;

    a = (14 - when->wMonth) / 12;
    y = when->wYear + 4800 - a;
    m = when->wMonth + 12   a - 3;
    d = when->wDay;

    /*
        Compute the difference between Julian days for Jan 1 and "when" of the same year
     */
    day = d + (y   365) + (y / 4) - (y / 100) + (y / 400) + (m * 153 + 2) / 5;
    y = when->wYear + 4799;
    startYearDay = 1 + (y   365) + (y / 4) - (y / 100) + (y / 400) + 1532 / 5;

    return day - startYearDay;
}


struct tm *gmtime_r(const time_t *when, struct tm *tp)
{
    FILETIME    f;
    SYSTEMTIME  s;
    
    timeToFileTime(*when, &f);
    FileTimeToSystemTime(&f, &s);

    tp->tm_year  = s.wYear - 1900;
    tp->tm_mon   = s.wMonth- 1;
    tp->tm_wday  = s.wDayOfWeek;
    tp->tm_mday  = s.wDay;
    tp->tm_yday  = getJulianDays(&s);
    tp->tm_hour  = s.wHour;
    tp->tm_min   = s.wMinute;
    tp->tm_sec   = s.wSecond;
    tp->tm_isdst = 0;

    return tp;
}


struct tm *localtime_r(const time_t *when, struct tm *tp)
{
    FILETIME                f;
    SYSTEMTIME              s;
    TIME_ZONE_INFORMATION   tz;
    int                     bias, rc;

    mprAssert(when);
    mprAssert(tp);

    //  MOB -- but this is setting if DST is enabled now, not at "when"
    rc = GetTimeZoneInformation(&tz);
    bias = tz.Bias;
    if (rc == TIME_ZONE_ID_DAYLIGHT) {
        tp->tm_isdst = 1;
        bias += tz.DaylightBias;
    } else {
        tp->tm_isdst = 0;
    }
    bias *= 60;

    timeToFileTime(*when - bias, &f);
    FileTimeToSystemTime(&f, &s);
    
    tp->tm_year   = s.wYear - 1900;
    tp->tm_mon    = s.wMonth- 1;
    tp->tm_wday   = s.wDayOfWeek;
    tp->tm_mday   = s.wDay;
    tp->tm_yday   = getJulianDays(&s);
    tp->tm_hour   = s.wHour;
    tp->tm_min    = s.wMinute;
    tp->tm_sec    = s.wSecond;

    return tp;
}


time_t mktime(struct tm *tp)
{
    TIME_ZONE_INFORMATION   tz;
    SYSTEMTIME              s;
    FILETIME                f;
    time_t                  result;
    int                     rc, bias;

    mprAssert(tp);

    //  MOB -- but this is setting if DST is enabled now, not at "when"
    rc = GetTimeZoneInformation(&tz);
    bias = tz.Bias;
    if (rc == TIME_ZONE_ID_DAYLIGHT) {
        tp->tm_isdst = 1;
        bias += tz.DaylightBias;
    }
    bias *= 60;
    
    s.wYear = tp->tm_year + 1900;
    s.wMonth = tp->tm_mon + 1;
    s.wDayOfWeek = tp->tm_wday;
    s.wDay = tp->tm_mday;
    s.wHour = tp->tm_hour;
    s.wMinute = tp->tm_min;
    s.wSecond = tp->tm_sec;

    //  MOB -- rc
    SystemTimeToFileTime(&s, &f);
    result = (time_t) (fileTimeToTime(f) + tz.Bias   60);
    if (rc == TIME_ZONE_ID_DAYLIGHT) {
        result -= bias;
    }
    return result;
}


int write(int fd, cvoid *buffer, uint count)
{
    DWORD   dw;

    WriteFile(getHandle(fd), buffer, count, &dw, NULL);
    return (int) dw;
}


int unlink(cchar *file)
{
    uni     *wpath;
    int     rc;

    wpath = mprToUni(MPR, file);
    rc = DeleteFileW(wpath);
    return rc == 0 ? 0 : -1;
}


/********************************************** Windows32 Extensions *********************************************/

WINBASEAPI HANDLE WINAPI CreateFileA(LPCSTR path, DWORD access, DWORD sharing,
    LPSECURITY_ATTRIBUTES security, DWORD create, DWORD flags, HANDLE template)
{
    LPWSTR  wpath;
    HANDLE  h;

    wpath = mprToUni(MPR, path);
    h = CreateFileW(wpath, access, sharing, security, create, flags, template);
    return h;
}


BOOL WINAPI CreateProcessA(LPCSTR app, LPCSTR cmd, LPSECURITY_ATTRIBUTES att, LPSECURITY_ATTRIBUTES threadatt,
    BOOL options, DWORD flags, LPVOID env, LPSTR dir, LPSTARTUPINFO lpsi, LPPROCESS_INFORMATION info)
{
    LPWSTR      wapp, wcmd, wdir;

    wapp  = mprToUni(MPR, app);
    wcmd  = mprToUni(MPR, cmd);
    wdir  = mprToUni(MPR, dir);

    return CreateProcessW(wapp, wcmd, att, threadatt, options, flags, env, wdir, lpsi, info);
}


HANDLE FindFirstFileA(LPCSTR path, WIN32_FIND_DATAA *data)
{
    WIN32_FIND_DATAW    wdata;
    LPWSTR              wpath;
    HANDLE              h;
    char                *file;

    wpath = mprToUni(MPR, path);
    h = FindFirstFileW(wpath, &wdata);
    
    file = mprToMulti(MPR, wdata.cFileName);
    strcpy(data->cFileName, file);
    return h;
}


BOOL FindNextFileA(HANDLE handle, WIN32_FIND_DATAA *data)
{
    WIN32_FIND_DATAW    wdata;
    char                *file;
    BOOL                result;

    result = FindNextFileW(handle, &wdata);
    file = mprToMulti(MPR, wdata.cFileName);
    strcpy(data->cFileName, file);
    return result;
}


DWORD GetFileAttributesA(cchar *path)
{
    LPWSTR      wpath;
    DWORD       result;

    wpath = mprToUni(MPR, path);
    result = GetFileAttributesW(wpath);
    return result;
}


DWORD GetModuleFileNameA(HMODULE module, LPSTR buf, DWORD size)
{
    LPWSTR      wpath;
    LPSTR       mb;
    ssize       ret;

    wpath = mprAlloc( size * sizeof(wchar_t));
    ret = GetModuleFileNameW(module, wpath, size);
    mb = mprToMulti(MPR, wpath);
    strcpy(buf, mb);
    return ret;
}


WINBASEAPI HMODULE WINAPI GetModuleHandleA(LPCSTR path)
{
    LPWSTR      wpath;

    wpath = mprToUni(MPR, path);
    return GetModuleHandleW(wpath);
}


void GetSystemTimeAsFileTime(FILETIME *ft)
{
    SYSTEMTIME  s;

    GetSystemTime(&s);
    SystemTimeToFileTime(&s, ft);
}


HINSTANCE WINAPI LoadLibraryA(LPCSTR path)
{
    LPWSTR      wpath;

    wpath = mprToUni(MPR, path);
    return LoadLibraryW(wpath);
}

void mprWriteToOsLog(cchar *message, int flags, int level)
{
    //  TODO
}

#else
void stubMprWince() {}
#endif /* WINCE */

/*
    @copy   default

    Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2011. All Rights Reserved.

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

    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
