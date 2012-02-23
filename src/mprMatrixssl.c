/*
    mprMatrixssl.c -- Support for secure sockets via MatrixSSL

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "buildConfig.h"

#if BLD_FEATURE_MATRIXSSL
 #include    "matrixsslApi.h"

/*
    Matrixssl defines int32, uint32, int64 and uint64. Disable these in the mpr
 */
#define     HAS_INT32 1
#define     HAS_UINT32 1
#define     HAS_INT64 1
#define     HAS_UINT64 1

#include    "mpr.h"

/************************************* Defines ********************************/

#define MPR_DEFAULT_SERVER_CERT_FILE    "server.crt"
#define MPR_DEFAULT_SERVER_KEY_FILE     "server.key.pem"
#define MPR_DEFAULT_CLIENT_CERT_FILE    "client.crt"
#define MPR_DEFAULT_CLIENT_CERT_PATH    "certs"

/*
    Per SSL configuration structure
 */
typedef struct MprMatrixSsl {
    sslKeys_t       *keys;
    sslSessionId_t  *session;
} MprMatrixSsl;

/*
    Per socket extended state
 */
typedef struct MprMatrixSocket {
    MprSocket       *sock;
    ssl_t           *handle;            /* MatrixSSL ssl_t structure */
    char            *outbuf;            /* */
    ssize           outlen;             /* */
    ssize           written;            /* */
    int             more;               /* MatrixSSL stack has buffered data */
} MprMatrixSocket;

/***************************** Forward Declarations ***************************/

static MprSocket *acceptMss(MprSocket *sp);
static void     closeMss(MprSocket *sp, bool gracefully);
static int      configureMss(MprSsl *ssl);
static int      connectMss(MprSocket *sp, cchar *host, int port, int flags);
static MprMatrixSsl *createMatrixSsl(MprSsl *ssl);
static MprSocketProvider *createMatrixSslProvider();
static MprSocket *createMss(MprSsl *ssl);
static void     disconnectMss(MprSocket *sp);
static int      doHandshake(MprSocket *sp, short cipherSuite);
static ssize    flushMss(MprSocket *sp);
static MprSsl   *getDefaultMatrixSsl();
static ssize    innerRead(MprSocket *sp, char *userBuf, ssize len);
static int      listenMss(MprSocket *sp, cchar *host, int port, int flags);
static void     manageMatrixSocket(MprMatrixSocket *msp, int flags);
static void     manageMatrixProvider(MprSocketProvider *provider, int flags);
static void     manageMatrixSsl(MprMatrixSsl *mssl, int flags);
static ssize    process(MprSocket *sp, char *buf, ssize size, ssize nbytes, int *readMore);
static ssize    readMss(MprSocket *sp, void *buf, ssize len);
static ssize    writeMss(MprSocket *sp, cvoid *buf, ssize len);

/************************************ Code ************************************/

int mprCreateMatrixSslModule(bool lazy)
{
    MprSocketProvider   *provider;

    /*
        Install this module as the SSL provider (can only have one)
     */
    if ((provider = createMatrixSslProvider()) == 0) {
        return 0;
    }
    mprSetSecureProvider(provider);
    if (matrixSslOpen() < 0) {
        return 0;
    }
    if (!lazy) {
        getDefaultMatrixSsl();
    }
    return 0;
}


/*
    Create the default MatrixSSL configuration state structure
 */
static MprSsl *getDefaultMatrixSsl()
{
    MprSocketService    *ss;
    MprSsl              *ssl;

    ss = MPR->socketService;
    if (ss->secureProvider->defaultSsl) {
        return ss->secureProvider->defaultSsl;
    }
    if ((ssl = mprCreateSsl()) == 0) {
        return 0;
    }
    ss->secureProvider->defaultSsl = ssl;
    if (!createMatrixSsl(ssl)) {
        return 0;
    }
    return ssl;
}


static MprMatrixSsl *createMatrixSsl(MprSsl *ssl)
{
    MprMatrixSsl    *mssl;

    if ((mssl = mprAllocObj(MprMatrixSsl, manageMatrixSsl)) == 0) {
        return 0;
    }
    ssl->extendedSsl = mssl;
    if (matrixSslNewKeys(&mssl->keys) < 0) {
        return 0;
    }
    return mssl;
}


