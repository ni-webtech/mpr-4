/*
    mprOpenssl.c - Support for secure sockets via OpenSSL

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

#if BLD_FEATURE_OPENSSL

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
} MprOpenSsl;


typedef struct MprOpenSocket {
    MprSocket       *sock;
    SSL             *handle;
    BIO             *bio;
} MprOpenSocket;

typedef struct OpenSslLocks {
    MprMutex    **locks;
} OpenSslLocks;

static OpenSslLocks *olocks;

typedef struct RandBuf {
    MprTime     now;
    int         pid;
} RandBuf;

static int      numLocks;

struct CRYPTO_dynlock_value {
    MprMutex    *mutex;
};
typedef struct CRYPTO_dynlock_value DynLock;

/***************************** Forward Declarations ***************************/

static MprSocket *acceptOss(MprSocket *sp);
static void     closeOss(MprSocket *sp, bool gracefully);
static MprSsl   *getDefaultSslSettings();
static int      configureCertificateFiles(MprSsl *ssl, SSL_CTX *ctx, char *key, char *cert);
static int      configureOss(MprSsl *ssl);
static int      connectOss(MprSocket *sp, cchar *host, int port, int flags);
static MprSocketProvider *createOpenSslProvider();
static MprSocket *createOss(MprSsl *ssl);
static DH       *dhCallback(SSL *ssl, int isExport, int keyLength);
static void     disconnectOss(MprSocket *sp);
static ssize    flushOss(MprSocket *sp);
static int      listenOss(MprSocket *sp, cchar *host, int port, int flags);
static void     manageOpenSsl(MprOpenSsl *ossl, int flags);
static void     manageOpenSslLocks(OpenSslLocks *olocks, int flags);
static void     manageOpenProvider(MprSocketProvider *provider, int flags);
static void     manageOpenSocket(MprOpenSocket *ssp, int flags);
static ssize    readOss(MprSocket *sp, void *buf, ssize len);
static RSA      *rsaCallback(SSL *ssl, int isExport, int keyLength);
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
int mprCreateOpenSslModule(bool lazy)
{
    MprSocketProvider   *provider;
    RandBuf             randBuf;
    int                 i;

    if ((olocks = mprAllocObj(OpenSslLocks, manageOpenSslLocks)) == 0) {
        return MPR_ERR_MEMORY;
    }

    /*
        Get some random bytes
     */
    randBuf.now = mprGetTime();
    randBuf.pid = getpid();
    RAND_seed((void*) &randBuf, sizeof(randBuf));

#if BLD_UNIX_LIKE
    mprLog(6, "OpenSsl: Before calling RAND_load_file");
    RAND_load_file("/dev/urandom", 256);
    mprLog(6, "OpenSsl: After calling RAND_load_file");
#endif

    /*
        Configure the global locks
     */
    numLocks = CRYPTO_num_locks();
    olocks->locks = mprAlloc(numLocks * sizeof(MprMutex*));
    for (i = 0; i < numLocks; i++) {
        olocks->locks[i] = mprCreateLock();
    }
    CRYPTO_set_id_callback(sslThreadId);
    CRYPTO_set_locking_callback(sslStaticLock);

    CRYPTO_set_dynlock_create_callback(sslCreateDynLock);
    CRYPTO_set_dynlock_destroy_callback(sslDestroyDynLock);
    CRYPTO_set_dynlock_lock_callback(sslDynLock);

#if !BLD_WIN_LIKE
    OpenSSL_add_all_algorithms();
#endif

    SSL_library_init();

    if ((provider = createOpenSslProvider()) == 0) {
        return MPR_ERR_MEMORY;
    }
    provider->data = olocks;
    mprSetSecureProvider(provider);
    if (!lazy) {
        getDefaultSslSettings();
    }
    return 0;
}


static void manageOpenSslLocks(OpenSslLocks *olocks, int flags)
{
    int     i;

    if (flags & MPR_MANAGE_MARK) {
        mprMark(olocks->locks);
        for (i = 0; i < numLocks; i++) {
            mprMark(olocks->locks[i]);
        }
    } else if (flags & MPR_MANAGE_FREE) {
        olocks->locks = 0;
    }
}


