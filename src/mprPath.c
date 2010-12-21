/**
    mprPath.c - Path (filename) services.

    This modules provides cross platform path name services.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/****************************** Forward Declarations **************************/
/*
    Find the first separator in the path
 */
#if BLD_UNIX_LIKE
    #define firstSep(fs, path)      strchr(path, fs->separators[0])
#else
    #define firstSep(fs, path)      strpbrk(path, fs->separators)
#endif

#define defaultSep(fs)              (fs->separators[0])

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


static MPR_INLINE bool isAbsPath(MprFileSystem *fs, cchar *path) 
{
    char    *cp, *endDrive;

    mprAssert(fs);
    mprAssert(path);

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


static MPR_INLINE bool isFullPath(MprFileSystem *fs, cchar *path) 
{
    char    *cp, *endDrive;

    mprAssert(fs);
    mprAssert(path);

    if (fs->hasDriveSpecs) {
        cp = firstSep(fs, path);
        endDrive = strchr(path, ':');
        if (endDrive && cp && &endDrive[1] == cp) {
            return 1;
        }
    } else {
        if (isSep(fs, path[0])) {
            return 1;
        }
    }
    return 0;
}


static MPR_INLINE bool isRoot(MprFileSystem *fs, cchar *path) 
{
    char    *cp;

    if (isFullPath(fs, path)) {
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

    for (cp = (char*) &path[strlen(path)] - 1; cp >= path; cp--) {
        if (isSep(fs, *cp)) {
            return cp;
        }
    }
    return 0;
}

/************************************ Code ************************************/
/*
    This copies the filename at the designated path
 */
int mprCopyPath(cchar *fromName, cchar *toName, int mode)
{
    MprFile     *from, *to;
    ssize       count;
    char        buf[MPR_BUFSIZE];

    if ((from = mprOpen(fromName, O_RDONLY | O_BINARY, 0)) == 0) {
        mprError("Can't open %s", fromName);
        return MPR_ERR_CANT_OPEN;
    }
    if ((to = mprOpen(toName, O_WRONLY | O_TRUNC | O_CREAT | O_BINARY, mode)) == 0) {
        mprError("Can't open %s", toName);
        return MPR_ERR_CANT_OPEN;
    }
    while ((count = mprRead(from, buf, sizeof(buf))) > 0) {
        mprWrite(to, buf, count);
    }
    mprFree(from);
    mprFree(to);
    return 0;
}


int mprDeletePath(cchar *path)
{
    MprFileSystem   *fs;

    if (path == NULL || *path == '\0') {
        return MPR_ERR_CANT_ACCESS;
    }
    fs = mprLookupFileSystem(path);
    if (!mprPathExists(path, F_OK)) {
        return 0;
    }
    return fs->deletePath(fs, path);
}


/*
    Return an absolute (normalized) path.
 */
char *mprGetAbsPath(cchar *pathArg)
{
    MprFileSystem   *fs;
    char            *path;

    if (pathArg == 0 || *pathArg == '\0') {
        pathArg = ".";
    }

#if BLD_FEATURE_ROMFS
    return mprGetNormalizedPath(pathArg);
#endif

    fs = mprLookupFileSystem(pathArg);
    if (isFullPath(fs, pathArg)) {
        return mprGetNormalizedPath(pathArg);
    }

#if BLD_WIN_LIKE && !WINCE
{
    char    buf[MPR_MAX_PATH];
    GetFullPathName(pathArg, sizeof(buf) - 1, buf, NULL);
    buf[sizeof(buf) - 1] = '\0';
    path = mprGetNormalizedPath(buf);
}
#elif VXWORKS
{
    char    *dir;
    if (hasDrive(fs, pathArg)) {
        dir = mprGetCurrentPath();
        path = mprJoinPath(dir, &strchr(pathArg, ':')[1]);
        mprFree(dir);

    } else {
        if (isAbsPath(fs, pathArg)) {
            /*
                Path is absolute, but without a drive. Use the current drive.
             */
            dir = mprGetCurrentPath();
            path = mprJoinPath(dir, pathArg);
            mprFree(dir);
        } else {
            dir = mprGetCurrentPath();
            path = mprJoinPath(dir, pathArg);
            mprFree(dir);
        }
    }
}
#else
{
    char   *dir;
    dir = mprGetCurrentPath();
    path = mprJoinPath(dir, pathArg);
    mprFree(dir);
}
#endif
    return path;
}


/*
    This will return a fully qualified absolute path for the current working directory.
 */
char *mprGetCurrentPath()
{
    MprFileSystem   *fs;
    char            dir[MPR_MAX_PATH];

    fs = mprLookupFileSystem(dir);
    if (getcwd(dir, sizeof(dir)) == 0) {
        return mprGetAbsPath("/");
    }

#if VXWORKS
{
    char    sep[2];

    /*
        Vx will sometimes just return a drive with no path.
     */
    if (firstSep(fs, dir) == NULL) {
        sep[0] = defaultSep(fs);
        sep[1] = '\0';
        return sjoin(dir, sep, NULL);
    }
}
#elif BLD_WIN_LIKE
    mprMapSeparators(dir, fs->separators[0]);
#endif
    return sclone(dir);
}


char *mprGetNativePath(cchar *path)
{
    return mprGetTransformedPath(path, MPR_PATH_NATIVE_SEP);
}


/*
    Return the last portion of a pathname. The separators are not mapped and the path is not cleaned.
 */
char *mprGetPathBase(cchar *path)
{
    MprFileSystem   *fs;
    char            *cp;

    fs = mprLookupFileSystem(path);
    cp = (char*) lastSep(fs, path);
    if (cp == 0) {
        return sclone(path);
    } 
    if (cp == path) {
        if (cp[1] == '\0') {
            return sclone(path);
        }
    } else {
        if (cp[1] == '\0') {
            return sclone("");
        }
    }
    return sclone(&cp[1]);
}


/*
    Return the directory portion of a pathname into the users buffer.
 */
char *mprGetPathDir(cchar *path)
{
    MprFileSystem   *fs;
    cchar           *cp;
    char            *result;
    ssize          len;

    mprAssert(path);

    if (*path == '\0') {
        return sclone(path);
    }

    fs = mprLookupFileSystem(path);
    len = strlen(path);
    cp = &path[len - 1];

    /*
        Step back over trailing slashes
     */
    while (cp > path && isSep(fs, *cp)) {
        cp--;
    }
    for (; cp > path && !isSep(fs, *cp); cp--) {
        ;
    }
    if (cp == path) {
        if (!isSep(fs, *cp)) {
            /* No slashes found, parent is current dir */
            return sclone(".");
        }
        return sclone(fs->root);
    }
    len = (cp - path);
    result = mprAlloc(len + 1);
    mprMemcpy(result, len + 1, path, len);
    result[len] = '\0';
    return result;
}


#if BLD_WIN_LIKE
MprList *mprGetPathFiles(cchar *dir, bool enumDirs)
{
    HANDLE          h;
    MprDirEntry     *dp;
    MprPath         fileInfo;
    MprList         *list;
	cchar			*seps;
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
        mprFree(path);
        return 0;
    }
    list = mprCreateList();

    do {
        if (findData.cFileName[0] == '.' && (findData.cFileName[1] == '\0' || findData.cFileName[1] == '.')) {
            continue;
        }
        if (enumDirs || !(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            dp = mprAlloc(sizeof(MprDirEntry));
            if (dp == 0) {
                mprFree(path);
                return 0;
            }
            dp->name = sclone(findData.cFileName);
            if (dp->name == 0) {
                mprFree(path);
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
        }
    } while (FindNextFile(h, &findData) != 0);

    mprFree(path);
    FindClose(h);
    return list;
}
#endif /* WIN */


#if BLD_UNIX_LIKE || VXWORKS || CYGWIN
MprList *mprGetPathFiles(cchar *path, bool enumDirs)
{
    DIR             *dir;
    MprPath         fileInfo;
    MprList         *list;
    struct dirent   *dirent;
    MprDirEntry     *dp;
    char            *fileName;
    int             rc;

    dp = 0;

    dir = opendir((char*) path);
    if (dir == 0) {
        return 0;
    }
    list = mprCreateList();

    while ((dirent = readdir(dir)) != 0) {
        if (dirent->d_name[0] == '.' && (dirent->d_name[1] == '\0' || dirent->d_name[1] == '.')) {
            continue;
        }
        fileName = mprJoinPath(path, dirent->d_name);
        rc = mprGetPathInfo(fileName, &fileInfo);
        mprFree(fileName);
        if (enumDirs || (rc == 0 && !fileInfo.isDir)) { 
            dp = mprAlloc(sizeof(MprDirEntry));
            if (dp == 0) {
                return 0;
            }
            dp->name = sclone(dirent->d_name);
            if (dp->name == 0) {
                return 0;
            }
            if (rc == 0) {
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
    }
    closedir(dir);
    return list;
}
#endif


char *mprGetPathLink(cchar *path)
{
    MprFileSystem  *fs;

    fs = mprLookupFileSystem(path);
    return fs->getPathLink(fs, path);
}


/*
    Return the extension portion of a pathname.
    Return the extension without the "."
 */
cchar *mprGetPathExtension(cchar *path)
{
    MprFileSystem  *fs;
    char            *cp;

    if ((cp = strrchr(path, '.')) != NULL) {
        fs = mprLookupFileSystem(path);
        if (firstSep(fs, cp) == 0) {
            return ++cp;
        }
    } 
    return 0;
}


int mprGetPathInfo(cchar *path, MprPath *info)
{
    MprFileSystem  *fs;

    fs = mprLookupFileSystem(path);
    return fs->getPathInfo(fs, path, info);
}


/*
    GetPathParent is smarter than GetPathDir which operates purely textually on the path. GetPathParent will convert
    relative paths to absolute to determine the parent directory.
 */
char *mprGetPathParent(cchar *path)
{
    MprFileSystem   *fs;
    char            *dir, *parent;

    fs = mprLookupFileSystem(path);

    if (path == 0 || path[0] == '\0') {
        return mprGetAbsPath(".");
    }
    if (firstSep(fs, path) == NULL) {
        /*
            No parents in the path, so convert to absolute
         */
        dir = mprGetAbsPath(path);
        parent = mprGetPathDir(dir);
        mprFree(dir);
        return parent;
    }
    return mprGetPathDir(path);
}


char *mprGetPortablePath(cchar *path)
{
    MprFileSystem   *fs;
    char            *result, *cp;

    fs = mprLookupFileSystem(path);
    result = mprGetTransformedPath(path, 0);
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
char *mprGetRelPath(cchar *pathArg)
{
    MprFileSystem   *fs;
    char            home[MPR_MAX_FNAME], *hp, *cp, *result, *tmp, *path;
    ssize           len;
    int             homeSegments, i, commonSegments, sep;

    fs = mprLookupFileSystem(pathArg);
    
    if (pathArg == 0 || *pathArg == '\0') {
        return sclone(".");
    }

    /*
        Must clean to ensure a minimal relative path result.
     */
    path = tmp = mprGetNormalizedPath(pathArg);

    if (!isAbsPath(fs, path)) {
        return path;
    }
    sep = (cp = firstSep(fs, path)) ? *cp : defaultSep(fs);
    
#if BLD_WIN_LIKE && !WINCE
{
    char    apath[MPR_MAX_FNAME];
    GetFullPathName(path, sizeof(apath) - 1, apath, NULL);
    apath[sizeof(apath) - 1] = '\0';
    path = apath;
    mprMapSeparators(path, sep);
}
#endif
    /*
        Get the working directory. Ensure it is null terminated and leave room to append a trailing separators
     */
    if (getcwd(home, sizeof(home)) == 0) {
        strcpy(home, ".");
    }
    home[sizeof(home) - 2] = '\0';
    len = strlen(home);

    /*
        Count segments in home working directory. Ignore trailing separators.
     */
    for (homeSegments = 0, cp = home; *cp; cp++) {
        if (isSep(fs, *cp) && cp[1]) {
            homeSegments++;
        }
    }

    /*
        Find portion of path that matches the home directory, if any. Start at -1 because matching root doesn't count.
     */
    commonSegments = -1;
    for (hp = home, cp = path; *hp && *cp; hp++, cp++) {
        if (isSep(fs, *hp)) {
            if (isSep(fs, *cp)) {
                commonSegments++;
            }
        } else if (fs->caseSensitive) {
            if (tolower((int) *hp) != tolower((int) *cp)) {
                break;
            }
        } else {
            if (*hp != *cp) {
                break;
            }
        }
    }
    mprAssert(commonSegments >= 0);

    /*
        Add one more segment if the last segment matches. Handle trailing separators
     */
    if ((isSep(fs, *hp) || *hp == '\0') && (isSep(fs, *cp) || *cp == '\0')) {
        commonSegments++;
    }
    if (isSep(fs, *cp)) {
        cp++;
    }
    
    hp = result = mprAlloc(homeSegments * 3 + strlen(path) + 2);
    for (i = commonSegments; i < homeSegments; i++) {
        *hp++ = '.';
        *hp++ = '.';
        *hp++ = defaultSep(fs);
    }
    if (*cp) {
        strcpy(hp, cp);
    } else if (hp > result) {
        /*
            Cleanup trailing separators ("../" is the end of the new path)
         */
        hp[-1] = '\0';
    } else {
        strcpy(result, ".");
    }
    mprMapSeparators(result, sep);
    mprFree(tmp);
    return result;
}


bool mprIsAbsPath(cchar *path)
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(path);
    return isAbsPath(fs, path);
}


bool mprIsRelPath(cchar *path)
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(path);
    return !isAbsPath(fs, path);
}


//  MOB -- should take a list of paths
/*
    Join paths. Returns a joined (normalized) path.
    If other is absolute, then return other. If other is null, empty or "." then return path.
    The separator is chosen to match the first separator found in either path. If none, it uses the default separator.
 */
char *mprJoinPath(cchar *path, cchar *other)
{
    MprFileSystem   *fs;
    char            *join, *result, *drive, *cp;
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
            result = sjoin(drive, other, NULL);
            mprFree(drive);
            return result;
        } else {
            return mprGetNormalizedPath(other);
        }
    }
    if (path == NULL || *path == '\0') {
        return mprGetNormalizedPath(other);
    }
    if ((cp = firstSep(fs, path)) != 0) {
        sep = *cp;
    } else if ((cp = firstSep(fs, other)) != 0) {
        sep = *cp;
    } else {
        sep = defaultSep(fs);
    }
    if ((join = mprAsprintf("%s%c%s", path, sep, other)) == 0) {
        return 0;
    }
    result = mprGetNormalizedPath(join);
    mprFree(join);
    return result;
}


/*
    Join an extension to a path. If path already has an extension, this call does nothing.
 */
char *mprJoinPathExt(cchar *path, cchar *ext)
{
    MprFileSystem   *fs;
    char            *cp;

    fs = mprLookupFileSystem(path);
    if (ext == NULL || *ext == '\0') {
        return sclone(path);
    }
    cp = strrchr(path, '.');
    if (cp && firstSep(fs, cp) == 0) {
        return sclone(path);
    }
    return sjoin(path, ext, NULL);
}


/*
    Make a directory with all necessary intervening directories.
 */
int mprMakeDir(cchar *path, int perms, bool makeMissing)
{
    MprFileSystem   *fs;
    char            *parent;
    int             rc;

    fs = mprLookupFileSystem(path);

    if (mprPathExists(path, X_OK)) {
        return 0;
    }
    if (fs->makeDir(fs, path, perms) == 0) {
        return 0;
    }
    if (makeMissing && !isRoot(fs, path)) {
        parent = mprGetPathParent(path);
        rc = mprMakeDir(parent, perms, makeMissing);
        mprFree(parent);
        return fs->makeDir(fs, path, perms);
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


char *mprGetTempPath(cchar *tempDir)
{
    MprFileSystem   *fs;
    MprFile         *file;
    char            *dir, *path;
    int             i, now;
    static int      tempSeed = 0;

    fs = mprLookupFileSystem(tempDir ? tempDir : (cchar*) "/");

    if (tempDir == 0) {
#if WINCE
        dir = sclone("/Temp");
#elif BLD_WIN_LIKE
        dir = sclone(getenv("TEMP"));
        mprMapSeparators(dir, defaultSep(fs));
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
        mprFree(path);
        path = mprAsprintf("%s/MPR_%d_%d_%d.tmp", dir, getpid(), now, ++tempSeed);
        file = mprOpen(path, O_CREAT | O_EXCL | O_BINARY, 0664);
        if (file) {
            mprFree(file);
            break;
        }
    }
    mprFree(dir);
    if (file == 0) {
        mprFree(path);
        return 0;
    }
    return path;
}


#if BLD_WIN_LIKE && FUTURE
/*
    Normalize to a cygwin path without a drive spec
 */
static char *toCygPath(cchar *path)
{
    Mpr     *mpr;
    char    *absPath, *result;
    int     len;

    mpr = mprGetMpr();

    absPath = NULL;
    if (!isFullPath(mpr, path)) {
        absPath = mprGetAbsPath(path);
        path = (cchar*) absPath;
    }
    mprAssert(isFullPath(mpr, path);
        
    if (fs->cygdrive) {
        len = strlen(fs->cygdrive);
        if (sncasecmp(fs->cygdrive, &path[2], len) == 0 && isSep(mpr, path[len+2])) {
            /*
                If path is like: "c:/cygdrive/c/..."
                Just strip the "c:" portion. Still validly qualified.
             */
            result = sclone(&path[len + 2]);

        } else {
            /*
                Path is like: "c:/some/other/path". Prepend "/cygdrive/c/"
             */
            result = mprAsprintf("%s/%c%s", fs->cygdrive, path[0], &path[2]);
            len = strlen(result);
            if (isSep(mpr, result[len-1])) {
                result[len-1] = '\0';
            }
        }
    } else {
        /*
            Best we can do is get a relative path
         */
        result = mprGetRelPath(pathArg);
    }
    mprFree(absPath);
    return result;
}


/*
    Convert from a cygwin path
 */
static char *fromCygPath(cchar *path)
{
    Mpr     *mpr;
    char    *buf, *result;
    int     len;

    mpr = mprGetMpr();

    if (isFullPath(mpr, path)) {
        return sclone(path);
    }
    if (fs->cygdrive) {
        len = strlen(fs->cygdrive);
        if (mprComparePath(mpr, fs->cygdrive, path, len) == 0 && isSep(mpr, path[len]) && 
                isalpha(path[len+1]) && isSep(mpr, path[len+2])) {
            /*
                Has a "/cygdrive/c/" style prefix
             */
            buf = mprAsprintf("%c:", path[len+1], &path[len + 2]);

        } else {
            /*
                Cygwin path. Prepend "c:/cygdrive"
             */
            buf = mprAsprintf("%s/%s", fs->cygdrive, path);
        }
        result = mprGetAbsPath(buf);
        mprFree(buf);

    } else {
        result = mprGetAbsPath(path);
    }
    mprMapSeparators(result, defaultSep(fs));
    return result;
}
#endif


//  MOB -- should this be mprNormalizePath?  apply to all APIs
/*
    Normalize a path to remove redundant "./" and cleanup "../" and make separator uniform. Does not make an abs path.
    It does not map separators nor change case. 
 */
char *mprGetNormalizedPath(cchar *pathArg)
{
    MprFileSystem   *fs;
    char            *dupPath, *path, *sp, *dp, *mark, **segments;
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
    len = strlen(pathArg);
    if ((path = mprAlloc(len + 2)) == 0) {
        return NULL;
    }
    dupPath = path;
    strcpy(path, pathArg);
    sep = (sp = firstSep(fs, path)) ? *sp : defaultSep(fs);

    /*
        Remove multiple path separators. Check if we have any "." characters and count the number of path segments
        Map separators to the first separator found
     */
    hasDot = segmentCount = 0;
    for (sp = dp = mark = path; *sp; ) {
        if (isSep(fs, *sp)) {
            *sp = sep;
            segmentCount++;
            while (isSep(fs, sp[1])) {
                sp++;
            }
            mark = sp + 1;
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
            last = path[strlen(path) - 1];
            if (last == ':') {
                path = sjoin(path, ".", NULL);
                mprFree(dupPath);
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
        mprFree(dupPath);
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
        mprFree(dupPath);
        mprFree(segments);
        return sclone(".");
    }

    addSep = 0;
    sp = segments[0];
    if (fs->hasDriveSpecs && *sp != '\0') {
        last = sp[strlen(sp) - 1];
        if (last == ':') {
            /* This matches an original path of: "c:/" but not "c:filename" */
            addSep++;
        }
    }
#if BLD_WIN_LIKE
    if (strcmp(segments[segmentCount - 1], " ") == 0) {
        segmentCount--;
    }
#endif
    if ((path = mprAlloc(len + segmentCount + 1)) == 0) {
        mprFree(segments);
        mprFree(dupPath);
        return NULL;
    }
    mprAssert(segmentCount > 0);

    /*
        First segment requires special treatment due to drive specs
     */
    dp = path;
    strcpy(dp, segments[0]);
    dp += strlen(segments[0]);

    if (segmentCount == 1 && (addSep || (*segments[0] == '\0'))) {
        *dp++ = sep;
    }

    for (i = 1; i < segmentCount; i++) {
        *dp++ = sep;
        strcpy(dp, segments[i]);
        dp += strlen(segments[i]);
    }
    *dp = '\0';
    mprFree(dupPath);
    mprFree(segments);
    return path;
}


#if UNUSED
cchar *mprGetPathSeparator(cchar *path)
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(path);
    return fs->separators;
}
#endif


bool mprIsPathSeparator(cchar *path, cchar c)
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(path);
    return isSep(fs, c);
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


cchar *mprGetFirstPathSeparator(cchar *path) 
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(path);
    return firstSep(fs, path);
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

    fs = mprLookupFileSystem(path);

    return fs->accessPath(fs, path, omode);
}


//  MOB -- should take a list of paths
/*
    Resolve one path against another path. Returns a joined (normalized) path.
    If other is absolute, then return other. If other is null, empty or "." then return path.
 */
char *mprResolvePath(cchar *path, cchar *other)
{
    MprFileSystem   *fs;
    char            *join, *result, *drive, *cp, *dir;

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
            result = sjoin(drive, other, NULL);
            mprFree(drive);
            return result;
        }
        return mprGetNormalizedPath(other);
    }
    if (path == NULL || *path == '\0') {
        return mprGetNormalizedPath(other);
    }
    dir = mprGetPathDir(path);
    if ((join = mprAsprintf("%s/%s", dir, other)) == 0) {
        mprFree(dir);
        return 0;
    }
    mprFree(dir);
    result = mprGetNormalizedPath(join);
    mprFree(join);
    return result;
}


/*
    Compare two file path to determine if they point to the same file.
 */
int mprSamePath(cchar *path1, cchar *path2)
{
    MprFileSystem   *fs;
    cchar           *p1, *p2;
    int             rc;

    fs = mprLookupFileSystem(path1);

    /*
        Convert to absolute (normalized) paths to compare. TODO - resolve symlinks.
     */
    if (!isFullPath(fs, path1)) {
        path1 = mprGetAbsPath(path1);
    } else {
        path1 = mprGetNormalizedPath(path1);
    }
    if (!isFullPath(fs, path2)) {
        path2 = mprGetAbsPath(path2);
    } else {
        path2 = mprGetNormalizedPath(path2);
    }
    if (fs->caseSensitive) {
        for (p1 = path1, p2 = path2; *p1 && *p2; p1++, p2++) {
            if (*p1 != *p2 && !(isSep(fs, *p1) && isSep(fs, *p2))) {
                break;
            }
        }
    } else {
        for (p1 = path1, p2 = path2; *p1 && *p2; p1++, p2++) {
            if (tolower((int) *p1) != tolower((int) *p2) && !(isSep(fs, *p1) && isSep(fs, *p2))) {
                break;
            }
        }
    }
    rc = (*p1 == *p2);
    mprFree((char*) path1);
    mprFree((char*) path2);
    return rc;
}


/*
    Compare two file path to determine if they point to the same file.
 */
int mprSamePathCount(cchar *path1, cchar *path2, ssize len)
{
    MprFileSystem   *fs;
    char            *tmpPath1, *tmpPath2;
    cchar           *p1, *p2;

    fs = mprLookupFileSystem(path1);
    tmpPath1 = tmpPath2 = 0;

    /*
        Convert to absolute paths to compare. TODO - resolve symlinks.
     */
    if (!isFullPath(fs, path1)) {
        tmpPath1 = mprGetAbsPath(path1);
        path1 = tmpPath1;
    }
    if (!isFullPath(fs, path2)) {
        tmpPath2 = mprGetAbsPath(path2);
        path2 = tmpPath2;
    }
    if (fs->caseSensitive) {
        for (p1 = path1, p2 = path2; *p1 && *p2 && len > 0; p1++, p2++, len--) {
            if (*p1 != *p2 && !(isSep(fs, *p1) && isSep(fs, *p2))) {
                break;
            }
        }
    } else {
        for (p1 = path1, p2 = path2; *p1 && *p2 && len > 0; p1++, p2++, len--) {
            if (tolower((int) *p1) != tolower((int) *p2) && !(isSep(fs, *p1) && isSep(fs, *p2))) {
                break;
            }
        }
    }
    mprFree(tmpPath1);
    mprFree(tmpPath2);
    return len == 0;
}


char *mprSearchPath(cchar *file, int flags, cchar *search, ...)
{
    va_list     args;
    char        *path, *dir, *result, *nextDir, *tok;
    int         access;

    va_start(args, search);
    access = (flags & MPR_SEARCH_EXE) ? X_OK : R_OK;

    for (nextDir = (char*) search; nextDir; nextDir = va_arg(args, char*)) {

        if (strchr(nextDir, MPR_SEARCH_SEP_CHAR)) {
            tok = NULL;
            nextDir = sclone(nextDir);
            dir = stok(nextDir, MPR_SEARCH_SEP, &tok);
            while (dir && *dir) {
                mprLog(5, "mprSearchForFile: %s in directory %s", file, nextDir);
                path = mprJoinPath(dir, file);
                if (mprPathExists(path, R_OK)) {
                    mprLog(5, "mprSearchForFile: found %s", path);
                    result = mprGetNormalizedPath(path);
                    mprFree(path);
                    mprFree(nextDir);
                    return result;
                }
                mprFree(path);
                dir = stok(0, MPR_SEARCH_SEP, &tok);
            }
            mprFree(nextDir);

        } else {
            mprLog(5, "mprSearchForFile: %s in directory %s", file, nextDir);
            path = mprJoinPath(nextDir, file);
            if (mprPathExists(path, R_OK)) {
                mprLog(5, "mprSearchForFile: found %s", path);
                result = mprGetNormalizedPath(path);
                mprFree(path);
                return result;
            }
        }
    }
    va_end(args);
    return 0;
}


// TODO - handle cygwin paths and converting to and from.
/*
    This normalizes a path. Returns a normalized path according to flags. Default is absolute. 
    if MPR_PATH_NATIVE_SEP is specified in the flags, map separators to the native format.
 */
char *mprGetTransformedPath(cchar *path, int flags)
{
    MprFileSystem       *fs;
    char                *result;

    fs = mprLookupFileSystem(path);

#if BLD_WIN_LIKE && FUTURE
    if (flags & MPR_PATH_CYGWIN) {
        result = toCygPath(path, flags);
    } else {
        /*
            Issues here. "/" is ambiguous. Is this "c:/" or is it "c:/cygdrive/c" which may map to c:/cygwin/...
         */
        result = fromCygPath(path);
    }
#endif

    if (flags & MPR_PATH_ABS) {
        result = mprGetAbsPath(path);

    } else if (flags & MPR_PATH_REL) {
        result = mprGetRelPath(path);

    } else {
        result = mprGetNormalizedPath(path);
    }

#if BLD_WIN_LIKE
    if (flags & MPR_PATH_NATIVE_SEP) {
        mprMapSeparators(result, '\\');
    }
#endif
    return result;
}


/*
    Return the extension portion of a pathname.
 */
char *mprTrimPathExtension(cchar *path)
{
    MprFileSystem   *fs;
    char            *cp, *ext;

    fs = mprLookupFileSystem(path);
    ext = sclone(path);
    if ((cp = strrchr(ext, '.')) != NULL) {
        if (firstSep(fs, cp) == 0) {
            *cp = '\0';
        }
    } 
    return ext;
}


/*
    Get the path for the application executable. Tries to return an absolute path.
 */
char *mprGetAppPath()
{ 
    Mpr     *mpr;

    mpr = mprGetMpr();
    if (mpr->appPath) {
        return sclone(mpr->appPath);
    }

#if MACOSX
{
    char    path[MPR_MAX_PATH], pbuf[MPR_MAX_PATH];
    uint    size;
    int     len;

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
    mpr->appPath = mprGetAbsPath(pbuf);
    return sclone(mpr->appPath);
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
     mpr->appPath = mprGetAbsPath(pbuf);
     return sclone(mpr->appPath);
}
#elif BLD_UNIX_LIKE 
{
    char    pbuf[MPR_MAX_STRING], *path;
    int     len;
#if SOLARIS
    path = mprAsprintf("/proc/%i/path/a.out", getpid()); 
#else
    path = mprAsprintf("/proc/%i/exe", getpid()); 
#endif
    len = readlink(path, pbuf, sizeof(pbuf) - 1);
    if (len < 0) {
        mprFree(path);
        return mprGetAbsPath(".");
    }
    pbuf[len] = '\0';
    mprFree(path);
    mpr->appPath = mprGetAbsPath(pbuf);
    return sclone(mpr->appPath);
}
#elif BLD_WIN_LIKE
{
    char    pbuf[MPR_MAX_PATH];

    if (GetModuleFileName(0, pbuf, sizeof(pbuf) - 1) <= 0) {
        return 0;
    }
    mpr->appPath = mprGetAbsPath(pbuf);
    return sclone(mpr->appPath);
}
#else
    mpr->appPath = mprGetCurrentPath();
    return sclone(mpr->appPath);
#endif
}

 
/*
    Get the directory containing the application executable. Tries to return an absolute path.
 */
char *mprGetAppDir()
{ 
    Mpr     *mpr;
    char    *path;

    mpr = mprGetMpr();
    if (mpr->appDir == 0) {
        path = sclone(mprGetAppPath());
        mpr->appDir = mprGetPathDir(path);
        mprFree(path);
    }
    return sclone(mpr->appDir); 
} 

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
