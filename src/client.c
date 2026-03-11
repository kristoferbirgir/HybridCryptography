#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include "common.h"

/* ======================================================================
 * Counting BIO filter – wraps the socket BIO to count bytes read/written.
 * This lets us measure how much data the TLS handshake exchanges.
 * ====================================================================== */
static long bytes_read_count  = 0;
static long bytes_written_count = 0;

static int counting_write(BIO *b, const char *buf, int len) {
    int ret = BIO_write(BIO_next(b), buf, len);
    if (ret > 0) bytes_written_count += ret;
    return ret;
}

static int counting_read(BIO *b, char *buf, int len) {
    int ret = BIO_read(BIO_next(b), buf, len);
    if (ret > 0) bytes_read_count += ret;
    return ret;
}

static long counting_ctrl(BIO *b, int cmd, long num, void *ptr) {
    if (BIO_next(b) == NULL) return 0;
    return BIO_ctrl(BIO_next(b), cmd, num, ptr);
}

static int counting_new(BIO *b) {
    BIO_set_init(b, 1);
    return 1;
}

static BIO_METHOD *get_counting_method(void) {
    static BIO_METHOD *method = NULL;
    if (!method) {
        method = BIO_meth_new(BIO_TYPE_FILTER | BIO_get_new_index(), "counting");
        BIO_meth_set_write(method, counting_write);
        BIO_meth_set_read(method, counting_read);
        BIO_meth_set_ctrl(method, counting_ctrl);
        BIO_meth_set_create(method, counting_new);
    }
    return method;
}

static void reset_counters(void) {
    bytes_read_count = 0;
    bytes_written_count = 0;
}

/* Retrieve the negotiated key exchange group name from the SSL connection. */
static const char *get_group_name(SSL *ssl) {
    /* SSL_get0_group_name (OpenSSL 3.4+) handles hybrid groups correctly */
    const char *name = SSL_get0_group_name(ssl);
    if (name) return name;
    /* Fallback for older OpenSSL */
    int nid = SSL_get_negotiated_group(ssl);
    if (nid == NID_undef) return "unknown";
    const char *sn = OBJ_nid2sn(nid);
    return sn ? sn : "unknown";
}

/* ======================================================================
 * Run one TLS session: connect, handshake, send/receive, measure.
 * Returns 0 on success, -1 on failure.
 * ====================================================================== */
static int run_one(SSL_CTX *ctx, int run_idx, int csv, int is_hybrid) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return -1; }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    addr.sin_port = htons(SERVER_PORT);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }

    SSL *ssl = SSL_new(ctx);
    BIO *socket_bio = BIO_new_socket(sock, BIO_NOCLOSE);
    BIO *filter_bio = BIO_new(get_counting_method());
    BIO_push(filter_bio, socket_bio);
    SSL_set_bio(ssl, filter_bio, filter_bio);
    SSL_set1_host(ssl, "localhost");

    /* --- Measure TLS handshake --- */
    reset_counters();
    long long t_hs_start = get_time_ns();

    if (SSL_connect(ssl) <= 0) {
        fprintf(stderr, "[Client] TLS handshake failed (run %d)\n", run_idx);
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(sock);
        return -1;
    }

    long long t_hs_end = get_time_ns();
    long hs_bytes_sent = bytes_written_count;
    long hs_bytes_recv = bytes_read_count;

    /* --- Measure data transfer --- */
    char data[DATA_SIZE];
    memset(data, 'A', DATA_SIZE);

    reset_counters();
    long long t_data_start = get_time_ns();

    SSL_write(ssl, data, DATA_SIZE);
    char response[DATA_SIZE];
    int resp_len = SSL_read(ssl, response, sizeof(response));

    long long t_data_end = get_time_ns();

    double hs_ms   = ns_to_ms(t_hs_end - t_hs_start);
    double data_ms = ns_to_ms(t_data_end - t_data_start);
    long   hs_total = hs_bytes_sent + hs_bytes_recv;

    if (csv) {
        /* CSV: run,mode,group,cipher,hs_ms,data_ms,hs_sent,hs_recv,hs_total,app_sent,app_recv */
        printf("%d,%s,%s,%s,%.3f,%.3f,%ld,%ld,%ld,%d,%d\n",
               run_idx,
               is_hybrid ? "hybrid" : "classical",
               get_group_name(ssl),
               SSL_get_cipher(ssl),
               hs_ms, data_ms,
               hs_bytes_sent, hs_bytes_recv, hs_total,
               DATA_SIZE, resp_len > 0 ? resp_len : 0);
    } else {
        printf("--- Run %d ---\n", run_idx);
        printf("  Mode:                   %s\n", is_hybrid ? "hybrid" : "classical");
        printf("  Key exchange group:     %s\n", get_group_name(ssl));
        printf("  Cipher:                 %s\n", SSL_get_cipher(ssl));
        printf("  Handshake time:         %.3f ms\n", hs_ms);
        printf("  Data transfer time:     %.3f ms\n", data_ms);
        printf("  Handshake bytes sent:   %ld\n", hs_bytes_sent);
        printf("  Handshake bytes recv:   %ld\n", hs_bytes_recv);
        printf("  Handshake bytes total:  %ld\n", hs_total);
        printf("  Application data sent:  %d bytes\n", DATA_SIZE);
        printf("  Application data recv:  %d bytes\n", resp_len > 0 ? resp_len : 0);
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    return 0;
}

