/**
    mprPath.c - Path (filename) services.

    This modules provides cross platform path name services.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/********************************** Defines ***********************************/
/*
    Find the first separator in the path
 */
#if BIT_UNIX_LIKE
    #define firstSep(fs, path)  strchr(path, fs->separators[0])
#else
    #define firstSep(fs, path)  strpbrk(path, fs->separators)
#endif

#define defaultSep(fs)          (fs->separators[0])

/************************************* Code ***********************************/

static MPR_INLINE bool isSep(MprFileSystem *fs, int c) 
{
    char    *separators;

    mprAssert(fs);
    for (separators = fs->separators; *separators; separators++) {
        if (*separators == c)
            return 1;
    }
    return 0;
}


static MPR_INLINE bool hasDrive(MprFileSystem *fs, cchar *path) 
{
    char    *cp, *endDrive;

    mprAssert(fs);
    mprAssert(path);

    if (fs->hasDriveSpecs) {
        cp = firstSep(fs, path);
        endDrive = strchr(path, ':');
        if (endDrive && (cp == NULL || endDrive < cp)) {
            return 1;
        }
    }
    return 0;
}


/*
    Return true if the path is absolute.
    This means the path portion after an optional drive specifier must begin with a directory speparator charcter.
    Cygwin returns true for "/abc" and "C:/abc".
 */
static MPR_INLINE bool isAbsPath(MprFileSystem *fs, cchar *path) 
{
    char    *cp, *endDrive;

    mprAssert(fs);
    mprAssert(path);

    if (path == NULL || *path == '\0') {
        return 0;
    }
    if (fs->hasDriveSpecs) {
        if ((cp = firstSep(fs, path)) != 0) {
            if ((endDrive = strchr(path, ':')) != 0) {
                if (&endDrive[1] == cp) {
                    return 1;
                }
            }
            if (cp == path) {
                return 1;
            }
        }
    } else {
        if (isSep(fs, path[0])) {
            return 1;
        }
    }
    return 0;
}


/*
    Return true if the path is a fully qualified absolute path.
    On windows, this means it must have a drive specifier.
    On cygwin, this means it must not have a drive specifier.
 */
static MPR_INLINE bool isFullPath(MprFileSystem *fs, cchar *path) 
{
    mprAssert(fs);
    mprAssert(path);

#if (BIT_WIN_LIKE || VXWORKS) && !WINCE
{
    char    *cp, *endDrive;

    if (fs->hasDriveSpecs) {
        cp = firstSep(fs, path);
        endDrive = strchr(path, ':');
        if (endDrive && cp && &endDrive[1] == cp) {
            return 1;
        }
        return 0;
    }
}
#endif
    if (isSep(fs, path[0])) {
        return 1;
    }
    return 0;
}


/*
    Return true if the directory is the root directory on a file system
 */
static MPR_INLINE bool isRoot(MprFileSystem *fs, cchar *path) 
{
    char    *cp;

    if (isAbsPath(fs, path)) {
        cp = firstSep(fs, path);
        if (cp && cp[1] == '\0') {
            return 1;
        }
    }
    return 0;
}


static MPR_INLINE char *lastSep(MprFileSystem *fs, cchar *path) 
{
    char    *cp;

    for (cp = (char*) &path[slen(path)] - 1; cp >= path; cp--) {
        if (isSep(fs, *cp)) {
            return cp;
        }
    }
    return 0;
}

/************************************ Code ************************************/
/*
    This copies a file.
 */
int mprCopyPath(cchar *fromName, cchar *toName, int mode)
{
    MprFile     *from, *to;
    ssize       count;
    char        buf[MPR_BUFSIZE];

    if ((from = mprOpenFile(fromName, O_RDONLY | O_BINARY, 0)) == 0) {
        mprError("Can't open %s", fromName);
        return MPR_ERR_CANT_OPEN;
    }
    if ((to = mprOpenFile(toName, O_WRONLY | O_TRUNC | O_CREAT | O_BINARY, mode)) == 0) {
        mprError("Can't open %s", toName);
        return MPR_ERR_CANT_OPEN;
    }
    while ((count = mprReadFile(from, buf, sizeof(buf))) > 0) {
        mprWriteFile(to, buf, count);
    }
    mprCloseFile(from);
    mprCloseFile(to);
    return 0;
}


int mprDeletePath(cchar *path)
{
    MprFileSystem   *fs;

    if (path == NULL || *path == '\0') {
        return MPR_ERR_CANT_ACCESS;
    }
    fs = mprLookupFileSystem(path);
    return fs->deletePath(fs, path);
}


/*
    Return an absolute (normalized) path.
    On CYGWIN, this is a cygwin path with forward-slashes and without drive specs. 
    Use mprGetWinPath for a windows style path with a drive specifier and back-slashes.
 */
