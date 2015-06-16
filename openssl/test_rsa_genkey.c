#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
 
/************************************************************************
 * RSA密钥生成函数
 *
 * file: test_rsa_genkey.c
 * gcc -Wall -O2 -o test_rsa_genkey test_rsa_genkey.c -lcrypto
 *
 * author: tonglulin@gmail.com by www.qmailer.net
 * modified: guozhengqian0825@126.com
 * 
 * Ref: http://www.qmailer.net/archives/216.html
 * 		http://stackoverflow.com/questions/17167254/rsa-encryption-using-openssl-variable-length-for-encrypted-text-with-pkcs1-padd
 ************************************************************************/
 
char *RSAReadKeyFromFile(char *prikey_path)
{
    RSA *rsa = NULL;
    FILE *fp = NULL;
    char *de = NULL;
 
    if ((fp = fopen("privateKey.pem", "r")) == NULL) {
        return NULL;
    }
 
    if ((rsa = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL)) == NULL) {
        return NULL;
    }
    
    RSA_print_fp(stdout, rsa, 0);
    
    printf("\n\nMiddle\n\n");
    
    
    //=========================================
    unsigned char *n_b = (unsigned char *)calloc(RSA_size(rsa), sizeof(unsigned char));
    unsigned char *e_b = (unsigned char *)calloc(RSA_size(rsa), sizeof(unsigned char));
    int n_size = BN_bn2bin(rsa->n, n_b);
    int b_size = BN_bn2bin(rsa->e, e_b);
    RSA *pubrsa = RSA_new();
    pubrsa->n = BN_bin2bn(n_b, n_size, NULL);
    pubrsa->e = BN_bin2bn(e_b, b_size, NULL);
    PEM_write_RSAPublicKey(stdout, pubrsa);
    //=========================================
    FILE *fp1 = NULL;
    
	if ((fp1 = fopen("publicKey.pem", "r")) == NULL) {
        return NULL;
    }
    
	if ((rsa = PEM_read_RSAPublicKey(fp1, NULL, NULL, NULL)) == NULL) {
        return NULL;
    }
    RSA_print_fp(stdout, rsa, 0);
 
    RSA_free(rsa);
    fclose(fp);
 
    return de;
}
/* This function is used when writing an RSA key to either a file
 * or, if fn is NULL, stdout. The key can be either the private or
 * public version depending on the which value passed in.
 */
static int writeRSA(RSA *rsa, const char *fn)
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

   // if (which == PRIVATE) 
        rv = PEM_write_bio_RSAPrivateKey(out, rsa, enc, NULL, 0, NULL, 
                                        NULL);
             //PEM_write_bio_RSAPrivateKey
   // else
   //     rv = PEM_write_bio_RSA_PUBKEY(out,rsa);

    /* Flush the BIO to make sure it's all written. */
    (void)BIO_flush(out);
    if (fn)
        BIO_free_all(out);

    return rv;
}

/*static RSA *readKeyFromBio(BIO *in, int which)
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
}*/
int main(int argc, char *argv[])
{
    /* 产生RSA密钥 */
    RSA *rsa = RSA_new();
    BIGNUM* e = BN_new();
    
    BN_set_word(e, 65537);
    
    RSA_generate_key_ex(rsa, 2048, e, NULL);
 
    printf("BIGNUM: %s\n", BN_bn2hex(rsa->n));
 
    /* 提取私钥 */
    printf("PRIKEY:\n");
    
    FILE *filename = NULL;
    filename = fopen("privateKey.pem", "wb");
    PEM_write_RSAPrivateKey(filename, rsa, NULL, NULL, 0, NULL, NULL);
    fclose(filename);
 
	//writeRSA(rsa, NULL);
    /* 提取公钥 */
    
    unsigned char *n_b = (unsigned char *)calloc(RSA_size(rsa), sizeof(unsigned char));
    unsigned char *e_b = (unsigned char *)calloc(RSA_size(rsa), sizeof(unsigned char));
 
    int n_size = BN_bn2bin(rsa->n, n_b);
    int b_size = BN_bn2bin(rsa->e, e_b);
 
    RSA *pubrsa = RSA_new();
    pubrsa->n = BN_bin2bn(n_b, n_size, NULL);
    pubrsa->e = BN_bin2bn(e_b, b_size, NULL);
 
    printf("PUBKEY: \n");
    
    FILE *publicKey = NULL;
    publicKey = fopen("publicKey.pem", "wb");
    PEM_write_RSAPublicKey(publicKey, pubrsa);
    fclose(publicKey);
 
    RSA_free(rsa);
    RSA_free(pubrsa);

	RSAReadKeyFromFile("privateKey.pem");
    return 0;
}


