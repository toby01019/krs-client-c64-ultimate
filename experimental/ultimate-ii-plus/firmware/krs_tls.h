#ifndef KRS_TLS_H
#define KRS_TLS_H
#include <stddef.h>
#include <stdint.h>
#include "bearssl.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef int (*krs_io_fn)(void *, unsigned char *, size_t);
typedef int (*krs_write_fn)(void *, const unsigned char *, size_t);
typedef struct {
    br_ssl_client_context client;
    br_x509_knownkey_context known;
    unsigned char buffer[BR_SSL_BUFSIZE_BIDI];
    br_sslio_context io;
    unsigned char authenticated;
    int last_error; /* public diagnostic only; no key material */
} krs_tls;
/* Each connection REQUIRES a fresh, unpredictable 32-byte seed. No fallback.
 * The caller owns transport timeouts, socket closure and seed quality.
 * Public key storage must outlive this context. Pin is a possession check,
 * not a CA/name/date check; Finished must authenticate before open succeeds.
 */
int krs_tls_open(krs_tls *, const unsigned char *, size_t,
                 const br_rsa_public_key *, krs_io_fn, krs_write_fn, void *);
int krs_tls_write(krs_tls *, const unsigned char *, size_t);
int krs_tls_read(krs_tls *, unsigned char *, size_t);
void krs_tls_clear(krs_tls *);
#ifdef __cplusplus
}
#endif
#endif
