/*
    mprOpenssl.c - Support for secure sockets via OpenSSL

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

#if BIT_PACK_OPENSSL

/* Clashes with WinCrypt.h */
#undef OCSP_RESPONSE
#include    <openssl/ssl.h>
#include    <openssl/evp.h>
#include    <openssl/rand.h>
#include    <openssl/err.h>
#include    <openssl/dh.h>

/************************************* Defines ********************************/

#define MPR_DEFAULT_SERVER_CERT_FILE    "server.crt"
#define MPR_DEFAULT_SERVER_KEY_FILE     "server.key.pem"
#define MPR_DEFAULT_CLIENT_CERT_FILE    "client.crt"
#define MPR_DEFAULT_CLIENT_CERT_PATH    "certs"

typedef struct MprOpenSsl {
    SSL_CTX         *context;
    RSA             *rsaKey512;
    RSA             *rsaKey1024;
    DH              *dhKey512;
    DH              *dhKey1024;
#if UNUSED
    MprMutex        **locks;
#endif
} MprOpenSsl;

typedef struct MprOpenSocket {
    MprSocket       *sock;
    SSL             *handle;
    BIO             *bio;
} MprOpenSocket;

typedef struct RandBuf {
    MprTime     now;
    int         pid;
} RandBuf;

static int      numLocks;
static MprMutex **olocks;
static MprSocketProvider *openSslProvider;
static MprOpenSsl *defaultOpenSsl;

struct CRYPTO_dynlock_value {
    MprMutex    *mutex;
};
typedef struct CRYPTO_dynlock_value DynLock;

/***************************** Forward Declarations ***************************/

static void     closeOss(MprSocket *sp, bool gracefully);
static int      configureCertificateFiles(MprSsl *ssl, SSL_CTX *ctx, char *key, char *cert);
static MprOpenSsl *createOpenSslConfig(MprSsl *ssl, int server);
static MprSocketProvider *createOpenSslProvider();
static DH       *dhCallback(SSL *ssl, int isExport, int keyLength);
static void     disconnectOss(MprSocket *sp);
static ssize    flushOss(MprSocket *sp);
static int      listenOss(MprSocket *sp, cchar *host, int port, int flags);
static void     manageOpenSsl(MprOpenSsl *ossl, int flags);
static void     manageOpenSocket(MprOpenSocket *ssp, int flags);
static ssize    readOss(MprSocket *sp, void *buf, ssize len);
static RSA      *rsaCallback(SSL *ssl, int isExport, int keyLength);
static int      upgradeOss(MprSocket *sp, MprSsl *ssl, int server);
static int      verifyX509Certificate(int ok, X509_STORE_CTX *ctx);
static ssize    writeOss(MprSocket *sp, cvoid *buf, ssize len);

static DynLock  *sslCreateDynLock(const char *file, int line);
static void     sslDynLock(int mode, DynLock *dl, const char *file, int line);
static void     sslDestroyDynLock(DynLock *dl, const char *file, int line);
static void     sslStaticLock(int mode, int n, const char *file, int line);
static ulong    sslThreadId(void);

static DH       *get_dh512();
static DH       *get_dh1024();

/************************************* Code ***********************************/
/*
    Create the Openssl module. This is called only once
 */