char *mprGetAbsPath(cchar *path)
{
    MprFileSystem   *fs;
    char            *result;

    if (path == 0 || *path == '\0') {
        path = ".";
    }
#if BIT_ROM
    return mprNormalizePath(path);
#elif CYGWIN
    {
        ssize   len;
        /*
            cygwin_conf_path has a bug for paths that attempt to address a directory above the root. ie. "../../../.."
            So must convert to a windows path first.
         */
        if (strncmp(path, "../", 3) == 0) {
            path = mprGetWinPath(path);
        }
        if ((len = cygwin_conv_path(CCP_WIN_A_TO_POSIX | CCP_ABSOLUTE, path, NULL, 0)) >= 0) {
            /* Len includes room for the null */
            if ((result = mprAlloc(len)) == 0) {
                return 0;
            }
            cygwin_conv_path(CCP_WIN_A_TO_POSIX | CCP_ABSOLUTE, path, result, len);
            if (len > 3 && result[len - 2] == '/' && result[len - 3] != ':') {
                /* Trim trailing "/" */
                result[len - 2] = '\0';
            }
            return result;
        }
    }
#endif
    fs = mprLookupFileSystem(path);
    if (isFullPath(fs, path)) {
        /* Already absolute. On windows, must contain a drive specifier */
        return mprNormalizePath(path);
    }

#if BIT_WIN_LIKE && !WINCE
{
    char    buf[MPR_MAX_PATH];
    GetFullPathName(path, sizeof(buf) - 1, buf, NULL);
    buf[sizeof(buf) - 1] = '\0';
    result = mprNormalizePath(buf);
}
#elif VXWORKS
{
    char    *dir;
    if (hasDrive(fs, path)) {
        dir = mprGetCurrentPath();
        result = mprJoinPath(dir, &strchr(path, ':')[1]);

    } else {
        if (isAbsPath(fs, path)) {
            /*
                Path is absolute, but without a drive. Use the current drive.
             */
            dir = mprGetCurrentPath();
            result = mprJoinPath(dir, path);
        } else {
            dir = mprGetCurrentPath();
            result = mprJoinPath(dir, path);
        }
    }
}
#else
{
    char   *dir;
    dir = mprGetCurrentPath();
    result = mprJoinPath(dir, path);
}
#endif
    return result;
}


/*
    Get the directory containing the application executable. Tries to return an absolute path.
 */
char *mprGetAppDir()
{ 
    if (MPR->appDir == 0) {
        MPR->appDir = mprGetPathDir(mprGetAppPath());
    }
    return sclone(MPR->appDir); 
} 


/*
    Get the path for the application executable. Tries to return an absolute path.
 */
char *mprGetAppPath()
{ 
    if (MPR->appPath) {
        return sclone(MPR->appPath);
    }

#if MACOSX
{
    char    path[MPR_MAX_PATH], pbuf[MPR_MAX_PATH];
    uint    size;
    ssize   len;

    size = sizeof(path) - 1;
    if (_NSGetExecutablePath(path, &size) < 0) {
        return mprGetAbsPath(".");
    }
    path[size] = '\0';
    len = readlink(path, pbuf, sizeof(pbuf) - 1);
    if (len < 0) {
        return mprGetAbsPath(path);
    }
    pbuf[len] = '\0';
    MPR->appPath = mprGetAbsPath(pbuf);
}
#elif FREEBSD 
{
    char    pbuf[MPR_MAX_STRING];
    int     len;

    len = readlink("/proc/curproc/file", pbuf, sizeof(pbuf) - 1);
    if (len < 0) {
        return mprGetAbsPath(".");
     }
     pbuf[len] = '\0';
     MPR->appPath = mprGetAbsPath(pbuf);
}
#elif BIT_UNIX_LIKE 
{
    char    pbuf[MPR_MAX_STRING], *path;
    int     len;
#if SOLARIS
    path = sfmt("/proc/%i/path/a.out", getpid()); 
#else
    path = sfmt("/proc/%i/exe", getpid()); 
#endif
    len = readlink(path, pbuf, sizeof(pbuf) - 1);
    if (len < 0) {
        return mprGetAbsPath(".");
    }
    pbuf[len] = '\0';
    MPR->appPath = mprGetAbsPath(pbuf);
}
#elif BIT_WIN_LIKE
{
    char    pbuf[MPR_MAX_PATH];

    if (GetModuleFileName(0, pbuf, sizeof(pbuf) - 1) <= 0) {
        return 0;
    }
    MPR->appPath = mprGetAbsPath(pbuf);
}
#else
    if (mprIsPathAbs(MPR->argv[0])) {
        MPR->appPath = sclone(MPR->argv[0]);
    } else {
        MPR->appPath = mprGetCurrentPath();
    }
#endif
    return sclone(MPR->appPath);
}

 
/*
    This will return a fully qualified absolute path for the current working directory.
 */
char *mprGetCurrentPath()
{
    char    dir[MPR_MAX_PATH];

    if (getcwd(dir, sizeof(dir)) == 0) {
        return mprGetAbsPath("/");
    }

#if VXWORKS
{
    MprFileSystem   *fs;
    char            sep[2];

    fs = mprLookupFileSystem(dir);

    /*
        Vx will sometimes just return a drive with no path.
     */
    if (firstSep(fs, dir) == NULL) {
        sep[0] = defaultSep(fs);
        sep[1] = '\0';
        return sjoin(dir, sep, NULL);
    }
}
#elif BIT_WIN_LIKE || CYGWIN
{
    MprFileSystem   *fs;
    fs = mprLookupFileSystem(dir);
    mprMapSeparators(dir, fs->separators[0]);
}
#endif
    return sclone(dir);
}


cchar *mprGetFirstPathSeparator(cchar *path) 
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(path);
    return firstSep(fs, path);
}


/*
    Return a pointer into the path at the last path separator or null if none found
 */
cchar *mprGetLastPathSeparator(cchar *path) 
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(path);
    return lastSep(fs, path);
}


/*
    Return a path with native separators. This means "\\" on windows and cygwin
 */
char *mprGetNativePath(cchar *path)
{
    return mprTransformPath(path, MPR_PATH_NATIVE_SEP);
}


/*
    Return the last portion of a pathname. The separators are not mapped and the path is not cleaned.
 */
char *mprGetPathBase(cchar *path)
{
    MprFileSystem   *fs;
    char            *cp;

    if (path == 0) {
        return sclone("");
    }
    fs = mprLookupFileSystem(path);
    cp = (char*) lastSep(fs, path);
    if (cp == 0) {
        return sclone(path);
    } 
    if (cp == path) {
        if (cp[1] == '\0') {
            return sclone(path);
        }
    } else if (cp[1] == '\0') {
        return sclone("");
    }
    return sclone(&cp[1]);
}


/*
    Return the last portion of a pathname. The separators are not mapped and the path is not cleaned.
    This returns a reference into the original string
 */
