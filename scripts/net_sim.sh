#!/bin/bash
# Simulate realistic network conditions on the loopback interface using tc.
# Requires root privileges. Only works on Linux (not macOS).
#
# Usage:
#   ./scripts/net_sim.sh add [delay_ms] [bandwidth_mbit]
#   ./scripts/net_sim.sh remove
#
# Examples:
#   ./scripts/net_sim.sh add 10 100     # 10ms delay, 100 Mbit/s
#   ./scripts/net_sim.sh add 50 10      # 50ms delay, 10 Mbit/s (slow link)
#   ./scripts/net_sim.sh remove         # remove all restrictions

set -e

IFACE="lo"

case "${1:-}" in
    add)
        DELAY_MS=${2:-10}
        BW_MBIT=${3:-100}

        echo "Adding network simulation on $IFACE:"
        echo "  Delay:     ${DELAY_MS}ms"
        echo "  Bandwidth: ${BW_MBIT} Mbit/s"

        # Remove existing rules
        tc qdisc del dev "$IFACE" root 2>/dev/null || true

        # Add delay and bandwidth limit
        tc qdisc add dev "$IFACE" root handle 1: tbf \
            rate "${BW_MBIT}mbit" burst 32kbit latency 400ms
        tc qdisc add dev "$IFACE" parent 1:1 handle 10: \
            netem delay "${DELAY_MS}ms"

        echo "Network simulation active."
        ;;

    remove)
        echo "Removing network simulation on $IFACE..."
        tc qdisc del dev "$IFACE" root 2>/dev/null || true
        echo "Network simulation removed."
        ;;

    *)
        echo "Usage: $0 {add [delay_ms] [bw_mbit] | remove}"
        echo ""
        echo "  add [delay_ms] [bw_mbit]   Add network delay and bandwidth limit"
        echo "  remove                      Remove all tc rules"
        echo ""
        echo "Examples:"
        echo "  $0 add 10 100       # 10ms delay, 100 Mbit/s"
        echo "  $0 add 50 10        # 50ms delay, 10 Mbit/s"
        echo "  $0 remove           # remove restrictions"
        exit 1
        ;;
esac