int mprCreateOpenSslModule()
{
    RandBuf     randBuf;
    int         i;

    /*
        Get some random data
     */
    randBuf.now = mprGetTime();
    randBuf.pid = getpid();
    RAND_seed((void*) &randBuf, sizeof(randBuf));
#if BIT_UNIX_LIKE
    mprLog(6, "OpenSsl: Before calling RAND_load_file");
    RAND_load_file("/dev/urandom", 256);
    mprLog(6, "OpenSsl: After calling RAND_load_file");
#endif

    if ((openSslProvider = createOpenSslProvider()) == 0) {
        return MPR_ERR_MEMORY;
    }
    mprAddSocketProvider("openssl", openSslProvider);

    /*
        Pre-create expensive keys
     */
    if ((defaultOpenSsl = mprAllocObj(MprOpenSsl, manageOpenSsl)) == 0) {
        return MPR_ERR_MEMORY;
    }
    defaultOpenSsl->rsaKey512 = RSA_generate_key(512, RSA_F4, 0, 0);
    defaultOpenSsl->rsaKey1024 = RSA_generate_key(1024, RSA_F4, 0, 0);
    defaultOpenSsl->dhKey512 = get_dh512();
    defaultOpenSsl->dhKey1024 = get_dh1024();

    /*
        Configure the SSL library. Use the crypto ID as a one-time test. This allows
        users to configure the library and have their configuration used instead.
     */
    if (CRYPTO_get_id_callback() == 0) {
        numLocks = CRYPTO_num_locks();
        if ((olocks = mprAlloc(numLocks * sizeof(MprMutex*))) == 0) {
            return MPR_ERR_MEMORY;
        }
        for (i = 0; i < numLocks; i++) {
            olocks[i] = mprCreateLock();
        }
        CRYPTO_set_id_callback(sslThreadId);
        CRYPTO_set_locking_callback(sslStaticLock);

        CRYPTO_set_dynlock_create_callback(sslCreateDynLock);
        CRYPTO_set_dynlock_destroy_callback(sslDestroyDynLock);
        CRYPTO_set_dynlock_lock_callback(sslDynLock);
#if !BIT_WIN_LIKE
        /* OPT - Should be a configure option to specify desired ciphers */
        OpenSSL_add_all_algorithms();
#endif
        /*
            WARNING: SSL_library_init() is not reentrant. Caller must ensure safety.
         */
        SSL_library_init();
        SSL_load_error_strings();
    }
    return 0;
}


static void manageOpenSsl(MprOpenSsl *ossl, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        ;
    } else if (flags & MPR_MANAGE_FREE) {
        if (ossl->context != 0) {
            SSL_CTX_free(ossl->context);
            ossl->context = 0;
        }
        if (ossl == defaultOpenSsl) {
            if (ossl->rsaKey512) {
                RSA_free(ossl->rsaKey512);
                ossl->rsaKey512 = 0;
            }
            if (ossl->rsaKey1024) {
                RSA_free(ossl->rsaKey1024);
                ossl->rsaKey1024 = 0;
            }
            if (ossl->dhKey512) {
                DH_free(ossl->dhKey512);
                ossl->dhKey512 = 0;
            }
            if (ossl->dhKey1024) {
                DH_free(ossl->dhKey1024);
                ossl->dhKey1024 = 0;
            }
        }
    }
}


static void manageOpenSslProvider(MprSocketProvider *provider, int flags)
{
    int     i;

    if (flags & MPR_MANAGE_MARK) {
        /*
            Mark global locks
         */
        if (olocks) {
            mprMark(olocks);
            for (i = 0; i < numLocks; i++) {
                mprMark(olocks[i]);
            }
        }
        mprMark(defaultOpenSsl);

    } else if (flags & MPR_MANAGE_FREE) {
        olocks = 0;
    }
}


/*
    Initialize a provider structure for OpenSSL
 */
static MprSocketProvider *createOpenSslProvider()
{
    MprSocketProvider   *provider;

    if ((provider = mprAllocObj(MprSocketProvider, manageOpenSslProvider)) == NULL) {
        return 0;
    }
    provider->upgradeSocket = upgradeOss;
    provider->closeSocket = closeOss;
    provider->disconnectSocket = disconnectOss;
    provider->flushSocket = flushOss;
    provider->listenSocket = listenOss;
    provider->readSocket = readOss;
    provider->writeSocket = writeOss;
    return provider;
}


/*
    Create an SSL configuration for a route. An application can have multiple different SSL 
    configurations for different routes. There is default SSL configuration that is used
    when a route does not define a configuration and also for clients.
 */
