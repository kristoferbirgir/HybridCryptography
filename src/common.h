#ifndef COMMON_H
#define COMMON_H

#include <time.h>

#define SERVER_ADDR  "127.0.0.1"
#define SERVER_PORT  4433
#define CERT_FILE    "certs/server.crt"
#define KEY_FILE     "certs/server.key"
#define CA_FILE      "certs/server.crt"  /* self-signed: cert is its own CA */
#define DATA_SIZE    4096
#define DEFAULT_RUNS 1

/* Group names for TLS 1.3 key exchange */
#define GROUP_CLASSICAL "X25519"
#define GROUP_HYBRID    "X25519MLKEM768"

static inline long long get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static inline double ns_to_ms(long long ns) {
    return (double)ns / 1000000.0;
}

#endif
