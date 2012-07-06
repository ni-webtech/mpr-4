/**
    mprSsl.c -- Initialization for libmprssl. Load the SSL provider.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

#if BIT_FEATURE_SSL
/************************************ Code ************************************/
/*
    Module initialization entry point
 */
int mprSslInit(void *unused, MprModule *module)
{
    mprAssert(module);

#if BIT_FEATURE_MATRIXSSL
    if (mprCreateMatrixSslModule() < 0) {
        return MPR_ERR_CANT_OPEN;
    }
    MPR->socketService->defaultProvider = sclone("matrixssl");
#endif
#if BIT_FEATURE_OPENSSL
    if (mprCreateOpenSslModule() < 0) {
        return MPR_ERR_CANT_OPEN;
    }
    MPR->socketService->defaultProvider = sclone("openssl");
#endif
    return 0;
}

#endif /* BLD_FEATURE_SSL */

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