static MprOpenSsl *createOpenSslConfig(MprSsl *ssl, int server)
{
    MprOpenSsl          *ossl;
    SSL_CTX             *context;
    uchar               resume[16];

    mprAssert(ssl);

    if ((ssl->pconfig = mprAllocObj(MprOpenSsl, manageOpenSsl)) == 0) {
        return 0;
    }
    ossl = ssl->pconfig;
    ossl->rsaKey512 = defaultOpenSsl->rsaKey512;
    ossl->rsaKey1024 = defaultOpenSsl->rsaKey1024;
    ossl->dhKey512 = defaultOpenSsl->dhKey512;
    ossl->dhKey1024 = defaultOpenSsl->dhKey1024;

    ossl = ssl->pconfig;
    mprAssert(ossl);

    if ((context = SSL_CTX_new(SSLv23_method())) == 0) {
        mprError("OpenSSL: Unable to create SSL context"); 
        return 0;
    }
    SSL_CTX_set_app_data(context, (void*) ssl);
    SSL_CTX_sess_set_cache_size(context, 512);
    RAND_bytes(resume, sizeof(resume));
    SSL_CTX_set_session_id_context(context, resume, sizeof(resume));

    /*
        Configure the certificates
     */
    if (ssl->keyFile || ssl->certFile) {
        if (configureCertificateFiles(ssl, context, ssl->keyFile, ssl->certFile) != 0) {
            SSL_CTX_free(context);
            return 0;
        }
    }
    SSL_CTX_set_cipher_list(context, ssl->ciphers);

    if (ssl->caFile || ssl->caPath) {
        if ((!SSL_CTX_load_verify_locations(context, ssl->caFile, ssl->caPath)) ||
                (!SSL_CTX_set_default_verify_paths(context))) {
            mprError("OpenSSL: Unable to set certificate locations"); 
            SSL_CTX_free(context);
            return 0;
        }
        if (ssl->caFile) {
            STACK_OF(X509_NAME) *certNames;
            certNames = SSL_load_client_CA_file(ssl->caFile);
            if (certNames) {
                /*
                    Define the list of CA certificates to send to the client
                    before they send their client certificate for validation
                 */
                SSL_CTX_set_client_CA_list(context, certNames);
            }
        }
    }
    if (server) {
        if (ssl->verifyPeer) {
            if (!ssl->caFile == 0 && !ssl->caPath) {
                mprError("OpenSSL: Must define CA certificates if using client verification");
                SSL_CTX_free(context);
                return 0;
            }
            SSL_CTX_set_verify(context, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, verifyX509Certificate);
            SSL_CTX_set_verify_depth(context, ssl->verifyDepth);
        } else {
            /* With this, the server will not request a client certificate */
            SSL_CTX_set_verify(context, SSL_VERIFY_NONE, verifyX509Certificate);
        }
    } else {
        SSL_CTX_set_verify(context, SSL_VERIFY_PEER, verifyX509Certificate);
    }
    /*
        Define callbacks
     */
    SSL_CTX_set_tmp_rsa_callback(context, rsaCallback);
    SSL_CTX_set_tmp_dh_callback(context, dhCallback);

    SSL_CTX_set_options(context, SSL_OP_ALL);
#ifdef SSL_OP_NO_TICKET
    SSL_CTX_set_options(context, SSL_OP_NO_TICKET);
#endif
#ifdef SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION
    SSL_CTX_set_options(context, SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);
#endif
    SSL_CTX_set_mode(context, SSL_MODE_ENABLE_PARTIAL_WRITE | SSL_MODE_AUTO_RETRY);

    /*
        Select the required protocols
        Disable SSLv2 by default -- it is insecure.
     */
    SSL_CTX_set_options(context, SSL_OP_NO_SSLv2);
    if (!(ssl->protocols & MPR_PROTO_SSLV3)) {
        SSL_CTX_set_options(context, SSL_OP_NO_SSLv3);
    }
    if (!(ssl->protocols & MPR_PROTO_TLSV1)) {
        SSL_CTX_set_options(context, SSL_OP_NO_TLSv1);
    }
    /* 
        Ensure we generate a new private key for each connection
     */
    SSL_CTX_set_options(context, SSL_OP_SINGLE_DH_USE);
    ossl->context = context;
    return ossl;
}


/*
    Configure the SSL certificate information using key and cert files
 */
static int configureCertificateFiles(MprSsl *ssl, SSL_CTX *ctx, char *key, char *cert)
{
    mprAssert(ctx);

    if (cert == 0) {
        return 0;
    }
    if (cert && SSL_CTX_use_certificate_chain_file(ctx, cert) <= 0) {
        if (SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_ASN1) <= 0) {
            mprError("OpenSSL: Can't open certificate file: %s", cert); 
            return -1;
        }
    }
    key = (key == 0) ? cert : key;
    if (key) {
        if (SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM) <= 0) {
            /* attempt ASN1 for self-signed format */
            if (SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_ASN1) <= 0) {
                mprError("OpenSSL: Can't define private key file: %s", key); 
                return -1;
            }
        }
        if (!SSL_CTX_check_private_key(ctx)) {
            mprError("OpenSSL: Check of private key file failed: %s", key);
            return -1;
        }
    }
    return 0;
}


/*
    Destructor for an MprOpenSocket object
 */