static MprSsl *getDefaultSslSettings()
{
    MprSocketService    *ss;
    MprSsl              *ssl;
    MprOpenSsl          *ossl;

    ss = MPR->socketService;

    if (ss->secureProvider->defaultSsl) {
        return ss->secureProvider->defaultSsl;
    }
    if ((ssl = mprCreateSsl()) == 0) {
        return 0;
    }
    /*
        Pre-generate keys that are slow to compute.
     */
    if ((ssl->extendedSsl = mprAllocObj(MprOpenSsl, manageOpenSsl)) == 0) {
        return 0;
    }
    ss->secureProvider->defaultSsl = ssl;
    ossl = ssl->extendedSsl;
    ossl->rsaKey512 = RSA_generate_key(512, RSA_F4, 0, 0);
    ossl->rsaKey1024 = RSA_generate_key(1024, RSA_F4, 0, 0);
    ossl->dhKey512 = get_dh512();
    ossl->dhKey1024 = get_dh1024();
    return ssl;
}


/*
    Initialize a provider structure for OpenSSL
 */
static MprSocketProvider *createOpenSslProvider()
{
    MprSocketProvider   *provider;

    if ((provider = mprAllocObj(MprSocketProvider, manageOpenProvider)) == NULL) {
        return 0;
    }
    provider->name = sclone("OpenSsl");
    provider->acceptSocket = acceptOss;
    provider->closeSocket = closeOss;
    provider->configureSsl = configureOss;
    provider->connectSocket = connectOss;
    provider->createSocket = createOss;
    provider->disconnectSocket = disconnectOss;
    provider->flushSocket = flushOss;
    provider->listenSocket = listenOss;
    provider->readSocket = readOss;
    provider->writeSocket = writeOss;
    return provider;
}


static void manageOpenProvider(MprSocketProvider *provider, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(provider->name);
        mprMark(provider->defaultSsl);
        mprMark(provider->data);
    }
}


/*
    Initialize a server-side SSL configuration. An application can have multiple different SSL configurations
    for different routes.
 */
