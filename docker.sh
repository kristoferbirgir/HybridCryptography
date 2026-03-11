#!/bin/bash
# Build and run the Docker development environment.
# Your project folder is mounted at /project inside the container,
# so edits on macOS are immediately visible inside Docker.

IMAGE_NAME="hybrid-crypto"

case "${1:-shell}" in
    build)
        echo "Building Docker image (this takes a few minutes the first time)..."
        docker build -t "$IMAGE_NAME" .
        ;;
    shell)
        echo "Starting Ubuntu shell with project mounted..."
        docker run --rm -it -v "$(pwd)":/project "$IMAGE_NAME" bash
        ;;
    gen-certs)
        docker run --rm -v "$(pwd)":/project "$IMAGE_NAME" ./scripts/gen_certs.sh
        ;;
    make)
        docker run --rm -v "$(pwd)":/project "$IMAGE_NAME" make
        ;;
    test-classical)
        docker run --rm -v "$(pwd)":/project "$IMAGE_NAME" ./scripts/run_classical.sh
        ;;
    test-hybrid)
        docker run --rm -v "$(pwd)":/project "$IMAGE_NAME" ./scripts/run_hybrid.sh
        ;;
    benchmark)
        docker run --rm -v "$(pwd)":/project "$IMAGE_NAME" ./scripts/benchmark.sh "${2:-10}"
        ;;
    clean)
        docker run --rm -v "$(pwd)":/project "$IMAGE_NAME" make clean
        ;;
    *)
        echo "Usage: $0 {build|gen-certs|shell|make|test-classical|test-hybrid|benchmark|clean}"
        echo ""
        echo "  build            Build the Docker image (Ubuntu 24.04 + OpenSSL 3.5)"
        echo "  gen-certs        Generate TLS test certificates"
        echo "  shell            Open a bash shell inside the container"
        echo "  make             Compile client and server"
        echo "  test-classical   Run classical TLS test"
        echo "  test-hybrid      Run hybrid TLS test"
        echo "  benchmark [N]    Run benchmark (default: 10 runs)"
        echo "  clean            Remove compiled binaries"
        exit 1
        ;;
esac