cchar *mprGetPathBaseRef(cchar *path)
{
    MprFileSystem   *fs;
    char            *cp;

    if (path == 0) {
        return sclone("");
    }
    fs = mprLookupFileSystem(path);
    if ((cp = (char*) lastSep(fs, path)) == 0) {
        return path;
    } 
    if (cp == path) {
        if (cp[1] == '\0') {
            return path;
        }
    } else if (cp[1] == '\0') {
        return "";
    }
    return &cp[1];
}


/*
    Return the directory portion of a pathname.
 */
char *mprGetPathDir(cchar *path)
{
    MprFileSystem   *fs;
    cchar           *cp, *start;
    char            *result;
    ssize          len;

    mprAssert(path);

    if (path == 0 || *path == '\0') {
        return sclone(path);
    }

    fs = mprLookupFileSystem(path);
    len = slen(path);
    cp = &path[len - 1];
    start = hasDrive(fs, path) ? strchr(path, ':') + 1 : path;

    /*
        Step back over trailing slashes
     */
    while (cp > start && isSep(fs, *cp)) {
        cp--;
    }
    for (; cp > start && !isSep(fs, *cp); cp--) { }

    if (cp == start) {
        if (!isSep(fs, *cp)) {
            /* No slashes found, parent is current dir */
            return sclone(".");
        }
        cp++;
    }
    len = (cp - path);
    result = mprAlloc(len + 1);
    mprMemcpy(result, len + 1, path, len);
    result[len] = '\0';
    return result;
}


/*
    Return the extension portion of a pathname.
    Return the extension without the "."
 */
char *mprGetPathExt(cchar *path)
{
    MprFileSystem  *fs;
    char            *cp;

    if ((cp = srchr(path, '.')) != NULL) {
        fs = mprLookupFileSystem(path);
        /*
            If there is no separator ("/") after the extension, then use it.
         */
        if (firstSep(fs, cp) == 0) {
            return sclone(++cp);
        }
    } 
    return 0;
}


/*
    This returns a list of MprDirEntry objects
 */
#if BIT_WIN_LIKE
static MprList *getDirFiles(cchar *dir, int flags)
{
    HANDLE          h;
    MprDirEntry     *dp;
    MprPath         fileInfo;
    MprList         *list;
    cchar           *seps;
    char            *path, pbuf[MPR_MAX_PATH];
#if WINCE
    WIN32_FIND_DATAA findData;
#else
    WIN32_FIND_DATA findData;
#endif

    list = 0;
    dp = 0;

    if ((path = mprJoinPath(dir, "*.*")) == 0) {
        return 0;
    }
    seps = mprGetPathSeparators(dir);

    h = FindFirstFile(path, &findData);
    if (h == INVALID_HANDLE_VALUE) {
        return 0;
    }
    list = mprCreateList(-1, 0);

    do {
        if (findData.cFileName[0] == '.' && (findData.cFileName[1] == '\0' || findData.cFileName[1] == '.')) {
            continue;
        }
        if ((dp = mprAlloc(sizeof(MprDirEntry))) == 0) {
            return 0;
        }
        dp->name = sclone(findData.cFileName);
        if (dp->name == 0) {
            return 0;
        }
        /* dp->lastModified = (uint) findData.ftLastWriteTime.dwLowDateTime; */

        if (mprSprintf(pbuf, sizeof(pbuf), "%s%c%s", dir, seps[0], dp->name) < 0) {
            dp->lastModified = 0;
        } else {
            mprGetPathInfo(pbuf, &fileInfo);
            dp->lastModified = fileInfo.mtime;
        }
        dp->isDir = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
        dp->isLink = 0;

#if FUTURE_64_BIT
        if (findData.nFileSizeLow < 0) {
            dp->size = (((uint64) findData.nFileSizeHigh) * INT64(4294967296)) + (4294967296L - 
                (uint64) findData.nFileSizeLow);
        } else {
            dp->size = (((uint64) findData.nFileSizeHigh * INT64(4294967296))) + (uint64) findData.nFileSizeLow;
        }
#else
        dp->size = (uint) findData.nFileSizeLow;
#endif
        mprAddItem(list, dp);
    } while (FindNextFile(h, &findData) != 0);

    FindClose(h);
    return list;
}
#endif /* WIN */


static void manageDirEntry(MprDirEntry *dp, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(dp->name);
    }
}


#if BIT_UNIX_LIKE || VXWORKS || CYGWIN
static MprList *getDirFiles(cchar *path, int flags)
{
    DIR             *dir;
    MprPath         fileInfo;
    MprList         *list;
    struct dirent   *dirent;
    MprDirEntry     *dp;
    char            *fileName;
    int             rc;

    if ((dir = opendir((char*) path)) == 0) {
        return 0;
    }
    list = mprCreateList(256, 0);

    while ((dirent = readdir(dir)) != 0) {
        if (dirent->d_name[0] == '.' && (dirent->d_name[1] == '\0' || dirent->d_name[1] == '.')) {
            continue;
        }
        fileName = mprJoinPath(path, dirent->d_name);
        /* workaround for if target of symlink does not exist */
        fileInfo.isLink = 0;
        fileInfo.isDir = 0;
        rc = mprGetPathInfo(fileName, &fileInfo);
        if ((dp = mprAllocObj(MprDirEntry, manageDirEntry)) == 0) {
            return 0;
        }
        dp->name = sclone(dirent->d_name);
        if (dp->name == 0) {
            return 0;
        }
        if (rc == 0 || fileInfo.isLink) {
            dp->lastModified = fileInfo.mtime;
            dp->size = fileInfo.size;
            dp->isDir = fileInfo.isDir;
            dp->isLink = fileInfo.isLink;
        } else {
            dp->lastModified = 0;
            dp->size = 0;
            dp->isDir = 0;
            dp->isLink = 0;
        }
        mprAddItem(list, dp);
    }
    closedir(dir);
    return list;
}
#endif


