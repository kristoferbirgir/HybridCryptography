#!/bin/bash
# Benchmark classical vs hybrid TLS 1.3.
# Runs N iterations of each configuration inside a single server/client process
# pair, outputs CSV results, and computes summary statistics.

set -e

RUNS=${1:-10}
RESULTS_DIR=results
CLASSICAL_CSV="$RESULTS_DIR/classical.csv"
HYBRID_CSV="$RESULTS_DIR/hybrid.csv"

mkdir -p "$RESULTS_DIR"

echo "=== Benchmark: $RUNS runs per configuration ==="
echo ""

# --- Classical ---
echo "--- Classical ($RUNS runs) ---"
./bin/server classical -n "$RUNS" 2>/dev/null &
SERVER_PID=$!
sleep 0.3
./bin/client classical -n "$RUNS" --csv > "$CLASSICAL_CSV"
kill $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null
echo "Saved: $CLASSICAL_CSV"

sleep 0.3

# --- Hybrid ---
echo "--- Hybrid ($RUNS runs) ---"
./bin/server hybrid -n "$RUNS" 2>/dev/null &
SERVER_PID=$!
sleep 0.3
./bin/client hybrid -n "$RUNS" --csv > "$HYBRID_CSV"
kill $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null
echo "Saved: $HYBRID_CSV"

echo ""

# --- Summary statistics ---
echo "=== Summary ==="
echo ""

for CONFIG in classical hybrid; do
    CSV="$RESULTS_DIR/$CONFIG.csv"
    echo "--- $CONFIG ---"
    awk -F',' '
    NR > 1 {
        hs[NR-1]   = $5
        dt[NR-1]   = $6
        hb[NR-1]   = $9
        n++
        sum_hs += $5
        sum_dt += $6
        sum_hb += $9
    }
    END {
        mean_hs = sum_hs / n
        mean_dt = sum_dt / n
        mean_hb = sum_hb / n

        for (i = 1; i <= n; i++) {
            d = hs[i] - mean_hs; var_hs += d*d
            d = dt[i] - mean_dt; var_dt += d*d
        }
        sd_hs = sqrt(var_hs / n)
        sd_dt = sqrt(var_dt / n)

        printf "  Runs:                   %d\n", n
        printf "  Handshake time:         %.3f ms  (sd: %.3f ms)\n", mean_hs, sd_hs
        printf "  Data transfer time:     %.3f ms  (sd: %.3f ms)\n", mean_dt, sd_dt
        printf "  Handshake bytes (avg):  %.0f\n", mean_hb
        print ""
    }
    ' "$CSV"
done

echo "Done. CSV files in $RESULTS_DIR/"