static int configureOss(MprSsl *ssl)
{
    MprSsl              *defaultSsl;
    MprOpenSsl          *ossl, *src;
    SSL_CTX             *context;
    uchar               resume[16];

    mprAssert(ssl);

    if ((context = SSL_CTX_new(SSLv23_method())) == 0) {
        mprError("OpenSSL: Unable to create SSL context"); 
        return MPR_ERR_CANT_CREATE;
    }

    SSL_CTX_set_app_data(context, (void*) ssl);
#if UNUSED
    SSL_CTX_set_quiet_shutdown(context, 1);
#endif
    SSL_CTX_sess_set_cache_size(context, 512);

    RAND_bytes(resume, sizeof(resume));
    SSL_CTX_set_session_id_context(context, resume, sizeof(resume));

    /*
        Configure the certificates
     */
#if FUTURE
    if (ssl->key || ssl->cert) {
        if (configureCertificates(ssl, context, ssl->key, ssl->cert) != 0) {
            SSL_CTX_free(context);
            return MPR_ERR_CANT_INITIALIZE;
        }
    } else 
#endif
    if (ssl->keyFile || ssl->certFile) {
        if (configureCertificateFiles(ssl, context, ssl->keyFile, ssl->certFile) != 0) {
            SSL_CTX_free(context);
            return MPR_ERR_CANT_INITIALIZE;
        }
    }

    mprLog(4, "OpenSSL: Using ciphers %s", ssl->ciphers);
    SSL_CTX_set_cipher_list(context, ssl->ciphers);

    /*
        Configure the client verification certificate locations
     */
    if (ssl->verifyClient) {
        if (ssl->caFile == 0 && ssl->caPath == 0) {
            mprError("OpenSSL: Must define CA certificates if using client verification");
            SSL_CTX_free(context);
            return MPR_ERR_BAD_STATE;
        }
        if (ssl->caFile || ssl->caPath) {
            if ((!SSL_CTX_load_verify_locations(context, ssl->caFile, ssl->caPath)) ||
                    (!SSL_CTX_set_default_verify_paths(context))) {
                mprError("OpenSSL: Unable to set certificate locations"); 
                SSL_CTX_free(context);
                return MPR_ERR_CANT_ACCESS;
            }
            if (ssl->caFile) {
                STACK_OF(X509_NAME) *certNames;
                certNames = SSL_load_client_CA_file(ssl->caFile);
                if (certNames == 0) {
                    ;
                } else {
                    /*
                        Define the list of CA certificates to send to the client
                        before they send their client certificate for validation
                     */
                    SSL_CTX_set_client_CA_list(context, certNames);
                }
            }
        }
        mprLog(4, "OpenSSL: enable verification of client connections");

        if (ssl->caFile) {
            mprLog(4, "OpenSSL: Using certificates from %s", ssl->caFile);

        } else if (ssl->caPath) {
            mprLog(4, "OpenSSL: Using certificates from directory %s", ssl->caPath);
        }
        SSL_CTX_set_verify(context, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, verifyX509Certificate);
        SSL_CTX_set_verify_depth(context, ssl->verifyDepth);

    } else {
        SSL_CTX_set_verify(context, SSL_VERIFY_NONE, verifyX509Certificate);
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
        mprLog(4, "OpenSSL: Disabling SSLv3");
    }
    if (!(ssl->protocols & MPR_PROTO_TLSV1)) {
        SSL_CTX_set_options(context, SSL_OP_NO_TLSv1);
        mprLog(4, "OpenSSL: Disabling TLSv1");
    }
    /* 
        Ensure we generate a new private key for each connection
     */
    SSL_CTX_set_options(context, SSL_OP_SINGLE_DH_USE);

    if ((defaultSsl = getDefaultSslSettings()) == 0) {
        return MPR_ERR_MEMORY;
    }
    if (ssl != defaultSsl) {
        if (!ssl->extendedSsl && (ssl->extendedSsl = mprAllocObj(MprOpenSsl, manageOpenSsl)) == 0) {
            return 0;
        }
        ossl = ssl->extendedSsl;
        src = defaultSsl->extendedSsl;
        ossl->rsaKey512 = src->rsaKey512;
        ossl->rsaKey1024 = src->rsaKey1024;
        ossl->dhKey512 = src->dhKey512;
        ossl->dhKey1024 = src->dhKey1024;
    } else {
        ossl = ssl->extendedSsl;
    }
    mprAssert(ossl);
    ossl->context = context;
    return 0;
}


/*
    Update the destructor for the MprSsl object. 
 */