/*
    Find files in the directory "dir". If base is set, use that as the prefix for returned files.
    Returns a list of MprDirEntry objects.
 */
static MprList *findFiles(MprList *list, cchar *dir, cchar *base, int flags)
{
    MprDirEntry     *dp;
    MprList         *files;
    char            *name;
    int             next;

    if ((files = getDirFiles(dir, flags)) == 0) {
        return 0;
    }
    for (next = 0; (dp = mprGetNextItem(files, &next)) != 0; ) {
        if (dp->name[0] == '.') {
            if (dp->name[1] == '\0' || (dp->name[1] == '.' && dp->name[2] == '\0')) {
                continue;
            }
            if (!(flags & MPR_PATH_INC_HIDDEN)) {
                continue;
            }
        }
        name = dp->name;
        dp->name = mprJoinPath(base, name);

        if (!(flags & MPR_PATH_DEPTH_FIRST) && !(dp->isDir && flags & MPR_PATH_NODIRS)) {
            mprAddItem(list, dp);
        }
        if (dp->isDir) {
            if (flags & MPR_PATH_DESCEND) {
                findFiles(list, mprJoinPath(dir, name), mprJoinPath(base, name), flags);
            } 
        }
        if ((flags & MPR_PATH_DEPTH_FIRST) && (!(dp->isDir && flags & MPR_PATH_NODIRS))) {
            mprAddItem(list, dp);
        }
    }
    return list;
}


/*
    Get the files in a directory. Returns a list of MprDirEntry objects.

    MPR_PATH_DESCEND        to traverse subdirectories
    MPR_PATH_DEPTH_FIRST    to do a depth-first traversal
    MPR_PATH_INC_HIDDEN     to include hidden files
    MPR_PATH_NODIRS         to exclude subdirectories
    MPR_PATH_RELATIVE       to return paths relative to the initial directory
 */
MprList *mprGetPathFiles(cchar *dir, int flags)
{
    cchar   *base;

    if (dir == 0 || *dir == '\0') {
        dir = ".";
    }
    base = (flags & MPR_PATH_RELATIVE) ? 0 : dir;
    return findFiles(mprCreateList(-1, 0), dir, base, flags);
}


/*
    Return the first directory of a pathname
 */
char *mprGetPathFirstDir(cchar *path)
{
    MprFileSystem   *fs;
    cchar           *cp;
    int             len;

    mprAssert(path);

    fs = mprLookupFileSystem(path);
    if (isAbsPath(fs, path)) {
        len = hasDrive(fs, path) ? 2 : 1;
        return snclone(path, len);
    } else {
        if ((cp = firstSep(fs, path)) != 0) {
            return snclone(path, cp - path);
        }
        return sclone(path);
    }
}


int mprGetPathInfo(cchar *path, MprPath *info)
{
    MprFileSystem  *fs;

    fs = mprLookupFileSystem(path);
    return fs->getPathInfo(fs, path, info);
}


char *mprGetPathLink(cchar *path)
{
    MprFileSystem  *fs;

    fs = mprLookupFileSystem(path);
    return fs->getPathLink(fs, path);
}


/*
    GetPathParent is smarter than GetPathDir which operates purely textually on the path. GetPathParent will convert
    relative paths to absolute to determine the parent directory.
 */
char *mprGetPathParent(cchar *path)
{
    MprFileSystem   *fs;
    char            *dir;

    fs = mprLookupFileSystem(path);

    if (path == 0 || path[0] == '\0') {
        return mprGetAbsPath(".");
    }
    if (firstSep(fs, path) == NULL) {
        /*
            No parents in the path, so convert to absolute
         */
        dir = mprGetAbsPath(path);
        return mprGetPathDir(dir);
    }
    return mprGetPathDir(path);
}


char *mprGetPortablePath(cchar *path)
{
    char    *result, *cp;

    result = mprTransformPath(path, 0);
    for (cp = result; *cp; cp++) {
        if (*cp == '\\') {
            *cp = '/';
        }
    }
    return result;
}


/*
    This returns a path relative to the current working directory for the given path
 */
char *mprGetRelPath(cchar *destArg, cchar *originArg)
{
    MprFileSystem   *fs;
    char            originBuf[MPR_MAX_FNAME], *cp, *result, *dest, *lastcp, *origin, *op, *lastop;
    int             originSegments, i, commonSegments, sep;

    fs = mprLookupFileSystem(destArg);
    
    if (destArg == 0 || *destArg == '\0') {
        return sclone(".");
    }
    dest = mprNormalizePath(destArg);

    if (!isAbsPath(fs, dest) && (originArg == 0 || *originArg == '\0')) {
        return dest;
    }
    sep = (cp = firstSep(fs, dest)) ? *cp : defaultSep(fs);
    
    if (originArg == 0 || *originArg == '\0') {
        /*
            Get the working directory. Ensure it is null terminated and leave room to append a trailing separators
            On cygwin, this will be a cygwin style path (starts with "/" and no drive specifier).
         */
        if (getcwd(originBuf, sizeof(originBuf)) == 0) {
            strcpy(originBuf, ".");
        }
        originBuf[sizeof(originBuf) - 2] = '\0';
        origin = originBuf;
    } else {
        origin = mprGetAbsPath(originArg);
    }
    dest = mprGetAbsPath(dest);

    /*
        Count segments in origin working directory. Ignore trailing separators.
     */
    for (originSegments = 0, cp = origin; *cp; cp++) {
        if (isSep(fs, *cp) && cp[1]) {
            originSegments++;
        }
    }

    /*
        Find portion of dest that matches the origin directory, if any. Start at -1 because matching root doesn't count.
     */
    commonSegments = -1;
    for (lastop = op = origin, lastcp = cp = dest; *op && *cp; op++, cp++) {
        if (isSep(fs, *op)) {
            lastop = op + 1;
            if (isSep(fs, *cp)) {
                lastcp = cp + 1;
                commonSegments++;
            }
        } else if (fs->caseSensitive) {
            if (*op != *cp) {
                break;
            }
        } else if (*op != *cp && tolower((uchar) *op) != tolower((uchar) *cp)) {
            break;
        }
    }
    mprAssert(commonSegments >= 0);

    if (*cp && *op) {
        op = lastop;
        cp = lastcp;
    }

    /*
        Add one more segment if the last segment matches. Handle trailing separators
     */
    if ((isSep(fs, *op) || *op == '\0') && (isSep(fs, *cp) || *cp == '\0')) {
        commonSegments++;
    }
    if (isSep(fs, *cp)) {
        cp++;
    }
    
    op = result = mprAlloc(originSegments * 3 + slen(dest) + 2);
    for (i = commonSegments; i < originSegments; i++) {
        *op++ = '.';
        *op++ = '.';
        *op++ = defaultSep(fs);
    }
    if (*cp) {
        strcpy(op, cp);
    } else if (op > result) {
        /*
            Cleanup trailing separators ("../" is the end of the new path)
         */
        op[-1] = '\0';
    } else {
        strcpy(result, ".");
    }
    mprMapSeparators(result, sep);
    return result;
}


