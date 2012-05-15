/*
    mprReadline.c - 

    Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

#if BIT_CC_EDITLINE
    #include <histedit.h>
#endif

/*********************************** Locals ***********************************/

#if BIT_CC_EDITLINE
static History *cmdHistory;
static EditLine *eh; 
static cchar *prompt;
#endif

/**************************** Forward Declarations ****************************/

/************************************* Code ***********************************/

#if BIT_CC_EDITLINE


EditLine *mprReadlineOpen(MprCtx ctx)
{
    EditLine    *e;
    HistEvent   ev; 

    cmdHistory = history_init(); 
    history(cmdHistory, &ev, H_SETSIZE, 100); 
    e = el_init("ejs", stdin, stdout, stderr); 
    el_set(e, EL_EDITOR, "vi");
    el_set(e, EL_HIST, history, cmdHistory);
    el_source(e, NULL);
    return e;
}


static cchar *issuePrompt(EditLine *e) {
    return prompt;
}


char *mprReadline(MprCtx ctx, cchar *msg) 
{ 
    HistEvent   ev; 
    cchar       *str; 
    char        *result;
    int         len, count; 
 
    if (eh == NULL) { 
        eh = mprReadlineOpen(ctx);
    }
    prompt = msg;
    el_set(eh, EL_PROMPT, issuePrompt);
    str = el_gets(eh, &count); 
    if (str && count > 0) { 
        result = strdup(str); 
        len = strlen(result);
        if (result[len - 1] == '\n') {
            result[len - 1] = '\0'; 
        }
        count = history(cmdHistory, &ev, H_ENTER, result); 
        return result; 
    }  
    return NULL; 
} 

#else /* BIT_CC_EDITLINE */

char *mprReadline(MprCtx ctx, cchar *msg)
{
    char    buf[MPR_MAX_STRING];

    printf("%s", msg);
    if (fgets(buf, sizeof(buf) - 1, stdin) == 0) {
        return NULL;
    }
    return strdup(buf);
}
#endif /* BIT_CC_EDITLINE */

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
