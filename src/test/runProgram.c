/**
    runProgram.c - Test program for testCmd
  
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

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
    char    buf[256], *ep;
    int     i, len, exitCode, sofar;

#if BLD_UNIX_LIKE
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

        /* Echo the CMD_ENV environment variable value */
        ep = getenv("CMD_ENV");
        if (ep) {
            printf("CMD_ENV=%s\n", ep);
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
