/**
    mprSsl.c -- Initialization for libmprssl. Load the SSL provider.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

#if BIT_PACK_SSL
/************************************ Code ************************************/
/*
    Module initialization entry point
 */
int mprSslInit(void *unused, MprModule *module)
{
    mprAssert(module);

#if BIT_PACK_MATRIXSSL
    if (mprCreateMatrixSslModule() < 0) {
        return MPR_ERR_CANT_OPEN;
    }
    MPR->socketService->defaultProvider = sclone("matrixssl");
#endif
#if BIT_PACK_OPENSSL
    if (mprCreateOpenSslModule() < 0) {
        return MPR_ERR_CANT_OPEN;
    }
    MPR->socketService->defaultProvider = sclone("openssl");
#endif
    return 0;
}

#endif /* BLD_PACK_SSL */

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