static MprSocketProvider *createMatrixSslProvider()
{
    MprSocketProvider   *provider;

    if ((provider = mprAllocObj(MprSocketProvider, manageMatrixProvider)) == NULL) {
        return 0;
    }
    provider->name = sclone("MatrixSsl");
    provider->acceptSocket = acceptMss;
    provider->closeSocket = closeMss;
    provider->configureSsl = configureMss;
    provider->connectSocket = connectMss;
    provider->createSocket = createMss;
    provider->disconnectSocket = disconnectMss;
    provider->flushSocket = flushMss;
    provider->listenSocket = listenMss;
    provider->readSocket = readMss;
    provider->writeSocket = writeMss;
    return provider;
}


static void manageMatrixProvider(MprSocketProvider *provider, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(provider->defaultSsl);
        mprMark(provider->name);
        mprMark(provider->data);
    }
}


/*
    Initialize a server-side SSL configuration. An application can have multiple different SSL configurations
    for different routes.
 */
static int configureMss(MprSsl *ssl)
{
    MprMatrixSsl    *mssl;
    MprSsl          *defaultSsl;
    char            *password;

    mprAssert(ssl);

#if UNUSED
    if ((defaultSsl = getDefaultMatrixSsl()) == 0) {
        return MPR_ERR_MEMORY;
    }
    if (ssl != defaultSsl) {
        /* 
            If not using the default SSL configuration, allocate a new MatrixSSL configuration.
         */
        if ((mssl = createMatrixSsl(ssl)) == 0) {
            return 0;
        }
#if UNUSED
        src = defaultSsl->extendedSsl;
        mssl->keys = src->keys;
        mssl->session = src->session;
#endif
    } else {
        mssl = ssl->extendedSsl;
    }
#endif

    if ((mssl = createMatrixSsl(ssl)) == 0) {
        return 0;
    }
    /*
        Read the certificate and the key file for this server. FUTURE - If using encrypted private keys, 
        we could prompt through a dialog box or on the console, for the user to enter the password
        rather than using NULL as the password here.
     */
    password = NULL;
    if (matrixSslLoadRsaKeys(mssl->keys, ssl->certFile, ssl->keyFile, password, NULL) < 0) {
        mprError("MatrixSSL: Could not read or decode certificate or key file."); 
        return MPR_ERR_CANT_INITIALIZE;
    }

    /*
        Select the required protocols. MatrixSSL supports only SSLv3.
     */
    if (ssl->protocols & MPR_PROTO_SSLV2) {
        mprError("MatrixSSL: SSLv2 unsupported"); 
        return MPR_ERR_CANT_INITIALIZE;
    }
#if UNUSED
    //  MOB - what is this
    if ((mssl->session = mprAllocObj(sslSessionId_t, NULL)) == 0) {
        return 0;
    }
    mprMark(mssl->session);
#endif
    return 0;
}


static void manageMatrixSsl(MprMatrixSsl *mssl, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        ;
    } else if (flags & MPR_MANAGE_FREE) {
        if (mssl->keys) {
            matrixSslDeleteKeys(mssl->keys);
            mssl->keys = 0;
        }
    }
}


/*
    Create a new Matrix socket
 */
static MprSocket *createMss(MprSsl *ssl)
{
    MprSocketService    *ss;
    MprSocket           *sp;
    MprMatrixSocket     *msp;
    
    if (ssl == MPR_SECURE_CLIENT) {
        /* Use the default SSL provider and configuration */
        ssl = 0;
    }
    /*
        First get a standard socket
     */
    ss = MPR->socketService;
    if ((sp = ss->standardProvider->createSocket(ssl)) == 0) {
        return 0;
    }
    lock(sp);
    sp->provider = ss->secureProvider;

    if ((msp = (MprMatrixSocket*) mprAllocObj(MprMatrixSocket, manageMatrixSocket)) == 0) {
        return 0;
    }
    msp->sock = sp;
    sp->sslSocket = msp;
    sp->ssl = ssl;
    unlock(sp);
    return sp;
}


static void manageMatrixSocket(MprMatrixSocket *msp, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(msp->sock);

    } else if (flags & MPR_MANAGE_FREE) {
        if (msp->handle) {
            matrixSslDeleteSession(msp->handle);
        }
    }
}


/*
    Close a slock
 */