char *mprGetTempPath(cchar *tempDir)
{
    MprFile         *file;
    char            *dir, *path;
    int             i, now;
    static int      tempSeed = 0;

    if (tempDir == 0 || *tempDir == '\0') {
#if WINCE
        dir = sclone("/Temp");
#elif BIT_WIN_LIKE
{
        MprFileSystem   *fs;
        fs = mprLookupFileSystem(tempDir ? tempDir : (cchar*) "/");
        dir = sclone(getenv("TEMP"));
        mprMapSeparators(dir, defaultSep(fs));
}
#elif VXWORKS
        dir = sclone(".");
#else
        dir = sclone("/tmp");
#endif
    } else {
        dir = sclone(tempDir);
    }
    now = ((int) mprGetTime() & 0xFFFF) % 64000;
    file = 0;
    path = 0;

    for (i = 0; i < 128; i++) {
        path = sfmt("%s/MPR_%d_%d_%d.tmp", dir, getpid(), now, ++tempSeed);
        file = mprOpenFile(path, O_CREAT | O_EXCL | O_BINARY, 0664);
        if (file) {
            mprCloseFile(file);
            break;
        }
    }
    if (file == 0) {
        return 0;
    }
    return path;
}


/*
    Return a windows path.
    On CYGWIN, this is a cygwin path without drive specs.
 */
char *mprGetWinPath(cchar *path)
{
    char            *result;

    if (path == 0 || *path == '\0') {
        path = ".";
    }
#if BIT_ROM
    result = mprNormalizePath(path);
#elif CYGWIN
{
    ssize   len;
    if ((len = cygwin_conv_path(CCP_POSIX_TO_WIN_A | CCP_ABSOLUTE, path, NULL, 0)) >= 0) {
        if ((result = mprAlloc(len)) == 0) {
            return 0;
        }
        cygwin_conv_path(CCP_POSIX_TO_WIN_A | CCP_ABSOLUTE, path, result, len);
        return result;
    } else {
        result = mprGetAbsPath(path);
    }
}
#else
    result = mprNormalizePath(path);
    mprMapSeparators(result, '\\');
#endif
    return result;
}


bool mprIsPathAbs(cchar *path)
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(path);
    return isAbsPath(fs, path);
}


bool mprIsPathDir(cchar *path)
{
    MprPath     info;

    return (mprGetPathInfo(path, &info) == 0 && info.isDir);
}


bool mprIsPathRel(cchar *path)
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(path);
    return !isAbsPath(fs, path);
}


bool mprIsPathSeparator(cchar *path, cchar c)
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(path);
    return isSep(fs, c);
}


/*
    Join paths. Returns a joined (normalized) path.
    If other is absolute, then return other. If other is null, empty or "." then return path.
    The separator is chosen to match the first separator found in either path. If none, it uses the default separator.
 */
char *mprJoinPath(cchar *path, cchar *other)
{
    MprFileSystem   *fs;
    char            *join, *drive, *cp;
    int             sep;

    fs = mprLookupFileSystem(path);
    if (other == NULL || *other == '\0' || strcmp(other, ".") == 0) {
        return sclone(path);
    }
    if (isAbsPath(fs, other)) {
        if (fs->hasDriveSpecs && !isFullPath(fs, other) && isFullPath(fs, path)) {
            /*
                Other is absolute, but without a drive. Use the drive from path.
             */
            drive = sclone(path);
            if ((cp = strchr(drive, ':')) != 0) {
                *++cp = '\0';
            }
            return sjoin(drive, other, NULL);
        } else {
            return mprNormalizePath(other);
        }
    }
    if (path == NULL || *path == '\0') {
        return mprNormalizePath(other);
    }
    if ((cp = firstSep(fs, path)) != 0) {
        sep = *cp;
    } else if ((cp = firstSep(fs, other)) != 0) {
        sep = *cp;
    } else {
        sep = defaultSep(fs);
    }
    if ((join = sfmt("%s%c%s", path, sep, other)) == 0) {
        return 0;
    }
    return mprNormalizePath(join);
}


/*
    Join an extension to a path. If path already has an extension, this call does nothing.
    The extension should not have a ".", but this routine is tolerant if it does.
 */
