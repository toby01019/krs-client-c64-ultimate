#include "krs_tls.h"
#include <string.h>

void krs_tls_clear(krs_tls *s) {
    volatile unsigned char *p = (volatile unsigned char *)s;
    size_t n = sizeof(*s);
    while (n--) *p++ = 0;
}

int krs_tls_open(krs_tls *s, const unsigned char *seed, size_t seed_len,
                 const br_rsa_public_key *pin, krs_io_fn rd,
                 krs_write_fn wr, void *ctx) {
    br_x509_minimal_context temporary;
    static const uint16_t suites[] = {BR_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256};
    krs_tls_clear(s);
    if (!seed || seed_len != 32 || !pin || !rd || !wr) return 0;
    br_ssl_client_init_full(&s->client, &temporary, NULL, 0);
    /* Use the pinned key for server possession, regardless of its untrusted
       certificate contents. A wrong key cannot authenticate server Finished. */
    br_x509_knownkey_init_rsa(&s->known, pin, BR_KEYTYPE_SIGN);
    br_ssl_engine_set_x509(&s->client.eng, &s->known.vtable);
    br_ssl_engine_set_versions(&s->client.eng, BR_TLS12, BR_TLS12);
    br_ssl_engine_set_suites(&s->client.eng, suites, 1);
    /* Portable RSA signature verification; ECDHE provides forward secrecy.
       Never resume or renegotiate: each open starts with a wiped context. */
    br_ssl_engine_set_rsavrfy(&s->client.eng, br_rsa_i31_pkcs1_vrfy);
    br_ssl_engine_add_flags(&s->client.eng, BR_OPT_NO_RENEGOTIATION);
    br_ssl_engine_set_buffer(&s->client.eng, s->buffer, sizeof(s->buffer), 1);
    br_ssl_engine_inject_entropy(&s->client.eng, seed, seed_len);
    if (!br_ssl_client_reset(&s->client, "quipzz.bplaced.net", 0)) {
        int err = br_ssl_engine_last_error(&s->client.eng);
        krs_tls_clear(s); s->last_error = err; return 0;
    }
    br_sslio_init(&s->io, &s->client.eng, rd, ctx, wr, ctx);
    /* flush drives the handshake to authenticated application readiness;
       no HTTP/login bytes are supplied before this succeeds. */
    if (br_sslio_flush(&s->io) < 0 ||
        !(br_ssl_engine_current_state(&s->client.eng) & BR_SSL_SENDAPP)) {
        int err = br_ssl_engine_last_error(&s->client.eng);
        krs_tls_clear(s); s->last_error = err; return 0;
    }
    s->authenticated = 1;
    return 1;
}

int krs_tls_write(krs_tls *s, const unsigned char *p, size_t n) {
    if (!s->authenticated || !p || n == 0 || n > 512) return 0;
    if (br_sslio_write_all(&s->io, p, n) < 0 || br_sslio_flush(&s->io) < 0) {
        krs_tls_clear(s); return 0;
    }
    return 1;
}

int krs_tls_read(krs_tls *s, unsigned char *p, size_t n) {
    int result;
    if (!s->authenticated || !p || n == 0 || n > 512) return -1;
    result = br_sslio_read(&s->io, p, n);
    if (result <= 0) { krs_tls_clear(s); return -1; }
    return result;
}