static void closeMss(MprSocket *sp, bool gracefully)
{
    MprMatrixSocket    *msp;
    uchar           *obuf;
    int             nbytes;

    mprAssert(sp);

    lock(sp);
    msp = sp->sslSocket;
    mprAssert(msp);

    if (!(sp->flags & MPR_SOCKET_EOF) && msp->handle) {
        /*
            Flush data. Append a closure alert to any buffered output data, and try to send it.
            Don't bother retrying or blocking, we're just closing anyway.
         */
        matrixSslEncodeClosureAlert(msp->handle);
        if ((nbytes = matrixSslGetOutdata(msp->handle, &obuf)) > 0) {
            /* Ignore return */
            sp->service->standardProvider->writeSocket(sp, obuf, nbytes);
        }
    }
    sp->service->standardProvider->closeSocket(sp, gracefully);
    unlock(sp);
}


static int listenMss(MprSocket *sp, cchar *host, int port, int flags)
{
    return sp->service->standardProvider->listenSocket(sp, host, port, flags);
}


/*
    Called to accept an incoming connection request
 */
static MprSocket *acceptMss(MprSocket *listen)
{
    MprSocket           *sp;
    MprMatrixSocket     *msp;
    MprMatrixSsl        *mssl;

    /*
        Do the standard accept stuff
     */
    if ((sp = listen->service->standardProvider->acceptSocket(listen)) == 0) {
        return 0;
    }
    lock(sp);
    msp = sp->sslSocket;
    mprAssert(msp);

    /* 
        Associate a new ssl session with this socket.  The session represents the state of the ssl protocol 
        over this socket. Session caching is handled automatically by this api.
     */
    mssl = sp->ssl->extendedSsl;
    if (matrixSslNewServerSession(&msp->handle, mssl->keys, NULL) < 0) {
        unlock(sp);
        return 0;
    }
    unlock(sp);
    return sp;
}


/*
    Validate the certificate
 */
static int certValidator(ssl_t *ssl, psX509Cert_t *cert, int32 alert)
{
    //  See client.c - example
    return alert;

#if UNUSED
    psX509Cert_t   *next;
    
    /*
          Make sure we are checking the last cert in the chain
     */
    next = cert;
    while (next->next != NULL) {
        next = next->next;
    }
    /*
        Flag a non-authenticated server correctly. Call matrixSslGetAnonStatus later to 
        see the status of this connection.
     */
    if (next->verified != 1) {
        return SSL_ALLOW_ANON_CONNECTION;
    }
    return next->verified;
#endif
}


/*
    Connect as a client
 */
static int connectMss(MprSocket *sp, cchar *host, int port, int flags)
{
    MprSocketService    *ss;
    MprMatrixSocket     *msp;
    MprMatrixSsl        *mssl;
    MprSsl              *ssl;
    uint32              cipherSuite;
    
    lock(sp);
    ss = sp->service;
    if (sp->service->standardProvider->connectSocket(sp, host, port, flags) < 0) {
        unlock(sp);
        return MPR_ERR_CANT_CONNECT;
    }
    msp = sp->sslSocket;
    mprAssert(msp);

    if (ss->secureProvider->defaultSsl == 0) {
        if ((ssl = getDefaultMatrixSsl()) == 0) {
            unlock(sp);
            return MPR_ERR_CANT_INITIALIZE;
        }
    } else {
        ssl = ss->secureProvider->defaultSsl;
    }
    sp->ssl = ssl;
    mssl = ssl->extendedSsl;

    cipherSuite = 0;
    if (matrixSslNewClientSession(&msp->handle, mssl->keys, NULL, cipherSuite, certValidator, NULL, NULL) < 0) {
        unlock(sp);
        return -1;
    }
#if UNUSED
    /*
        Configure the certificate validator and do the SSL handshake
     */
    matrixSslSetCertValidator(msp->handle, certValidator, NULL);
#endif
    if (doHandshake(sp, 0) < 0) {
        unlock(sp);
        return -1;
    }
    unlock(sp);
    return 0;
}


static void disconnectMss(MprSocket *sp)
{
    sp->service->standardProvider->disconnectSocket(sp);
}


/*
    Low level blocking write
 */
static ssize blockingWrite(MprSocket *sp, cchar *buf, ssize len)
{
    MprSocketProvider   *standard;
    ssize               written, bytes;
    int                 mode;

    standard = sp->service->standardProvider;
    mode = mprSetSocketBlockingMode(sp, 1);
    for (written = 0; len > 0; ) {
        if ((bytes = standard->writeSocket(sp, buf, len)) < 0) {
            mprSetSocketBlockingMode(sp, mode);
            return bytes;
        } else if (bytes == 0) {
            mprNap(10);
        }
        buf += bytes;
        len -= bytes;
        written += bytes;
    }
    mprSetSocketBlockingMode(sp, mode);
    return written;
}


