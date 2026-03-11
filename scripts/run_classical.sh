#!/bin/bash
# Run a classical TLS 1.3 test (server + client on loopback)

if [ ! -f certs/server.crt ]; then
    echo "Certificates not found. Run: ./scripts/gen_certs.sh"
    exit 1
fi

RUNS=${1:-1}
CSV_FLAG=${2:-}

echo "=== Classical TLS Test ($RUNS runs) ==="

./bin/server classical -n "$RUNS" &
SERVER_PID=$!
sleep 0.3

./bin/client classical -n "$RUNS" $CSV_FLAG
CLIENT_EXIT=$?

kill $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null
exit $CLIENT_EXIT