static void manageOpenSocket(MprOpenSocket *osp, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(osp->sock);

    } else if (flags & MPR_MANAGE_FREE) {
        if (osp->handle) {
            SSL_set_shutdown(osp->handle, SSL_SENT_SHUTDOWN | SSL_RECEIVED_SHUTDOWN);
            SSL_free(osp->handle);
            osp->handle = 0;
        }
    }
}


static void closeOss(MprSocket *sp, bool gracefully)
{
    MprOpenSocket    *osp;

    osp = sp->sslSocket;
    SSL_free(osp->handle);
    osp->handle = 0;
    sp->service->standardProvider->closeSocket(sp, gracefully);
}


/*
    Initialize a new server-side connection
 */
static int listenOss(MprSocket *sp, cchar *host, int port, int flags)
{
    mprAssert(sp);
    mprAssert(port);
    return sp->service->standardProvider->listenSocket(sp, host, port, flags);
}


/*
    Upgrade a standard socket to use SSL/TLS
 */
static int upgradeOss(MprSocket *sp, MprSsl *ssl, int server)
{
    MprOpenSocket   *osp;
    MprOpenSsl      *ossl;
    char            ebuf[MPR_MAX_STRING];
    ulong           error;
    int             rc;

    mprAssert(sp);

    if (ssl == 0) {
        ssl = mprCreateSsl();
    }
    lock(sp);
    if ((osp = (MprOpenSocket*) mprAllocObj(MprOpenSocket, manageOpenSocket)) == 0) {
        unlock(sp);
        return MPR_ERR_MEMORY;
    }
    osp->sock = sp;
    sp->sslSocket = osp;
    sp->ssl = ssl;

    if (!ssl->pconfig && (ssl->pconfig = createOpenSslConfig(ssl, server)) == 0) {
        unlock(sp);
        return MPR_ERR_CANT_INITIALIZE;
    }
    /*
        Create and configure the SSL struct
     */
    ossl = sp->ssl->pconfig;
    if ((osp->handle = (SSL*) SSL_new(ossl->context)) == 0) {
        unlock(sp);
        return MPR_ERR_BAD_STATE;
    }
    SSL_set_app_data(osp->handle, (void*) osp);

    /*
        Create a socket bio
     */
    osp->bio = BIO_new_socket(sp->fd, BIO_NOCLOSE);
    SSL_set_bio(osp->handle, osp->bio, osp->bio);
    if (server) {
        SSL_set_accept_state(osp->handle);
    } else {
        /* Block while connecting */
        mprSetSocketBlockingMode(sp, 1);
        if ((rc = SSL_connect(osp->handle)) < 1) {
            error = ERR_get_error();
            ERR_error_string_n(error, ebuf, sizeof(ebuf) - 1);
            sp->errorMsg = sclone(ebuf);
            mprLog(4, "SSL_read error %s", ebuf);
            unlock(sp);
            return MPR_ERR_CANT_CONNECT;
        }
        mprSetSocketBlockingMode(sp, 0);
    }
    unlock(sp);
    return 0;
}


static void disconnectOss(MprSocket *sp)
{
    sp->service->standardProvider->disconnectSocket(sp);
}


/*
    Return the number of bytes read. Return -1 on errors and EOF
 */