/*
    Construct the initial HELLO message to send to the server and initiate
    the SSL handshake. Can be used in the re-handshake scenario as well.
 */
static int doHandshake(MprSocket *sp, short cipherSuite)
{
    MprMatrixSocket     *msp;
    MprSocketProvider   *standard;
    ssize               rc, written, toWrite;
    char                *obuf, buf[MPR_SSL_BUFSIZE];

    msp = sp->sslSocket;
    standard = sp->service->standardProvider;

    toWrite = matrixSslGetOutdata(msp->handle, (uchar**) &obuf);
    if ((written = blockingWrite(sp, obuf, toWrite)) < 0) {
        mprError("MatrixSSL: Error in socketWrite");
        return MPR_ERR_CANT_INITIALIZE;
    }
    matrixSslSentData(msp->handle, (int) written);

    //  MOB - is this blocking? or does it spin?
    while (1) {
        /*
            Reading handshake records should always return 0 bytes, we aren't expecting any data yet.
         */
        if ((rc = innerRead(sp, buf, sizeof(buf))) == 0) {
            if (mprIsSocketEof(sp)) {
                return MPR_ERR_CANT_INITIALIZE;
            }
            if (matrixSslHandshakeIsComplete(msp->handle)) {
                break;
            }
        } else {
            mprError("MatrixSSL: sslRead error in sslDoHandhake, rc %d", rc);
            return MPR_ERR_CANT_INITIALIZE;
        }
    }
    return 0;
}


/*
    Process 
    MOB - rename
 */
static ssize process(MprSocket *sp, char *buf, ssize size, ssize nbytes, int *readMore)
{
    MprMatrixSocket     *msp;
    MprSocketProvider   *standard;
    uchar               *data, *obuf;
    ssize               toWrite, written, copied, sofar;
    uint32              dlen;
    int                 rc;

    msp = (MprMatrixSocket*) sp->sslSocket;
    standard = sp->service->standardProvider;
    *readMore = 0;
    sofar = 0;

    /*
        Process the received data. If there is application data, it is returned in data/dlen
     */
    rc = matrixSslReceivedData(msp->handle, (int) nbytes, &data, &dlen);

    while (1) {
        switch (rc) {
        case PS_SUCCESS:
            return sofar;

        case MATRIXSSL_REQUEST_SEND:
            toWrite = matrixSslGetOutdata(msp->handle, &obuf);
            if ((written = blockingWrite(sp, (cchar*) obuf, (int) toWrite)) < 0) {
                mprError("MatrixSSL: Error in process");
                return MPR_ERR_CANT_INITIALIZE;
            }
            matrixSslSentData(msp->handle, (int) written);
            *readMore = 1;
            return 0;

        case MATRIXSSL_REQUEST_RECV:
            /* Partial read. More read data required */
            *readMore = 1;
            msp->more = 1;
            return 0;

        case MATRIXSSL_HANDSHAKE_COMPLETE:
            *readMore = 1;
            return 0;

        case MATRIXSSL_RECEIVED_ALERT:
            mprAssert(dlen == 2);
            if (data[0] == SSL_ALERT_LEVEL_FATAL) {
                return MPR_ERR;
            } else if (data[1] == SSL_ALERT_CLOSE_NOTIFY) {
                //  ignore - graceful close
                return 0;
            } else {
                //  ignore
            }
            rc = matrixSslProcessedData(msp->handle, &data, &dlen);
            break;

        case MATRIXSSL_APP_DATA:
            copied = min(dlen, size);
            memcpy(buf, data, copied);
            buf += copied;
            size -= copied;
            data += copied;
            dlen = dlen - (int) copied;
            sofar += copied;
            msp->more = (dlen > size) ? 1 : 0;
            if (!msp->more) {
                /* The MatrixSSL buffer has been consumed, see if we can get more data */
                rc = matrixSslProcessedData(msp->handle, &data, &dlen);
                break;
            }
            return sofar;

        default:
            return MPR_ERR;
        }
    }
}


/*
    Return number of bytes read. Return -1 on errors and EOF
 */
