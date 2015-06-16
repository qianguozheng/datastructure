#include <Python.h>

#include <openssl/asn1.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/pem.h>

/* Type of key */
#define PUBLIC   1
#define PRIVATE  2

/* Minimum and maximum number of bits for a key */
#define MINBITS   1024
#define MAXBITS   8192

/* Exception declarations */
static PyObject *keySizeError;

/* Call back function used when generating an RSA key */
static int genrsa_callback(int p, int n, BN_GENCB *cb)
{
    char c = '*';

    if (p == 0) c = '.';
    if (p == 1) c = '+';
    if (p == 2) c = '*';
    if (p == 3) c = '\n';
    BIO_write(cb->arg, &c, 1);
    (void)BIO_flush(cb->arg);
    return 1;
}

static RSA *getRSAFromPython(PyObject *temp)
{
    RSA * rsa = (RSA *)PyCObject_AsVoidPtr(temp);
    /* We should verify we have a valid RSA object here, but how? */
    if (!rsa) {
        PyErr_SetString(PyExc_ValueError, "Invalid RSA object");
        return NULL;
    }
    return rsa;
}

/* This function is used when writing an RSA key to either a file
 * or, if fn is NULL, stdout. The key can be either the private or
 * public version depending on the which value passed in.
 */
static int writeRSA(RSA *rsa, int which, const char *fn)
{
    BIO *out = NULL;
    int rv = -1;
    EVP_CIPHER *enc = NULL;

    if (!rsa)
        return 0;

    out = BIO_new(BIO_s_file());
    if (!out)
        return 0;

    if (fn)
        rv = BIO_write_filename(out, (char *)fn);
    else
        rv = BIO_set_fp(out, stdout, BIO_NOCLOSE);
    if (rv != 1)
        return 0;

    if (which == PRIVATE) 
        rv = PEM_write_bio_RSAPrivateKey(out, rsa, enc, NULL, 0, NULL, 
                                        NULL);
    else
        rv = PEM_write_bio_RSA_PUBKEY(out,rsa);

    /* Flush the BIO to make sure it's all written. */
    (void)BIO_flush(out);
    if (fn)
        BIO_free_all(out);

    return rv;
}

static RSA *readKeyFromBio(BIO *in, int which)
{
    EVP_PKEY *pkey = NULL;
    RSA *rsa = NULL;

    if (which == PUBLIC)
        pkey=PEM_read_bio_PUBKEY(in, NULL, NULL, NULL);
    else
        pkey=PEM_read_bio_PrivateKey(in, NULL, NULL, NULL);

    if (pkey) {
        rsa = EVP_PKEY_get1_RSA(pkey);
        if (rsa)
            RSA_up_ref(rsa);
        EVP_PKEY_free(pkey);
    }
    return rsa;
}

static RSA *readKeyFromMemory(char *ptr, int len, int which)
{
    BIO *in = BIO_new_mem_buf(ptr, len);
    RSA *rsa = NULL;

    if (!in) {
        PyErr_SetString(PyExc_MemoryError, "Unable to create a BIO object");
        return NULL;
    }

    rsa = readKeyFromBio(in, which);
    if (rsa == NULL)
        PyErr_SetString(PyExc_IOError, "Unable to get public RSA key from memory");

    (void)BIO_free_all(in);

    if (rsa) {
        int strength = BN_num_bits(rsa->n);
        if (strength < MINBITS || strength > MAXBITS) {
            RSA_free(rsa);
            rsa = NULL;
            PyErr_SetString(keySizeError, "Invalid key strength");
        }
    }
    return rsa;
}

