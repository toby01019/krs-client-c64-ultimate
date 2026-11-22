/* Experimental private UCI target 0x0E. Not an upstream allocation.
   Add to the U2+ firmware build; never silently replace a stock target. */
#include "command_target.h"
#include "krs_tls.h"
#include "rsa_pin.h"
#include "socket.h"
#include "netdb.h"
#include <string.h>
#include <new>

class KrsTlsTarget : public CommandTarget {
    krs_tls *tls;
    int fd;
    unsigned char seed[32], data[512];
    bool seeded;
    Message reply, status;
    static int input(void *ctx, unsigned char *p, size_t n) {
        KrsTlsTarget *s = (KrsTlsTarget *)ctx;
        int r = recv(s->fd, p, n, 0);
        return r > 0 ? r : -1;
    }
    static int output(void *ctx, const unsigned char *p, size_t n) {
        KrsTlsTarget *s = (KrsTlsTarget *)ctx;
        int r = send(s->fd, p, n, 0);
        return r > 0 ? r : -1;
    }
    void stop() {
        if (fd >= 0) { close(fd); fd = -1; }
        if (tls) { krs_tls_clear(tls); delete tls; tls = NULL; }
        volatile unsigned char *p = seed;
        for (unsigned i = 0; i < sizeof(seed); ++i) p[i] = 0;
        seeded = false;
        memset(data, 0, sizeof(data));
    }
    bool connect_peer() {
        struct hostent host, *found = NULL;
        struct sockaddr_in addr;
        char dnsbuf[1024]; int error;
        if (gethostbyname_r("quipzz.bplaced.net", &host, dnsbuf,
                            sizeof(dnsbuf), &found, &error) || !found) return false;
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) return false;
        struct timeval timeout;
        timeout.tv_sec = 8; timeout.tv_usec = 0;
        if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) ||
            setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))) return false;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET; addr.sin_port = htons(443);
        memcpy(&addr.sin_addr.s_addr, found->h_addr, found->h_length);
        if (connect(fd, (struct sockaddr *)&addr, sizeof(addr))) return false;
        tls = new(std::nothrow) krs_tls;
        if (!tls) return false;
        static unsigned char exponent[] = {1, 0, 1};
        br_rsa_public_key pin = {(unsigned char *)rsa_modulus, sizeof(rsa_modulus),
                                 exponent, sizeof(exponent)};
        return krs_tls_open(tls, seed, sizeof(seed), &pin, input, output, this) != 0;
    }
    void result(const char *s) {
        status.message = (uint8_t *)s; status.length = strlen(s); status.last_part = true;
    }
public:
    KrsTlsTarget() : tls(NULL), fd(-1), seeded(false) {
        memset(seed, 0, sizeof(seed)); memset(data, 0, sizeof(data));
        command_targets[0x0E] = this;
    }
    ~KrsTlsTarget() { stop(); }
    void parse_command(Message *cmd, Message **r, Message **s) {
        reply.message = data; reply.length = 0; reply.last_part = true;
        *r = &reply; *s = &status; result("400 BAD COMMAND");
        if (cmd->length < 2 || cmd->message[0] != 0x0E) return;
        unsigned char op = cmd->message[1];
        if (op == 1 && cmd->length == 2) {
            memcpy(data, "KRSTLS/1", 8); reply.length = 8; result("000 OK");
        } else if (op == 2 && cmd->length == 34) {
            stop(); memcpy(seed, cmd->message + 2, 32); seeded = true; result("000 OK");
        } else if (op == 0x10 && cmd->length == 2) {
            if (!seeded) { result("403 SEED REQUIRED"); return; }
            bool ok = connect_peer();
            volatile unsigned char *p = seed;
            for (unsigned i = 0; i < sizeof(seed); ++i) p[i] = 0;
            seeded = false;
            if (!ok) { stop(); result("502 TLS FAILED"); }
            else result("000 OK");
        } else if (op == 0x11 && cmd->length > 2 && cmd->length <= 514) {
            if (!tls || !krs_tls_write(tls, cmd->message + 2, cmd->length - 2)) {
                stop(); result("502 WRITE FAILED");
            } else result("000 OK");
        } else if (op == 0x12 && cmd->length == 2) {
            int n = tls ? krs_tls_read(tls, data, sizeof(data)) : -1;
            if (n <= 0) { stop(); result("502 READ FAILED"); }
            else { reply.length = n; result("000 OK"); }
        } else if (op == 0x13 && cmd->length == 2) {
            stop(); memset(data, 0, sizeof(data)); result("000 OK");
        }
    }
    void get_more_data(Message **r, Message **s) {
        reply.length = 0; reply.last_part = true; result("000 OK"); *r = &reply; *s = &status;
    }
    void abort(int) { stop(); memset(data, 0, sizeof(data)); }
    void c64_reset() { abort(0); }
};

/* Static registration matches network_target.cc. Link this object normally,
   not inside a discarded archive. Reserve 0x0E before enabling in a build. */
static KrsTlsTarget krs_tls_target;