static ssize readOss(MprSocket *sp, void *buf, ssize len)
{
    MprOpenSocket   *osp;
    MprSsl          *ssl;
    X509_NAME       *xSubject;
    X509            *cert;
    char            subject[260], issuer[260], peer[260], ebuf[MPR_MAX_STRING];
    ulong           serror;
    int             rc, error, retries, i;

    lock(sp);
    osp = (MprOpenSocket*) sp->sslSocket;
    mprAssert(osp);

    if (osp->handle == 0) {
        mprAssert(osp->handle);
        unlock(sp);
        return -1;
    }
    /*  
        Limit retries on WANT_READ. If non-blocking and no data, then this can spin forever.
     */
    retries = 5;
    for (i = 0; i < retries; i++) {
        rc = SSL_read(osp->handle, buf, (int) len);
        if (rc < 0) {
            error = SSL_get_error(osp->handle, rc);
            if (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_CONNECT || error == SSL_ERROR_WANT_ACCEPT) {
                continue;
            }
            serror = ERR_get_error();
            ERR_error_string_n(serror, ebuf, sizeof(ebuf) - 1);
            mprLog(5, "SSL_read %s", ebuf);
        }
        break;
    }
    if (rc > 0 && !(sp->flags & MPR_SOCKET_TRACED)) {
        ssl = sp->ssl;
        mprLog(4, "OpenSSL connected using cipher: \"%s\" from set %s", SSL_get_cipher(osp->handle), ssl->ciphers);
        if (ssl->caFile) {
            mprLog(4, "OpenSSL: Using certificates from %s", ssl->caFile);
        } else if (ssl->caPath) {
            mprLog(4, "OpenSSL: Using certificates from directory %s", ssl->caPath);
        }
        cert = SSL_get_peer_certificate(osp->handle);
        if (cert == 0) {
            mprLog(4, "OpenSSL: client supplied no certificate");
        } else {
            xSubject = X509_get_subject_name(cert);
            X509_NAME_oneline(xSubject, subject, sizeof(subject) -1);
            X509_NAME_oneline(X509_get_issuer_name(cert), issuer, sizeof(issuer) -1);
            X509_NAME_get_text_by_NID(xSubject, NID_commonName, peer, sizeof(peer) - 1);
            mprLog(4, "OpenSSL Subject %s", subject);
            mprLog(4, "OpenSSL Issuer: %s", issuer);
            mprLog(4, "OpenSSL Peer: %s", peer);
            X509_free(cert);
        }
        sp->flags |= MPR_SOCKET_TRACED;
    }
    if (rc <= 0) {
        error = SSL_get_error(osp->handle, rc);
        if (error == SSL_ERROR_WANT_READ) {
            rc = 0;
        } else if (error == SSL_ERROR_WANT_WRITE) {
            mprNap(10);
            rc = 0;
        } else if (error == SSL_ERROR_ZERO_RETURN) {
            sp->flags |= MPR_SOCKET_EOF;
            rc = -1;
        } else if (error == SSL_ERROR_SYSCALL) {
            sp->flags |= MPR_SOCKET_EOF;
            rc = -1;
        } else if (error != SSL_ERROR_ZERO_RETURN) {
            /* SSL_ERROR_SSL */
            serror = ERR_get_error();
            ERR_error_string_n(serror, ebuf, sizeof(ebuf) - 1);
            mprLog(4, "OpenSSL: connection with protocol error: %s", ebuf);
            rc = -1;
            sp->flags |= MPR_SOCKET_EOF;
        }
    } else if (SSL_pending(osp->handle) > 0) {
        sp->flags |= MPR_SOCKET_PENDING;
        mprRecallWaitHandlerByFd(sp->fd);
    }
    unlock(sp);
    return rc;
}


/*
    Write data. Return the number of bytes written or -1 on errors.
 */
static ssize writeOss(MprSocket *sp, cvoid *buf, ssize len)
{
    MprOpenSocket    *osp;
    ssize           totalWritten;
    int             rc;

    lock(sp);
    osp = (MprOpenSocket*) sp->sslSocket;

    if (osp->bio == 0 || osp->handle == 0 || len <= 0) {
        mprAssert(0);
        unlock(sp);
        return -1;
    }
    totalWritten = 0;
    ERR_clear_error();

    do {
        rc = SSL_write(osp->handle, buf, (int) len);
        mprLog(7, "OpenSSL: written %d, requested len %d", rc, len);
        if (rc <= 0) {
            rc = SSL_get_error(osp->handle, rc);
            if (rc == SSL_ERROR_WANT_WRITE) {
                mprNap(10);
                continue;
            } else if (rc == SSL_ERROR_WANT_READ) {
                //  AUTO-RETRY should stop this
                mprAssert(0);
                unlock(sp);
                return -1;
            } else {
                unlock(sp);
                return -1;
            }
        }
        totalWritten += rc;
        buf = (void*) ((char*) buf + rc);
        len -= rc;
        mprLog(7, "OpenSSL: write: len %d, written %d, total %d, error %d", len, rc, totalWritten, 
            SSL_get_error(osp->handle, rc));

    } while (len > 0);

    unlock(sp);
    return totalWritten;
}


/*
    Called to verify X509 client certificates
 */