static void manageOpenSsl(MprOpenSsl *ossl, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        ;

    } else if (flags & MPR_MANAGE_FREE) {
        if (ossl->context != 0) {
            SSL_CTX_free(ossl->context);
            ossl->context = 0;
        }
        if (ossl == MPR->socketService->secureProvider->defaultSsl->extendedSsl) {
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
    Create a new socket. If listen is set, this is a socket for an accepting connection.
 */
static MprSocket *createOss(MprSsl *ssl)
{
    MprSocketService    *ss;
    MprSocket           *sp;
    MprOpenSocket       *osp;
    
    if (ssl == MPR_SECURE_CLIENT) {
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

    /*
        Create a SslSocket object for ssl state. This logically extends MprSocket.
     */
    if ((osp = (MprOpenSocket*) mprAllocObj(MprOpenSocket, manageOpenSocket)) == 0) {
        return 0;
    }
    osp->sock = sp;
    sp->sslSocket = osp;
    sp->ssl = ssl;
    unlock(sp);
    return sp;
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
    Initialize a new server-side connection. Called by listenOss and by acceptOss.
 */
static int listenOss(MprSocket *sp, cchar *host, int port, int flags)
{
    return sp->service->standardProvider->listenSocket(sp, host, port, flags);
}


/*
    Initialize a new server-side connection
 */
static MprSocket *acceptOss(MprSocket *listen)
{
    MprSocket       *sp;
    MprOpenSocket   *osp;
    MprOpenSsl      *ossl;
    BIO             *bioSock;
    SSL             *handle;

    if ((sp = listen->service->standardProvider->acceptSocket(listen)) == 0) {
        return 0;
    }
    lock(sp);
    osp = sp->sslSocket;
    mprAssert(osp);

    /*
        Create and configure the SSL struct
     */
    ossl = sp->ssl->extendedSsl;
    if ((handle = (SSL*) SSL_new(ossl->context)) == 0) {
        unlock(sp);
        return 0;
    }
    osp->handle = handle;
    SSL_set_app_data(handle, (void*) osp);

    /*
        Create a socket bio
     */
    bioSock = BIO_new_socket(sp->fd, BIO_NOCLOSE);
    mprAssert(bioSock);
    SSL_set_bio(handle, bioSock, bioSock);
    SSL_set_accept_state(handle);
    osp->bio = bioSock;
    unlock(sp);
    return sp;
}


/*
    Initialize a new client connection
 */
static int connectOss(MprSocket *sp, cchar *host, int port, int flags)
{
    MprSocketService    *ss;
    MprOpenSocket       *osp;
    MprOpenSsl          *ossl;
    MprSsl              *ssl;
    BIO                 *bioSock;
    int                 rc;
    
    lock(sp);
    ss = sp->service;
    if (ss->standardProvider->connectSocket(sp, host, port, flags) < 0) {
        unlock(sp);
        return MPR_ERR_CANT_CONNECT;
    }
    osp = sp->sslSocket;
    mprAssert(osp);

    if (!sp->ssl) {
        if ((ssl = ss->secureProvider->defaultSsl) == 0) {
            if ((ssl = getDefaultSslSettings()) == 0) {
                unlock(sp);
                return MPR_ERR_CANT_INITIALIZE;
            }
        }
    }
    sp->ssl = ssl;
    ossl = ssl->extendedSsl;

    if (ossl->context == 0 && configureOss(ssl) < 0) {
        unlock(sp);
        return MPR_ERR_CANT_INITIALIZE;
    }
    /*
        Create and configure the SSL struct
     */
    osp->handle = (SSL*) SSL_new(ossl->context);
    mprAssert(osp->handle);
    if (osp->handle == 0) {
        unlock(sp);
        return MPR_ERR_CANT_INITIALIZE;
    }
    SSL_set_app_data(osp->handle, (void*) osp);

    /*
        Create a socket bio
     */
    bioSock = BIO_new_socket(sp->fd, BIO_NOCLOSE);
    mprAssert(bioSock);
    SSL_set_bio(osp->handle, bioSock, bioSock);

    //  TODO - should be calling SSL_set_connect_state(osp->handle);
    osp->bio = bioSock;

    /*
        Make the socket blocking while we connect
     */
    mprSetSocketBlockingMode(sp, 1);
    
    rc = SSL_connect(osp->handle);
    if (rc < 1) {
#if KEEP
        rc = SSL_get_error(osp->handle, rc);
        if (rc == SSL_ERROR_WANT_READ) {
            rc = SSL_connect(osp->handle);
        }
#endif
        unlock(sp);
        return MPR_ERR_CANT_CONNECT;
    }
    mprSetSocketBlockingMode(sp, 0);
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
    MprOpenSocket    *osp;
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
            char    ebuf[MPR_MAX_STRING];
            error = SSL_get_error(osp->handle, rc);
            if (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_CONNECT || error == SSL_ERROR_WANT_ACCEPT) {
                continue;
            }
            ERR_error_string_n(error, ebuf, sizeof(ebuf) - 1);
            mprLog(4, "SSL_read error %d, %s", error, ebuf);
        }
        break;
    }

#if DEBUG && UNUSED
    if (rc > 0 && !connTraced) {
        X509_NAME   *xSubject;
        X509        *cert;
        char        subject[260], issuer[260], peer[260];

        mprLog(4, "%d: OpenSSL Connected using: \"%s\"", sock, SSL_get_cipher(ssl));

        cert = SSL_get_peer_certificate(ssl);
        if (cert == 0) {
            mprLog(4, "%d: OpenSSL Details: client supplied no certificate", sock);

        } else {
            xSubject = X509_get_subject_name(cert);
            X509_NAME_oneline(xSubject, subject, sizeof(subject) -1);
            X509_NAME_oneline(X509_get_issuer_name(cert), issuer, sizeof(issuer) -1);
            X509_NAME_get_text_by_NID(xSubject, NID_commonName, peer, sizeof(peer) - 1);
            mprLog(4, "%d: OpenSSL Subject %s", sock, subject);
            mprLog(4, "%d: OpenSSL Issuer: %s", sock, issuer);
            mprLog(4, "%d: OpenSSL Peer: %s", sock, peer);
            X509_free(cert);
        }
        connTraced = 1;
    }
#endif

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

    if (!ssl->verifyClient) {
        return ok;
    }
    cert = X509_STORE_CTX_get_current_cert(xContext);
    depth = X509_STORE_CTX_get_error_depth(xContext);
    error = X509_STORE_CTX_get_error(xContext);

    if (X509_NAME_oneline(X509_get_subject_name(cert), subject, sizeof(subject) - 1) < 0) {
        ok = 0;
    }
    /*
        TODO -- should compare subject name and host name. Need smart compare.
     */
    if (X509_NAME_oneline(X509_get_issuer_name(xContext->current_cert), issuer, sizeof(issuer) - 1) < 0) {
        ok = 0;
    }
    if (X509_NAME_get_text_by_NID(X509_get_subject_name(xContext->current_cert), NID_commonName, peer, 
            sizeof(peer) - 1) < 0) {
        ok = 0;
    }

    /*
        Customizers: add your own code here to validate client certificates
     */
    if (ok && ssl->verifyDepth < depth) {
        if (error == 0) {
            error = X509_V_ERR_CERT_CHAIN_TOO_LONG;
        }
        ok = 0;
    }
    if (error != 0) {
        mprAssert(!ok);
        /* X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY */
    }

#if KEEP
    switch (error) {
    case X509_V_ERR_CERT_HAS_EXPIRED:
    case X509_V_ERR_CERT_NOT_YET_VALID:
    case X509_V_ERR_CERT_REJECTED:
    case X509_V_ERR_CERT_SIGNATURE_FAILURE:
    case X509_V_ERR_CERT_UNTRUSTED:
    case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
    case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
    case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
    case X509_V_ERR_INVALID_CA:
    case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
    default:
        ok = 0;
        break;
    }
#endif
    if (!ok) {
        mprLog(0, "OpenSSL: Certification failed: subject %s", subject);
        mprLog(4, "OpenSSL: Issuer: %s", issuer);
        mprLog(4, "OpenSSL: Peer: %s", peer);
        mprLog(4, "OpenSSL: Error: %d: %s", error, X509_verify_cert_error_string(error));
    } else {
        mprLog(0, "OpenSSL: Certificate verified: subject %s", subject);
        mprLog(4, "OpenSSL: Issuer: %s", issuer);
        mprLog(4, "OpenSSL: Peer: %s", peer);
    }
    return ok;
}


static ssize flushOss(MprSocket *sp)
{
#if KEEP
    MprOpenSocket    *osp;

    osp = (MprOpenSocket*) sp->sslSocket;

    mprAssert(0);
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

    if (olocks->locks) {
        if (mode & CRYPTO_LOCK) {
            mprLock(olocks->locks[n]);
        } else {
            mprUnlock(olocks->locks[n]);
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

    ossl = sp->ssl->extendedSsl;

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
    ossl = sp->ssl->extendedSsl;

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

    if ((dh=DH_new()) == NULL) {
        return(NULL);
    }

    dh->p=BN_bin2bn(dh512_p,sizeof(dh512_p),NULL);
    dh->g=BN_bin2bn(dh512_g,sizeof(dh512_g),NULL);

    if ((dh->p == NULL) || (dh->g == NULL)) { 
        DH_free(dh); return(NULL); 
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

    if ((dh=DH_new()) == NULL) return(NULL);
    dh->p=BN_bin2bn(dh1024_p,sizeof(dh1024_p),NULL);
    dh->g=BN_bin2bn(dh1024_g,sizeof(dh1024_g),NULL);
    if ((dh->p == NULL) || (dh->g == NULL)) {
        DH_free(dh); 
        return(NULL); 
    }
    return dh;
}

#else
int mprCreateOpenSslModule(bool lazy) { return -1; }
#endif /* BLD_FEATURE_OPENSSL */

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
