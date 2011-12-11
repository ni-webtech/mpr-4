/**
    mprSsl.c -- Load and manage the SSL providers.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"
#include    "mprSsl.h"

#if BLD_FEATURE_SSL
/************************************ Code ************************************/
/*
    Load the ssl provider
 */
static MprModule *loadSsl(bool lazy)
{
    MprModule   *mp;

    if (MPR->flags & MPR_SSL_PROVIDER_LOADED) {
        return mprLookupModule("sslModule");
    }
#if UNUSED
    mprLog(MPR_CONFIG, "Activating the SSL provider");
#endif
#if BLD_FEATURE_OPENSSL
    /*
        NOTE: preference given to open ssl if both are enabled
     */
    mprLog(4, "Loading OpenSSL module");
    if (mprCreateOpenSslModule(lazy) < 0) {
        return 0;
    }
#elif BLD_FEATURE_MATRIXSSL
    mprLog(4, "Loading MatrixSSL module");
    if (mprCreateMatrixSslModule(lazy) < 0) {
        return 0;
    }
#endif
    if ((mp = mprCreateModule("sslModule", NULL, NULL, NULL)) == 0) {
        return 0;
    }
    MPR->flags |= MPR_SSL_PROVIDER_LOADED;
    return mp;
}


MprModule *mprLoadSsl(bool lazy)
{
    return loadSsl(lazy);
}


/*
    Loadable module interface. 
 */
MprModule *mprSslInit(cchar *path)
{
    return loadSsl(1);
}


static void defaultSslManager(MprSsl *ssl, int flags) 
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(ssl->key);
        mprMark(ssl->cert);
        mprMark(ssl->keyFile);
        mprMark(ssl->certFile);
        mprMark(ssl->caFile);
        mprMark(ssl->caPath);
        mprMark(ssl->ciphers);
    }
}


/*
    Create a new Ssl context object
 */
MprSsl *mprCreateSsl()
{
    MprSsl      *ssl;

    if ((ssl = mprAllocObj(MprSsl, defaultSslManager)) == 0) {
        return 0;
    }
    ssl->ciphers = sclone(MPR_DEFAULT_CIPHER_SUITE);
    ssl->protocols = MPR_PROTO_SSLV3 | MPR_PROTO_TLSV1;
    ssl->verifyDepth = 6;
    return ssl;
}


void mprConfigureSsl(MprSsl *ssl)
{
    MprSocketProvider   *provider;

    if (ssl == 0 || ssl->configured) {
        return;
    }
    provider = MPR->socketService->secureProvider;
    if (provider) {
        provider->configureSsl(ssl);
        ssl->configured = 1;
    } else {
        mprError("Secure socket provider not loaded");
    }
}


void mprSetSslCiphers(MprSsl *ssl, cchar *ciphers)
{
    mprAssert(ssl);
    
    ssl->ciphers = sclone(ciphers);
}


void mprSetSslKeyFile(MprSsl *ssl, cchar *keyFile)
{
    mprAssert(ssl);
    
    ssl->keyFile = sclone(keyFile);
}


void mprSetSslCertFile(MprSsl *ssl, cchar *certFile)
{
    mprAssert(ssl);
    
    ssl->certFile = sclone(certFile);
}


void mprSetSslCaFile(MprSsl *ssl, cchar *caFile)
{
    mprAssert(ssl);
    
    ssl->caFile = sclone(caFile);
}


void mprSetSslCaPath(MprSsl *ssl, cchar *caPath)
{
    mprAssert(ssl);
    
    ssl->caPath = sclone(caPath);
}


void mprSetSslProtocols(MprSsl *ssl, int protocols)
{
    ssl->protocols = protocols;
}


void mprSetSocketSslConfig(MprSocket *sp, MprSsl *ssl)
{
    if (sp->sslSocket) {
        sp->sslSocket->ssl = ssl;
    }
}


void mprVerifySslClients(MprSsl *ssl, bool on)
{
    ssl->verifyClient = on;
}


#else /* SSL */

/*
    Stubs
 */
MprModule *mprLoadSsl(bool lazy)
{
    return 0;
}

MprModule *mprSslInit(cchar *path)
{
    return 0;
}


MprSsl *mprCreateSsl()
{
    return 0;
}


void mprConfigureSsl(MprSsl *ssl)
{
}


void mprSetSslCiphers(MprSsl *ssl, cchar *ciphers)
{
}


void mprSetSslKeyFile(MprSsl *ssl, cchar *keyFile)
{
}


void mprSetSslCertFile(MprSsl *ssl, cchar *certFile)
{
}


void mprSetSslCaFile(MprSsl *ssl, cchar *caFile)
{
}


void mprSetSslCaPath(MprSsl *ssl, cchar *caPath)
{
}


void mprSetSslProtocols(MprSsl *ssl, int protocols)
{
}


void mprSetSocketSslConfig(MprSocket *sp, MprSsl *ssl)
{
}


void mprVerifySslClients(MprSsl *ssl, bool on)
{
}


#endif /* SSL */


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
