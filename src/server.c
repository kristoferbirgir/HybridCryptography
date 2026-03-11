#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "common.h"

/* Handle one TLS client: handshake, read, respond, shutdown. */
static int handle_client(SSL_CTX *ctx, int client_fd, int run_idx, const char *mode) {
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_fd);

    long long t_hs_start = get_time_ns();

    if (SSL_accept(ssl) <= 0) {
        fprintf(stderr, "[Server] TLS handshake failed (run %d)\n", run_idx);
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        return -1;
    }

    long long t_hs_end = get_time_ns();

    printf("[Server] Run %d – handshake %.3f ms – cipher: %s\n",
           run_idx, ns_to_ms(t_hs_end - t_hs_start), SSL_get_cipher(ssl));

    /* Read client data */
    char buf[DATA_SIZE];
    int bytes = SSL_read(ssl, buf, sizeof(buf));
    if (bytes > 0)
        printf("[Server] Run %d – received %d bytes\n", run_idx, bytes);

    /* Send response */
    const char *response = "Server response OK";
    SSL_write(ssl, response, strlen(response));

    SSL_shutdown(ssl);
    SSL_free(ssl);
    return 0;
}

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s <classical|hybrid> [-n runs]\n"
        "\n"
        "  classical   Use X25519 key exchange\n"
        "  hybrid      Use X25519MLKEM768 hybrid key exchange\n"
        "  -n runs     Number of connections to accept (default: 1)\n",
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

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            runs = atoi(argv[++i]);
            if (runs < 1) runs = 1;
        }
    }

    /* --- SSL context --- */
    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) { ERR_print_errors_fp(stderr); return 1; }

    SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
    SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);

    if (SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM) != 1 ||
        SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) != 1) {
        fprintf(stderr, "[Server] Failed to load certificate/key\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }

    if (!SSL_CTX_set1_groups_list(ctx, is_hybrid ? GROUP_HYBRID : GROUP_CLASSICAL)) {
        fprintf(stderr, "[Server] Failed to set groups\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }

    /* --- TCP socket --- */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); SSL_CTX_free(ctx); return 1; }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    addr.sin_port = htons(SERVER_PORT);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        SSL_CTX_free(ctx);
        return 1;
    }
    listen(sock, 1);
    printf("[Server] Listening on %s:%d (%s mode, %d run%s)\n",
           SERVER_ADDR, SERVER_PORT, mode, runs, runs > 1 ? "s" : "");

    /* --- Accept connections --- */
    for (int i = 1; i <= runs; i++) {
        int client_fd = accept(sock, NULL, NULL);
        if (client_fd < 0) { perror("accept"); continue; }
        handle_client(ctx, client_fd, i, mode);
        close(client_fd);
    }

    close(sock);
    SSL_CTX_free(ctx);
    return 0;
}