char *mprJoinPathExt(cchar *path, cchar *ext)
{
    MprFileSystem   *fs;
    char            *cp;

    fs = mprLookupFileSystem(path);
    if (ext == NULL || *ext == '\0') {
        return sclone(path);
    }
    cp = srchr(path, '.');
    if (cp && firstSep(fs, cp) == 0) {
        return sclone(path);
    }
    if (ext[0] == '.') {
        return sjoin(path, ext, NULL);
    } else {
        return sjoin(path, ".", ext, NULL);
    }
}


/*
    Make a directory with all necessary intervening directories.
 */
int mprMakeDir(cchar *path, int perms, int owner, int group, bool makeMissing)
{
    MprFileSystem   *fs;
    char            *parent;
    int             rc;

    fs = mprLookupFileSystem(path);

    if (mprPathExists(path, X_OK)) {
        return 0;
    }
    if (fs->makeDir(fs, path, perms, owner, group) == 0) {
        return 0;
    }
    if (makeMissing && !isRoot(fs, path)) {
        parent = mprGetPathParent(path);
        if ((rc = mprMakeDir(parent, perms, owner, group, makeMissing)) < 0) {
            return rc;
        }
        return fs->makeDir(fs, path, perms, owner, group);
    }
    return MPR_ERR_CANT_CREATE;
}


int mprMakeLink(cchar *path, cchar *target, bool hard)
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(path);
    if (mprPathExists(path, X_OK)) {
        return 0;
    }
    return fs->makeLink(fs, path, target, hard);
}


/*
    Normalize a path to remove redundant "./" and cleanup "../" and make separator uniform. Does not make an abs path.
    It does not map separators, change case, nor add drive specifiers.
 */
char *mprNormalizePath(cchar *pathArg)
{
    MprFileSystem   *fs;
    char            *path, *sp, *dp, *mark, **segments;
    ssize           len;
    int             addSep, i, segmentCount, hasDot, last, sep;

    if (pathArg == 0 || *pathArg == '\0') {
        return sclone("");
    }
    fs = mprLookupFileSystem(pathArg);

    /*
        Allocate one spare byte incase we need to break into segments. If so, will add a trailing "/" to make 
        parsing easier later.
     */
    len = slen(pathArg);
    if ((path = mprAlloc(len + 2)) == 0) {
        return NULL;
    }
    strcpy(path, pathArg);
    sep = (sp = firstSep(fs, path)) ? *sp : defaultSep(fs);

    /*
        Remove multiple path separators. Check if we have any "." characters and count the number of path segments
        Map separators to the first separator found
     */
    hasDot = segmentCount = 0;
    for (sp = dp = path; *sp; ) {
        if (isSep(fs, *sp)) {
            *sp = sep;
            segmentCount++;
            while (isSep(fs, sp[1])) {
                sp++;
            }
        } 
        if (*sp == '.') {
            hasDot++;
        }
        *dp++ = *sp++;
    }
    *dp = '\0';
    if (!sep) {
        sep = defaultSep(fs);
    }
    if (!hasDot && segmentCount == 0) {
        if (fs->hasDriveSpecs) {
            last = path[slen(path) - 1];
            if (last == ':') {
                path = sjoin(path, ".", NULL);
            }
        }
        return path;
    }

    if (dp > path && !isSep(fs, dp[-1])) {
        *dp++ = sep;
        *dp = '\0';
        segmentCount++;
    }

    /*
        Have dots to process so break into path segments. Add one incase we need have an absolute path with a drive-spec.
     */
    mprAssert(segmentCount > 0);
    if ((segments = mprAlloc(sizeof(char*) * (segmentCount + 1))) == 0) {
        return NULL;
    }

    /*
        NOTE: The root "/" for absolute paths will be stored as empty.
     */
    len = 0;
    for (i = 0, mark = sp = path; *sp; sp++) {
        if (isSep(fs, *sp)) {
            *sp = '\0';
            if (*mark == '.' && mark[1] == '\0' && segmentCount > 1) {
                /* Remove "."  However, preserve lone "." */
                mark = sp + 1;
                segmentCount--;
                continue;
            }
            if (*mark == '.' && mark[1] == '.' && mark[2] == '\0' && i > 0 && strcmp(segments[i-1], "..") != 0) {
                /* Erase ".." and previous segment */
                if (*segments[i - 1] == '\0' ) {
                    mprAssert(i == 1);
                    /* Previous segement is "/". Prevent escape from root */
                    segmentCount--;
                } else {
                    i--;
                    segmentCount -= 2;
                }
                mprAssert(segmentCount >= 0);
                mark = sp + 1;
                continue;
            }
            segments[i++] = mark;
            len += (sp - mark);
#if KEEP
            if (i == 1 && segmentCount == 1 && fs->hasDriveSpecs && strchr(mark, ':') != 0) {
                /*
                    Normally we truncate a trailing "/", but this is an absolute path with a drive spec (c:/). 
                 */
                segments[i++] = "";
                segmentCount++;
            }
#endif
            mark = sp + 1;
        }
    }

    if (--sp > mark) {
        segments[i++] = mark;
        len += (sp - mark);
    }
    mprAssert(i <= segmentCount);
    segmentCount = i;

    if (segmentCount <= 0) {
        return sclone(".");
    }

    addSep = 0;
    sp = segments[0];
    if (fs->hasDriveSpecs && *sp != '\0') {
        last = sp[slen(sp) - 1];
        if (last == ':') {
            /* This matches an original path of: "c:/" but not "c:filename" */
            addSep++;
        }
    }
#if BIT_WIN_LIKE
    if (strcmp(segments[segmentCount - 1], " ") == 0) {
        segmentCount--;
    }
#endif
    if ((path = mprAlloc(len + segmentCount + 1)) == 0) {
        return NULL;
    }
    mprAssert(segmentCount > 0);

    /*
        First segment requires special treatment due to drive specs
     */
    dp = path;
    strcpy(dp, segments[0]);
    dp += slen(segments[0]);

    if (segmentCount == 1 && (addSep || (*segments[0] == '\0'))) {
        *dp++ = sep;
    }

    for (i = 1; i < segmentCount; i++) {
        *dp++ = sep;
        strcpy(dp, segments[i]);
        dp += slen(segments[i]);
    }
    *dp = '\0';
    return path;
}