static RSA *readKey(const char *fn, int which)
{
    RSA *rsa = NULL;
    BIO *in = BIO_new(BIO_s_file());

    if (!in) {
        PyErr_SetString(PyExc_MemoryError, "Unable to create a BIO object");
        return NULL;
    }

    if (BIO_read_filename(in, fn) == 1) {
        rsa = readKeyFromBio(in, which);
        if (rsa == NULL)
            PyErr_SetString(PyExc_IOError, "Unable to get public RSA key from file");
    } else
        PyErr_SetString(PyExc_IOError, "Unable to read from file supplied");

    (void)BIO_free_all(in);

    if (rsa) {
        int strength = BN_num_bits(rsa->n);
        if (strength < MINBITS || strength > MAXBITS) {
            RSA_free(rsa);
            rsa = NULL;
            PyErr_SetString(keySizeError, "Invalid key strength");
        }
    }

    return rsa;
}

/* This function is used when writing an RSA key to either a file
 * or, if fn is NULL, stdout. The key can be either the private or
 * public version depending on the which value passed in.
 */
static long getRSAString(RSA *rsa, int which, char **strPtr)
{
    BIO *out = NULL;
    EVP_CIPHER *enc = NULL;
    long sz = 0;
    int rv;

    if (!rsa)
        return 0;

    out = BIO_new(BIO_s_mem());
    if (!out)
        return 0;

    if (which == PRIVATE) 
        rv = PEM_write_bio_RSAPrivateKey(out, rsa, enc, NULL, 0, NULL, 
                                        NULL);
    else
        rv = PEM_write_bio_RSA_PUBKEY(out,rsa);

    /* Flush the BIO to make sure it's all written. */
    (void)BIO_flush(out);

    sz = BIO_get_mem_data(out, strPtr);
    (void)BIO_set_close(out, BIO_NOCLOSE);
    BIO_free(out);

    return sz;
}

/* This fucntion is called when an RSA pointer is finally freed by
 * Python. This is done via setting this as the 2nd argument in
 * PyCObject_FromVoidPtr().
 */
static void delrsa(void *ptr)
{
    RSA_free((RSA *)ptr);
}

/* genrsa(bits)
   bits = the strength of key required
   Returns a pointer to an RSA object.
 */
