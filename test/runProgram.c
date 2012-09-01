/**
    runProgram.c - Test program for testCmd
  
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#define _CRT_SECURE_NO_WARNINGS

#include "bit.h"

#if VXWORKS
    #ifndef _VSB_CONFIG_FILE
        #define _VSB_CONFIG_FILE "vsbConfig.h"
    #endif
#endif

#include <stdio.h>
#include <stdlib.h>

#if BIT_UNIX_LIKE
#include <signal.h>
#endif

#if _WIN32
#include    <io.h>
#define read _read
#define write _write
#else
#include <unistd.h>
#endif

/************************************ Code ************************************/

int main(int argc, char* argv[])
{
    char    buf[256], *cp;
    int     i, len, exitCode, sofar;

#if BIT_UNIX_LIKE
    signal(SIGPIPE, SIG_IGN);
#endif
#if TRACE_PROGRESS
    FILE    *fp;
    FILE *fp = fopen("/tmp/r.log", O_CREAT|O_TRUNC|O_WRONLY, 0664);
    fprintf(fp, "runProgram: argc %d\n", argc);
    for (i = 0; i < argc; i++) {
        fprintf(fp, "runProgram: arg[%d] = %s\n", i, argv[i]);
    }
    fclose(fp);
#endif

    if (argc < 2) {
        fprintf(stderr, "Usage: runProgram exitCode args...\n");
        exit(3);
    }
    exitCode = atoi(argv[1]);

    if (exitCode != 99) {
        /*
            Echo the args
         */
        for (i = 2; i < argc; ) {
            printf("%s", argv[i]);
            if (++i != argc) {
                printf(" ");
            }
        }
        printf("\n");
#if UNUSED && KEEP
        for (i = 0, ep = (char**) environ; ep && *ep; ep++) {
            printf("ENV[%d] = %s\n", i, ep);
        }
#endif
        /* Echo the CMD_ENV environment variable value */
        cp = getenv("CMD_ENV");
        if (cp) {
            printf("CMD_ENV=%s\n", cp);
        } else {
            printf("NOT FOUND CMD_ENV\n");
        }
        fflush(stderr);
        fflush(stdout);
    }

    /*
        Read the input
     */
    sofar = 0;
    while ((len = (int) read(0, buf, sizeof(buf))) > 0) {
        sofar += (int) write(1, buf, len);
        buf[len] = '\0';
    }
    if (exitCode != 99) {
        printf("END\n");
    }
    fflush(stdout);
    return exitCode;
}

/*
    @copy   default

    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.

    This software is distributed under commercial and open source licenses.
    You may use the Embedthis Open Source license or you may acquire a 
    commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.md distributed with
    this software for full details and other copyrights.

    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
