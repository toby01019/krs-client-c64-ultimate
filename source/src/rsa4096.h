#ifndef KRS_RSA4096_H
#define KRS_RSA4096_H
/* Public verification only. No private RSA operations are provided. */
#define RSA_BYTES 512
extern unsigned char rsa_message_hash[32];
extern unsigned char *rsa_signature;
extern unsigned int rsa_signature_len;
extern unsigned char rsa_result;
extern unsigned char *hash_ptr;
extern unsigned int hash_len;
extern unsigned char hash_out[32];
void crypto_hash(void);
void rsa_verify(void);
unsigned char rsa_check_certificate(const unsigned char*,unsigned int);
#endif