static ssize innerRead(MprSocket *sp, char *buf, ssize size)
{
    MprMatrixSocket     *msp;
    MprSocketProvider   *standard;
    uchar               *mbuf;
    ssize               nbytes;
    int                 msize, readMore;

    msp = (MprMatrixSocket*) sp->sslSocket;
    standard = sp->service->standardProvider;
    readMore = 0;
    do {
        /*
            Get the MatrixSSL read buffer to read data into
         */
        if ((msize = matrixSslGetReadbuf(msp->handle, &mbuf)) < 0) {
            return MPR_ERR_BAD_STATE;
        }
        if ((nbytes = standard->readSocket(sp, mbuf, msize)) > 0) {
            if ((nbytes = process(sp, buf, size, nbytes, &readMore)) > 0) {
                return nbytes;
            }
        }
    } while (readMore);
    return 0;
}


/*
    Return number of bytes read. Return -1 on errors and EOF.
 */
static ssize readMss(MprSocket *sp, void *buf, ssize len)
{
    MprMatrixSocket *msp;
    ssize           bytes;

    if (len <= 0) {
        return -1;
    }
    lock(sp);
    bytes = innerRead(sp, buf, len);
    /*
        If there is more data buffered locally here, then ensure the select handler will recall us again even 
        if there is no more IO events
     */
    msp = (MprMatrixSocket*) sp->sslSocket;
    if (msp->more) {
        sp->flags |= MPR_SOCKET_PENDING;
        mprRecallWaitHandlerByFd(sp->fd);
    }
    unlock(sp);
    return bytes;
}


/*
    Non-blocking write data. Return the number of bytes written or -1 on errors.
    Returns zero if part of the data was written.

    Encode caller's data buffer into an SSL record and write to socket. The encoded data will always be 
    bigger than the incoming data because of the record header (5 bytes) and MAC (16 bytes MD5 / 20 bytes SHA1)
    This would be fine if we were using blocking sockets, but non-blocking presents an interesting problem.  Example:

        A 100 byte input record is encoded to a 125 byte SSL record
        We can send 124 bytes without blocking, leaving one buffered byte
        We can't return 124 to the caller because it's more than they requested
        We can't return 100 to the caller because they would assume all data
        has been written, and we wouldn't get re-called to send the last byte

    We handle the above case by returning 0 to the caller if the entire encoded record could not be sent. Returning 
    0 will prompt us to select this socket for write events, and we'll be called again when the socket is writable.  
    We'll use this mechanism to flush the remaining encoded data, ignoring the bytes sent in, as they have already 
    been encoded.  When it is completely flushed, we return the originally requested length, and resume normal 
    processing.
 */
static ssize writeMss(MprSocket *sp, cvoid *buf, ssize len)
{
    MprMatrixSocket     *msp;
    uchar               *obuf;
    ssize               encoded, nbytes, written;

    msp = (MprMatrixSocket*) sp->sslSocket;

    while (len > 0 || msp->outlen > 0) {
        if ((encoded = matrixSslGetOutdata(msp->handle, &obuf)) <= 0) {
            if (msp->outlen <= 0) {
                msp->outbuf = (char*) buf;
                msp->outlen = len;
                msp->written = 0;
                len = 0;
                nbytes = min(msp->outlen, SSL_MAX_PLAINTEXT_LEN);
            }
            if ((encoded = matrixSslEncodeToOutdata(msp->handle, (uchar*) buf, (int) nbytes)) < 0) {
                return encoded;
            }
            msp->outbuf += nbytes;
            msp->outlen -= nbytes;
            msp->written += nbytes;
        }
        if ((written = sp->service->standardProvider->writeSocket(sp, obuf, encoded)) < 0) {
            return written;
        } else if (written == 0) {
            break;
        }
        matrixSslSentData(msp->handle, (int) written);
    }
    /*
        Only signify all the data has been written if MatrixSSL has absorbed all the data
     */
    return msp->outlen == 0 ? msp->written : 0;
}


/*
    Flush write data. This is blocking.
 */
static ssize flushMss(MprSocket *sp)
{
    MprMatrixSocket     *msp;
    ssize               written, bytes;
    int                 mode;

    msp = (MprMatrixSocket*) sp->sslSocket;
    written = 0;
    mode = mprSetSocketBlockingMode(sp, 1);
    while (msp->outlen > 0) {
        if ((bytes = writeMss(sp, NULL, 0)) < 0) {
            mprSetSocketBlockingMode(sp, mode);
            return bytes;
        }
        written += bytes;
    }
    mprSetSocketBlockingMode(sp, mode);
    return written;
}

#else

int mprCreateMatrixSslModule() { return -1; }
#endif /* BLD_FEATURE_MATRIXSSL */

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
