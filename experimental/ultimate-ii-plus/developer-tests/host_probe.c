/* Windows host integration test of THE SAME portable TLS core.
   No credentials, posts, guest login or device firmware changes. */
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include "krs_tls.h"
#include "rsa_pin.h"

static int rd(void *ctx, unsigned char *p, size_t n) {
    int r = recv(*(SOCKET *)ctx, (char *)p, (int)n, 0); return r > 0 ? r : -1;
}
static int wr(void *ctx, const unsigned char *p, size_t n) {
    int r = send(*(SOCKET *)ctx, (char *)p, (int)n, 0); return r > 0 ? r : -1;
}
int main(int argc, char **argv) {
    WSADATA w; struct addrinfo hints, *addr = NULL; SOCKET sock;
    unsigned char seed[32], modulus[512], exponent[] = {1,0,1}, response[512];
    unsigned timeout = 8000; int n, ok, badpin;
    krs_tls *tls = (krs_tls *)calloc(1, sizeof(krs_tls));
    br_rsa_public_key pin;
    const char request[] = "GET /krs/ HTTP/1.1\r\nHost: quipzz.bplaced.net\r\nConnection: close\r\n\r\n";
    if (argc != 2 || !tls || WSAStartup(MAKEWORD(2,2), &w)) return 2;
    badpin = !strcmp(argv[1], "bad-pin");
    memcpy(modulus, rsa_modulus, 512); if (badpin) modulus[0] ^= 2;
    pin.n = modulus; pin.nlen = sizeof(modulus); pin.e = exponent; pin.elen = sizeof(exponent);
    /* OS-generated test seed arrives on stdin, never logged or packaged. */
    _setmode(_fileno(stdin), _O_BINARY);
    if (fread(seed, 1, 32, stdin) != 32) return 3;
    memset(&hints, 0, sizeof(hints)); hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo("quipzz.bplaced.net", "443", &hints, &addr)) return 4;
    sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
    if (connect(sock, addr->ai_addr, (int)addr->ai_addrlen)) return 5;
    freeaddrinfo(addr);
    ok = krs_tls_open(tls, seed, sizeof(seed), &pin, rd, wr, &sock);
    memset(seed, 0, sizeof(seed));
    printf("authenticated=%d tls_error=%d\n", ok, tls->last_error);
    if (!ok) { closesocket(sock); krs_tls_clear(tls); free(tls); WSACleanup(); return badpin ? 0 : 6; }
    if (badpin) return 7;
    if (!krs_tls_write(tls, (const unsigned char *)request, sizeof(request)-1)) return 8;
    n = krs_tls_read(tls, response, sizeof(response));
    if (n < 12 || memcmp(response, "HTTP/1.1 200", 12)) return 9;
    puts("http_status=200");
    /* Bad lengths never cause network writes; close erases all live keys. */
    if (krs_tls_write(tls, response, 513)) return 10;
    closesocket(sock); krs_tls_clear(tls);
    for (n=0; n<(int)sizeof(*tls); ++n) if (((unsigned char *)tls)[n]) return 11;
    free(tls); WSACleanup(); puts("key_wipe=ok"); return 0;
}