static PyObject *
genrsa(PyObject *self, PyObject *args)
{
    BN_GENCB cb;
    RSA *rsa = RSA_new();
    BIGNUM *bn = BN_new();
    BIO *bio_err=NULL;
    int numBits = 0;
    unsigned long f4 = RSA_F4;

    BN_GENCB_set(&cb, genrsa_callback, bio_err);

    PyArg_ParseTuple(args, "i", &numBits);
    if (numBits < 1024 || numBits > 8192) {
        PyErr_SetString(keySizeError, "Invalid key strength requested");
        return NULL;
    }

    if (BN_set_word(bn, f4) &&
        RSA_generate_key_ex(rsa, numBits, bn, &cb)) {
        /* return a pointer to the RSA structure */
        RSA_up_ref(rsa);
        return PyCObject_FromVoidPtr(rsa, delrsa);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

/* This function gets a pointer to the RSA object representing the
 * key associated with the certificate. We simply get a pointer rather
 * than duplicating presently and rely on the internal reference
 * counting of OpenSSL to ensure it's always available to us.
 */
static PyObject *
fromX509(PyObject *self, PyObject *args)
{
    X509 *cert = NULL;
    RSA *rsa = NULL;
    EVP_PKEY *pkey = NULL;
    PyObject *pCert = NULL;

    if (!PyArg_ParseTuple(args, "O", &pCert))
        return NULL;
    cert = (X509 *)PyCObject_AsVoidPtr(pCert);

    pkey = X509_get_pubkey(cert);
    if (!pkey) {
        PyErr_SetString(PyExc_ValueError, "Invalid certificate object supplied");
        return NULL;
    }

    if (pkey) {
        if (pkey->type == EVP_PKEY_RSA) {
            rsa = EVP_PKEY_get1_RSA(pkey);
    /* When we call EVP_PKEY_free() it will decrement the ref counter
     * for the object pointer we have stored in rsa, so we need to
     * increment the reference ourselves so it stays in memory and isn't
     * released at this point.
     */
            if (rsa)
                RSA_up_ref(rsa);
        }
        EVP_PKEY_free(pkey);
        if (rsa)
            return PyCObject_FromVoidPtr(rsa, delrsa);
    }
    PyErr_SetString(PyExc_ValueError, "No RSA key found in certificate");
    return NULL;
}

static PyObject *
fromX509_REQ(PyObject *self, PyObject *args)
{
    void *tmp = NULL;
    X509_REQ *req = NULL;
    EVP_PKEY *pkey = NULL;
    RSA *rsa = NULL;

    if (! PyArg_ParseTuple(args, "O", &tmp))
        return NULL;

    req = (X509_REQ *)PyCObject_AsVoidPtr(tmp);
    if (!req) {
        PyErr_SetString(PyExc_TypeError, "Invalid X509_REQ object passed");
        return NULL;
    }

    pkey = X509_REQ_get_pubkey(req);
    if (pkey) {
        if (pkey->type == EVP_PKEY_RSA) {
            rsa = EVP_PKEY_get1_RSA(pkey);
    /* When we call EVP_PKEY_free() it will decrement the ref counter
     * for the object pointer we have stored in rsa, so we need to
     * increment the reference ourselves so it stays in memory and isn't
     * released at this point.
     */
            if (rsa)
                RSA_up_ref(rsa);
        }
        EVP_PKEY_free(pkey);
        if (rsa)
            return PyCObject_FromVoidPtr(rsa, delrsa);
    }
    PyErr_SetString(PyExc_ValueError, "No RSA key found within CSR object");
    return NULL;
}






/* Read an RSA key from a file. when we read it in we also need to
 * check the key size, as we don't allow keys with less than MINBITS
 * or greater than MAXBITS.
 */
static PyObject *
readRSA(PyObject *self, PyObject *args)
{
    RSA *rsa = NULL;
    char *fn = NULL;

    if (! PyArg_ParseTuple(args, "s", &fn))
        return NULL;

    rsa = readKey(fn, PRIVATE);
    if (! rsa)
        return NULL;

    return PyCObject_FromVoidPtr(rsa, delrsa);
}

static PyObject *
readRSAPublic(PyObject *self, PyObject *args)
{
    RSA *rsa = NULL;
    char *fn = NULL;

    if (! PyArg_ParseTuple(args, "s", &fn))
        return NULL;

    rsa = readKey(fn, PUBLIC);
    if (!rsa)
        return NULL;

    return PyCObject_FromVoidPtr(rsa, delrsa);
}

static PyObject *
readRSAPublicFromMemory(PyObject *self, PyObject *args)
{
    RSA *rsa = NULL;
    char *ptr = NULL;
    int len = 0;

    if (! PyArg_ParseTuple(args, "s#", &ptr, &len))
        return NULL;
    if (len == 0) {
        PyErr_SetString(PyExc_ValueError, "Invalid length of buffer!");
        return NULL;
    }
    rsa = readKeyFromMemory(ptr, len, PUBLIC);
    if (!rsa)
        return NULL;

    return PyCObject_FromVoidPtr(rsa, delrsa);
}

static PyObject *
writeRSAPrivate(PyObject *self, PyObject *args)
{
    int i = 0;
    RSA *rsa = NULL;
    char *fn = NULL;
    PyObject *temp = NULL;

    if (! PyArg_ParseTuple(args, "Oz", &temp, &fn))
        return NULL;
    if ((rsa = getRSAFromPython(temp)) == NULL)
        return NULL;

    i = writeRSA(rsa, PRIVATE, fn);

    return Py_BuildValue("i", i);
}

static PyObject *
writeRSAPublic(PyObject *self, PyObject *args)
{
    RSA *rsa = NULL;
    char *fn = NULL;
    int i = 0;
    PyObject *temp = NULL;

    if (! PyArg_ParseTuple(args, "Oz", &temp, &fn)) {
        return NULL;
    }

    if ((rsa = getRSAFromPython(temp)) == NULL)
        return NULL;

    i = writeRSA(rsa, PUBLIC, fn);

    return Py_BuildValue("i", i);
}

static PyObject *
getRSAPublic(PyObject *self, PyObject *args)
{
    RSA *rsa = NULL, *pubRSA = NULL;
    PyObject *temp = NULL;

    if (! PyArg_ParseTuple(args, "O", &temp))
        return NULL;

    if ((rsa = getRSAFromPython(temp)) == NULL)
        return NULL;

    pubRSA = RSAPublicKey_dup(rsa);
    if (pubRSA)
        return PyCObject_FromVoidPtr(pubRSA, delrsa);

    /* TODO - PyErr_SetString(...) here */
    return NULL;
}

static PyObject *
getRSAPublicString(PyObject *self, PyObject *args)
{
    RSA *rsa = NULL;
    long i = 0;
    PyObject *temp = NULL;
    char *ptr = NULL;

    if (! PyArg_ParseTuple(args, "O", &temp))
        return NULL;

    if ((rsa = getRSAFromPython(temp)) == NULL)
        return NULL;

    i = getRSAString(rsa, PUBLIC, &ptr);
    
    return PyString_FromStringAndSize(ptr, i);
}

static PyObject *
getBits(PyObject *self, PyObject *args)
{
    RSA *rsa = NULL;
    PyObject *temp = NULL;

    if (! PyArg_ParseTuple(args, "O", &temp))
        return NULL;

    if ((rsa = getRSAFromPython(temp)) == NULL)
        return NULL;

    return Py_BuildValue("l", BN_num_bits(rsa->n));
}

static PyObject *
getModulus(PyObject *self, PyObject *args)
{
    RSA *rsa = NULL;
    PyObject *temp = NULL, *rv = NULL;
    BIO *out = NULL;

    if (! PyArg_ParseTuple(args, "O", &temp))
        return NULL;

    if ((rsa = getRSAFromPython(temp)) == NULL)
        return NULL;

    out = BIO_new(BIO_s_mem());
    if (out) {
        char *ptr = NULL;
        long sz = 0;
    
        BN_print(out,rsa->n);
        (void)BIO_flush(out);
        sz = BIO_get_mem_data(out, &ptr);
        rv = PyString_FromStringAndSize(ptr, sz);
        BIO_free(out);        
    }
    return rv;
}

static PyMethodDef RSAMethods[] = {
    { "genrsa", genrsa, METH_VARARGS, "Generate an RSA Key" },
    { "fromX509", fromX509, METH_VARARGS, "Get an RSA key from an X509 certificate" },
    { "fromCSR", fromX509_REQ, METH_VARARGS, "Get an RSA key from an X509 request" },
    { "read", readRSA, METH_VARARGS, "Read an RSA private key from a file" },
    { "readPublic", readRSAPublic, METH_VARARGS, "Read an RSA public key from a file" },
    { "fromMemoryPublic", readRSAPublicFromMemory, METH_VARARGS, "Read an RSA key from memory" },
    { "writePrivate", writeRSAPrivate, METH_VARARGS, "Write a private RSA key to a file" },
    { "writePublic", writeRSAPublic, METH_VARARGS, "Write public key to file" },
    { "getPublicKey", getRSAPublic, METH_VARARGS, "Get public key as an object" },
    { "getPublicKeyString", getRSAPublicString, METH_VARARGS, "Get public key as a string" },
    { "getKeyStrength", getBits, METH_VARARGS, "Get the key strength (in bits)" },
    { "getModulus", getModulus, METH_VARARGS, "Get the key modulus" },
    { NULL, NULL, 0, NULL },
};

PyMODINIT_FUNC
initrsa(void)
{
    PyObject *mod = NULL;
    CRYPTO_malloc_init();
    ERR_load_crypto_strings(); 
    OpenSSL_add_all_algorithms();

    mod = Py_InitModule("rsa", RSAMethods);

    keySizeError = PyErr_NewException("rsa.keySize", NULL, NULL);
    Py_INCREF(keySizeError);
    PyModule_AddObject(mod, "keySize", keySizeError);
}