void mprMapSeparators(char *path, int separator)
{
    MprFileSystem   *fs;
    char            *cp;

    fs = mprLookupFileSystem(path);
    for (cp = path; *cp; cp++) {
        if (isSep(fs, *cp)) {
            *cp = separator;
        }
    }
}


bool mprPathExists(cchar *path, int omode)
{
    MprFileSystem  *fs;

    if (path == 0 || *path == '\0') {
        return 0;
    }
    fs = mprLookupFileSystem(path);
    return fs->accessPath(fs, path, omode);
}


char *mprReadPathContents(cchar *path, ssize *lenp)
{
    MprFile     *file;
    MprPath     info;
    ssize       len;
    char        *buf;

    if ((file = mprOpenFile(path, O_RDONLY | O_BINARY, 0)) == 0) {
        mprError("Can't open %s", path);
        return 0;
    }
    if (mprGetPathInfo(path, &info) < 0) {
        mprCloseFile(file);
        return 0;
    }
    len = (ssize) info.size;
    if ((buf = mprAlloc(len + 1)) == 0) {
        mprCloseFile(file);
        return 0;
    }
    if (mprReadFile(file, buf, len) != len) {
        mprCloseFile(file);
        return 0;
    }
    buf[len] = '\0';
    if (lenp) {
        *lenp = len;
    }
    mprCloseFile(file);
    return buf;
}


int mprRenamePath(cchar *from, cchar *to)
{
    return rename(from, to);
}


char *mprReplacePathExt(cchar *path, cchar *ext)
{
    return mprJoinPathExt(mprTrimPathExt(path), ext);
}


/*
    Resolve paths in the neighborhood of this path. Resolve operates like join, except that it joins the 
    given paths to the directory portion of the current ("this") path. For example: 
    Path("/usr/bin/ejs/bin").resolve("lib") will return "/usr/lib/ejs/lib". i.e. it will return the
    sibling directory "lib".

    Resolve operates by determining a virtual current directory for this Path object. It then successively 
    joins the given paths to the directory portion of the current result. If the next path is an absolute path, 
    it is used unmodified.  The effect is to find the given paths with a virtual current directory set to the 
    directory containing the prior path.

    Resolve is useful for creating paths in the region of the current path and gracefully handles both 
    absolute and relative path segments.

    Returns a joined (normalized) path.
    If path is absolute, then return path. If path is null, empty or "." then return path.
 */
char *mprResolvePath(cchar *base, cchar *path)
{
    MprFileSystem   *fs;
    char            *join, *drive, *cp, *dir;

    fs = mprLookupFileSystem(base);
    if (path == NULL || *path == '\0' || strcmp(path, ".") == 0) {
        return sclone(base);
    }
    if (isAbsPath(fs, path)) {
        if (fs->hasDriveSpecs && !isFullPath(fs, path) && isFullPath(fs, base)) {
            /*
                Other is absolute, but without a drive. Use the drive from base.
             */
            drive = sclone(base);
            if ((cp = strchr(drive, ':')) != 0) {
                *++cp = '\0';
            }
            return sjoin(drive, path, NULL);
        }
        return mprNormalizePath(path);
    }
    if (base == NULL || *base == '\0') {
        return mprNormalizePath(path);
    }
    dir = mprGetPathDir(base);
    if ((join = sfmt("%s/%s", dir, path)) == 0) {
        return 0;
    }
    return mprNormalizePath(join);
}


/*
    Compare two file path to determine if they point to the same file.
 */
int mprSamePath(cchar *path1, cchar *path2)
{
    MprFileSystem   *fs;
    cchar           *p1, *p2;

    fs = mprLookupFileSystem(path1);

    /*
        Convert to absolute (normalized) paths to compare. 
        MOB - resolve symlinks.
     */
    if (!isFullPath(fs, path1)) {
        path1 = mprGetAbsPath(path1);
    } else {
        path1 = mprNormalizePath(path1);
    }
    if (!isFullPath(fs, path2)) {
        path2 = mprGetAbsPath(path2);
    } else {
        path2 = mprNormalizePath(path2);
    }
    if (fs->caseSensitive) {
        for (p1 = path1, p2 = path2; *p1 && *p2; p1++, p2++) {
            if (*p1 != *p2 && !(isSep(fs, *p1) && isSep(fs, *p2))) {
                break;
            }
        }
    } else {
        for (p1 = path1, p2 = path2; *p1 && *p2; p1++, p2++) {
            if (tolower((uchar) *p1) != tolower((uchar) *p2) && !(isSep(fs, *p1) && isSep(fs, *p2))) {
                break;
            }
        }
    }
    return (*p1 == *p2);
}


/*
    Compare two file path to determine if they point to the same file.
 */
int mprSamePathCount(cchar *path1, cchar *path2, ssize len)
{
    MprFileSystem   *fs;
    cchar           *p1, *p2;

    fs = mprLookupFileSystem(path1);

    /*
        Convert to absolute paths to compare. 
        MOB - resolve symlinks.
     */
    if (!isFullPath(fs, path1)) {
        path1 = mprGetAbsPath(path1);
    }
    if (!isFullPath(fs, path2)) {
        path2 = mprGetAbsPath(path2);
    }
    if (fs->caseSensitive) {
        for (p1 = path1, p2 = path2; *p1 && *p2 && len > 0; p1++, p2++, len--) {
            if (*p1 != *p2 && !(isSep(fs, *p1) && isSep(fs, *p2))) {
                break;
            }
        }
    } else {
        for (p1 = path1, p2 = path2; *p1 && *p2 && len > 0; p1++, p2++, len--) {
            if (tolower((uchar) *p1) != tolower((uchar) *p2) && !(isSep(fs, *p1) && isSep(fs, *p2))) {
                break;
            }
        }
    }
    return len == 0;
}