static int verifyX509Certificate(int ok, X509_STORE_CTX *xContext)
{
    X509            *cert;
    SSL             *handle;
    MprOpenSocket   *osp;
    MprSsl          *ssl;
    char            subject[260], issuer[260], peer[260];
    int             error, depth;
    
    subject[0] = issuer[0] = '\0';

    handle = (SSL*) X509_STORE_CTX_get_app_data(xContext);
    osp = (MprOpenSocket*) SSL_get_app_data(handle);
    ssl = osp->sock->ssl;

    cert = X509_STORE_CTX_get_current_cert(xContext);
    depth = X509_STORE_CTX_get_error_depth(xContext);
    error = X509_STORE_CTX_get_error(xContext);

    ok = 1;
    if (X509_NAME_oneline(X509_get_subject_name(cert), subject, sizeof(subject) - 1) < 0) {
        ok = 0;
    }
    if (X509_NAME_oneline(X509_get_issuer_name(xContext->current_cert), issuer, sizeof(issuer) - 1) < 0) {
        ok = 0;
    }
    if (X509_NAME_get_text_by_NID(X509_get_subject_name(xContext->current_cert), NID_commonName, peer, 
            sizeof(peer) - 1) < 0) {
        ok = 0;
    }
    if (ok && ssl->verifyDepth < depth) {
        if (error == 0) {
            error = X509_V_ERR_CERT_CHAIN_TOO_LONG;
        }
    }
    switch (error) {
    case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
        /* Normal self signed certificate */
    case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
    case X509_V_ERR_CERT_UNTRUSTED:
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
        if (ssl->verifyIssuer) {
            /* Issuer can't be verified */
            ok = 0;
        }
        break;

    case X509_V_ERR_CERT_CHAIN_TOO_LONG:
    case X509_V_ERR_CERT_HAS_EXPIRED:
    case X509_V_ERR_CERT_NOT_YET_VALID:
    case X509_V_ERR_CERT_REJECTED:
    case X509_V_ERR_CERT_SIGNATURE_FAILURE:
    case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
    case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
    case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
    case X509_V_ERR_INVALID_CA:
    default:
        ok = 0;
        break;
    }
    if (ok) {
        mprLog(3, "OpenSSL: Certificate verified: subject %s", subject);
        mprLog(4, "OpenSSL: Issuer: %s", issuer);
        mprLog(4, "OpenSSL: Peer: %s", peer);
    } else {
        mprLog(1, "OpenSSL: Certification failed: subject %s (more trace at level 4)", subject);
        mprLog(4, "OpenSSL: Issuer: %s", issuer);
        mprLog(4, "OpenSSL: Peer: %s", peer);
        mprLog(4, "OpenSSL: Error: %d: %s", error, X509_verify_cert_error_string(error));
    }
    return ok;
}


static ssize flushOss(MprSocket *sp)
{
#if NOT_REQUIRED && KEEP
    MprOpenSocket    *osp;
    osp = (MprOpenSocket*) sp->sslSocket;
    return BIO_flush(osp->bio);
#endif
    return 0;
}

 
static ulong sslThreadId()
{
    return (long) mprGetCurrentOsThread();
}


static void sslStaticLock(int mode, int n, const char *file, int line)
{
    mprAssert(0 <= n && n < numLocks);

    if (olocks) {
        if (mode & CRYPTO_LOCK) {
            mprLock(olocks[n]);
        } else {
            mprUnlock(olocks[n]);
        }
    }
}


static DynLock *sslCreateDynLock(const char *file, int line)
{
    DynLock     *dl;

    dl = mprAllocZeroed(sizeof(DynLock));
    dl->mutex = mprCreateLock(dl);
    mprHold(dl->mutex);
    return dl;
}


static void sslDestroyDynLock(DynLock *dl, const char *file, int line)
{
    if (dl->mutex) {
        mprRelease(dl->mutex);
        dl->mutex = 0;
    }
}


static void sslDynLock(int mode, DynLock *dl, const char *file, int line)
{
    if (mode & CRYPTO_LOCK) {
        mprLock(dl->mutex);
    } else {
        mprUnlock(dl->mutex);
    }
}


/*
    Used for ephemeral RSA keys
 */
static RSA *rsaCallback(SSL *handle, int isExport, int keyLength)
{
    MprSocket       *sp;
    MprOpenSocket   *osp;
    MprOpenSsl      *ossl;
    RSA             *key;

    osp = (MprOpenSocket*) SSL_get_app_data(handle);
    sp = osp->sock;
    mprAssert(sp);
    ossl = sp->ssl->pconfig;

    key = 0;
    switch (keyLength) {
    case 512:
        key = ossl->rsaKey512;
        break;

    case 1024:
    default:
        key = ossl->rsaKey1024;
    }
    return key;
}


