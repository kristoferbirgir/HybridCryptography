#!/bin/bash
# Generate a self-signed EC certificate for the TLS server.
# Uses P-256 for the certificate (authentication).
# The key exchange algorithm (classical vs hybrid) is separate.

set -e
mkdir -p certs

openssl req -x509 -newkey ec -pkeyopt ec_paramgen_curve:prime256v1 \
    -keyout certs/server.key -out certs/server.crt \
    -days 365 -nodes -subj "/CN=localhost"

echo "Certificates generated:"
echo "  certs/server.crt"
echo "  certs/server.key"