void mprSetAppPath(cchar *path)
{ 
    MPR->appPath = sclone(path);
    MPR->appDir = mprGetPathDir(MPR->appPath);
}


static char* checkPath(cchar *path, int flags) 
{
    MprPath     info;
    int         access;

    access = (flags & (MPR_SEARCH_EXE | MPR_SEARCH_DIR)) ? X_OK : R_OK;

    if (mprPathExists(path, access)) {
        mprGetPathInfo(path, &info);
        if (flags & MPR_SEARCH_DIR && info.isDir) {
            mprLog(4, "mprSearchForFile: found %s", path);
            return sclone(path);
        }
        if (info.isReg) {
            mprLog(4, "mprSearchForFile: found %s", path);
            return sclone(path);
        }
    }
    return 0;
}


char *mprSearchPath(cchar *file, int flags, cchar *search, ...)
{
    va_list     args;
    char        *result, *path, *dir, *nextDir, *tok;

    va_start(args, search);

    mprLog(5, "mprSearchForFile: %s", file);

    if ((result = checkPath(file, flags)) != 0) {
        return result;
    }
    if ((flags & MPR_SEARCH_EXE) && *BIT_EXE) {
        if ((result = checkPath(mprJoinPathExt(file, BIT_EXE), flags)) != 0) {
            return result;
        }
    }
    for (nextDir = (char*) search; nextDir; nextDir = va_arg(args, char*)) {
        tok = NULL;
        nextDir = sclone(nextDir);
        dir = stok(nextDir, MPR_SEARCH_SEP, &tok);
        while (dir && *dir) {
            mprLog(5, "mprSearchForFile: %s in search path %s", file, dir);
            path = mprJoinPath(dir, file);
            if ((result = checkPath(path, flags)) != 0) {
                return mprNormalizePath(result);
            }
            if ((flags & MPR_SEARCH_EXE) && *BIT_EXE) {
                if ((result = checkPath(mprJoinPathExt(path, BIT_EXE), flags)) != 0) {
                    return mprNormalizePath(result);
                }
            }
            dir = stok(0, MPR_SEARCH_SEP, &tok);
        }
    }
    va_end(args);
    return 0;
}


/*
    This normalizes a path. Returns a normalized path according to flags. Default is absolute. 
    if MPR_PATH_NATIVE_SEP is specified in the flags, map separators to the native format.
 */
char *mprTransformPath(cchar *path, int flags)
{
    char    *result;

#if CYGWIN
    if (flags & MPR_PATH_ABS) {
        if (flags & MPR_PATH_WIN) {
            result = mprGetWinPath(path);
        } else {
            result = mprGetAbsPath(path);
        }
#else
    if (flags & MPR_PATH_ABS) {
        result = mprGetAbsPath(path);

#endif
    } else if (flags & MPR_PATH_REL) {
        result = mprGetRelPath(path, 0);

    } else {
        result = mprNormalizePath(path);
    }
    if (flags & MPR_PATH_NATIVE_SEP) {
#if BIT_WIN_LIKE
        mprMapSeparators(result, '\\');
#elif CYGWIN
        mprMapSeparators(result, '/');
#endif
    }
    return result;
}


char *mprTrimPathExt(cchar *path)
{
    MprFileSystem   *fs;
    char            *cp, *result;

    fs = mprLookupFileSystem(path);
    result = sclone(path);
    if ((cp = srchr(result, '.')) != NULL) {
        if (firstSep(fs, cp) == 0) {
            *cp = '\0';
        }
    } 
    return result;
}


char *mprTrimPathDrive(cchar *path)
{
    MprFileSystem   *fs;
    char            *cp, *endDrive;

    fs = mprLookupFileSystem(path);
    if (fs->hasDriveSpecs) {
        cp = firstSep(fs, path);
        endDrive = strchr(path, ':');
        if (endDrive && (cp == NULL || endDrive < cp)) {
            return sclone(++endDrive);
        }
    }
    return sclone(path);
}


ssize mprWritePathContents(cchar *path, cchar *buf, ssize len, int mode)
{
    MprFile     *file;

    if (mode == 0) {
        mode = 0644;
    }
    if (len < 0) {
        len = slen(buf);
    }
    if ((file = mprOpenFile(path, O_WRONLY | O_TRUNC | O_CREAT | O_BINARY, mode)) == 0) {
        mprError("Can't open %s", path);
        return MPR_ERR_CANT_OPEN;
    }
    if (mprWriteFile(file, buf, len) != len) {
        mprError("Can't write %s", path);
        mprCloseFile(file);
        return MPR_ERR_CANT_WRITE;
    }
    mprCloseFile(file);
    return len;
}


/*
    @copy   default
    
    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
    
    This software is distributed under commercial and open source licenses.
    You may use the GPL open source license described below or you may acquire 
    a commercial license from Embedthis Software. You agree to be fully bound 
    by the terms of either license. Consult the LICENSE.TXT distributed with 
    this software for full details.
    
    This software is open source; you can redistribute it and/or modify it 
    under the terms of the GNU General Public License as published by the 
    Free Software Foundation; either version 2 of the License, or (at your 
    option) any later version. See the GNU General Public License for more 
    details at: http://embedthis.com/downloads/gplLicense.html
    
    This program is distributed WITHOUT ANY WARRANTY; without even the 
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
    
    This GPL license does NOT permit incorporating this software into 
    proprietary programs. If you are unable to comply with the GPL, you must
    acquire a commercial license to use this software. Commercial licenses 
    for this software and support services are available from Embedthis 
    Software at http://embedthis.com 
    
    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