/* ====================================================================== */

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s <classical|hybrid> [-n runs] [--csv]\n"
        "\n"
        "  classical   Use X25519 key exchange\n"
        "  hybrid      Use X25519MLKEM768 hybrid key exchange\n"
        "  -n runs     Number of connections (default: 1)\n"
        "  --csv       Output results in CSV format\n",
        prog);
}

int main(int argc, char *argv[]) {
    if (argc < 2) { usage(argv[0]); return 1; }

    const char *mode = argv[1];
    if (strcmp(mode, "classical") != 0 && strcmp(mode, "hybrid") != 0) {
        usage(argv[0]);
        return 1;
    }
    int is_hybrid = (strcmp(mode, "hybrid") == 0);
    int runs = DEFAULT_RUNS;
    int csv  = 0;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            runs = atoi(argv[++i]);
            if (runs < 1) runs = 1;
        } else if (strcmp(argv[i], "--csv") == 0) {
            csv = 1;
        }
    }

    /* --- SSL context (reused across all runs) --- */
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) { ERR_print_errors_fp(stderr); return 1; }

    SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
    SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);

    if (!SSL_CTX_load_verify_locations(ctx, CA_FILE, NULL)) {
        fprintf(stderr, "[Client] Failed to load CA certificate\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    if (!SSL_CTX_set1_groups_list(ctx, is_hybrid ? GROUP_HYBRID : GROUP_CLASSICAL)) {
        fprintf(stderr, "[Client] Failed to set groups\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }

    if (csv) {
        printf("run,mode,group,cipher,handshake_ms,transfer_ms,"
               "hs_bytes_sent,hs_bytes_recv,hs_bytes_total,"
               "app_bytes_sent,app_bytes_recv\n");
    } else {
        printf("=== %s TLS Benchmark (%d run%s) ===\n",
               is_hybrid ? "Hybrid" : "Classical", runs, runs > 1 ? "s" : "");
    }

    int failures = 0;
    for (int i = 1; i <= runs; i++) {
        if (run_one(ctx, i, csv, is_hybrid) != 0) failures++;
        /* Small delay between runs so the server can re-listen */
        if (i < runs) usleep(100000);
    }

    if (!csv && failures > 0)
        printf("\n%d/%d runs failed.\n", failures, runs);

    SSL_CTX_free(ctx);
    return failures > 0 ? 1 : 0;
}