/*
    Used for ephemeral DH keys
 */
static DH *dhCallback(SSL *handle, int isExport, int keyLength)
{
    MprSocket       *sp;
    MprOpenSocket   *osp;
    MprOpenSsl      *ossl;
    DH              *key;

    osp = (MprOpenSocket*) SSL_get_app_data(handle);
    sp = osp->sock;
    ossl = sp->ssl->pconfig;

    key = 0;
    switch (keyLength) {
    case 512:
        key = ossl->dhKey512;
        break;

    case 1024:
    default:
        key = ossl->dhKey1024;
    }
    return key;
}


/*
    openSslDh.c - OpenSSL DH get routines. Generated by openssl.
    Use bit gendh to generate new content.
 */
static DH *get_dh512()
{
    static unsigned char dh512_p[] = {
        0x8E,0xFD,0xBE,0xD3,0x92,0x1D,0x0C,0x0A,0x58,0xBF,0xFF,0xE4,
        0x51,0x54,0x36,0x39,0x13,0xEA,0xD8,0xD2,0x70,0xBB,0xE3,0x8C,
        0x86,0xA6,0x31,0xA1,0x04,0x2A,0x09,0xE4,0xD0,0x33,0x88,0x5F,
        0xEF,0xB1,0x70,0xEA,0x42,0xB6,0x0E,0x58,0x60,0xD5,0xC1,0x0C,
        0xD1,0x12,0x16,0x99,0xBC,0x7E,0x55,0x7C,0xE4,0xC1,0x5D,0x15,
        0xF6,0x45,0xBC,0x73,
    };
    static unsigned char dh512_g[] = {
        0x02,
    };

    DH *dh;

    if ((dh = DH_new()) == NULL) {
        return(NULL);
    }
    dh->p=BN_bin2bn(dh512_p,sizeof(dh512_p),NULL);
    dh->g=BN_bin2bn(dh512_g,sizeof(dh512_g),NULL);

    if ((dh->p == NULL) || (dh->g == NULL)) { 
        DH_free(dh); 
        return(NULL); 
    }
    return dh;
}


static DH *get_dh1024()
{
    static unsigned char dh1024_p[] = {
        0xCD,0x02,0x2C,0x11,0x43,0xCD,0xAD,0xF5,0x54,0x5F,0xED,0xB1,
        0x28,0x56,0xDF,0x99,0xFA,0x80,0x2C,0x70,0xB5,0xC8,0xA8,0x12,
        0xC3,0xCD,0x38,0x0D,0x3B,0xE1,0xE3,0xA3,0xE4,0xE9,0xCB,0x58,
        0x78,0x7E,0xA6,0x80,0x7E,0xFC,0xC9,0x93,0x3A,0x86,0x1C,0x8E,
        0x0B,0xA2,0x1C,0xD0,0x09,0x99,0x29,0x9B,0xC1,0x53,0xB8,0xF3,
        0x98,0xA7,0xD8,0x46,0xBE,0x5B,0xB9,0x64,0x31,0xCF,0x02,0x63,
        0x0F,0x5D,0xF2,0xBE,0xEF,0xF6,0x55,0x8B,0xFB,0xF0,0xB8,0xF7,
        0xA5,0x2E,0xD2,0x6F,0x58,0x1E,0x46,0x3F,0x74,0x3C,0x02,0x41,
        0x2F,0x65,0x53,0x7F,0x1C,0x7B,0x8A,0x72,0x22,0x1D,0x2B,0xE9,
        0xA3,0x0F,0x50,0xC3,0x13,0x12,0x6C,0xD2,0x17,0xA9,0xA5,0x82,
        0xFC,0x91,0xE3,0x3E,0x28,0x8A,0x97,0x73,
    };

    static unsigned char dh1024_g[] = {
        0x02,
    };

    DH *dh;

    if ((dh = DH_new()) == NULL) {
        return(NULL);
    }
    dh->p = BN_bin2bn(dh1024_p,sizeof(dh1024_p),NULL);
    dh->g = BN_bin2bn(dh1024_g,sizeof(dh1024_g),NULL);
    if ((dh->p == NULL) || (dh->g == NULL)) {
        DH_free(dh); 
        return(NULL); 
    }
    return dh;
}

#endif /* BIT_PACK_OPENSSL */

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
