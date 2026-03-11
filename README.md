# Hybrid Cryptography – TLS 1.3 Performance Analysis

Benchmarking classical vs. hybrid post-quantum key exchange in TLS 1.3, using OpenSSL 3.5 on Ubuntu 24.04.

---

## 1. Compilation and Installation

### Prerequisites

| Requirement | Version |
|---|---|
| Operating system | Ubuntu 24.04.2 LTS |
| Compiler | GCC (via `build-essential`) |
| OpenSSL | **3.5.0 or higher** (must be built from source) |
| Docker (optional) | For development on macOS/Windows |

### Option A: Docker (recommended for macOS/Windows)

```bash
# Build the Docker image (Ubuntu 24.04 + OpenSSL 3.5)
./docker.sh build

# Generate TLS test certificates
./docker.sh gen-certs

# Compile client and server
./docker.sh make
```

### Option B: Native Ubuntu 24.04

Ubuntu 24.04 ships OpenSSL 3.0.x which lacks hybrid KEM support. Build 3.5 from source:

```bash
sudo apt update && sudo apt install -y build-essential wget zlib1g-dev

cd /tmp
wget https://github.com/openssl/openssl/releases/download/openssl-3.5.0/openssl-3.5.0.tar.gz
tar xzf openssl-3.5.0.tar.gz
cd openssl-3.5.0
./Configure --prefix=/usr/local --libdir=lib --openssldir=/usr/local/ssl
make -j$(nproc)
sudo make install
echo "/usr/local/lib" | sudo tee /etc/ld.so.conf.d/openssl.conf
sudo ldconfig
```

Verify: `/usr/local/bin/openssl version` should show `OpenSSL 3.5.0`.

Then compile:

```bash
make                          # produces bin/client and bin/server
./scripts/gen_certs.sh        # generate test certificates
```

The Makefile expects OpenSSL in `/usr/local`. Edit `OPENSSL_DIR` in the Makefile if you installed elsewhere.

---

## 2. Running the Test Cases

### Single test run

```bash
# Classical TLS 1.3 (X25519 key exchange)
./scripts/run_classical.sh

# Hybrid TLS 1.3 (X25519 + ML-KEM-768 key exchange)
./scripts/run_hybrid.sh
```

With Docker:

```bash
./docker.sh test-classical
./docker.sh test-hybrid
```

### Multiple runs

```bash
# 20 consecutive handshakes in one process (more accurate than separate runs)
./scripts/run_classical.sh 20
./scripts/run_hybrid.sh 20
```

### Full benchmark with statistics

```bash
./scripts/benchmark.sh            # default: 10 runs each
./scripts/benchmark.sh 50         # custom: 50 runs each
```

With Docker: `./docker.sh benchmark 50`

The benchmark outputs CSV files and a summary table with mean and standard deviation.

### Network simulation (optional bonus)

Simulate real network conditions (requires root, Linux only):

```bash
# Add 10ms delay + 100 Mbit/s bandwidth limit on loopback
sudo ./scripts/net_sim.sh add 10 100

# Run benchmark under simulated conditions
./scripts/benchmark.sh 20

# Remove simulation
sudo ./scripts/net_sim.sh remove
```

---

## 3. Interpreting the Output

### Human-readable output (default)

```
--- Run 1 ---
  Mode:                   classical
  Key exchange group:     X25519
  Cipher:                 TLS_AES_256_GCM_SHA384
  Handshake time:         1.374 ms
  Data transfer time:     0.187 ms
  Handshake bytes sent:   297
  Handshake bytes recv:   757
  Handshake bytes total:  1054
  Application data sent:  4096 bytes
  Application data recv:  18 bytes
```

### CSV output (`--csv` flag)

```
run,mode,group,cipher,handshake_ms,transfer_ms,hs_bytes_sent,hs_bytes_recv,hs_bytes_total,app_bytes_sent,app_bytes_recv
1,classical,X25519,TLS_AES_256_GCM_SHA384,1.374,0.187,297,757,1054,4096,18
```

### Benchmark summary

```
--- classical ---
  Runs:                   10
  Handshake time:         1.289 ms  (sd: 0.152 ms)
  Data transfer time:     0.213 ms  (sd: 0.041 ms)
  Handshake bytes (avg):  1053
```

**Key metrics explained:**

| Metric | Description |
|---|---|
| **Handshake time** | Wall-clock time for the TLS 1.3 handshake (`SSL_connect`) measured with `CLOCK_MONOTONIC` |
| **Data transfer time** | Time to send 4096 bytes and receive the server response after the handshake |
| **Handshake bytes** | Raw TCP bytes exchanged during the handshake, measured via a custom BIO filter |
| **Key exchange group** | The negotiated group: `X25519` (classical) or `X25519MLKEM768` (hybrid) |

---

## 4. Algorithm Configurations

| Configuration | Key Exchange | Authentication | TLS Version |
|---|---|---|---|
| Classical | X25519 (ECDHE) | ECDSA P-256 | TLS 1.3 |
| Hybrid | X25519 + ML-KEM-768 | ECDSA P-256 | TLS 1.3 |

Both configurations use the same cipher suite (`TLS_AES_256_GCM_SHA384`) and authentication. The only difference is the key exchange mechanism.

---

## 5. Project Structure

```
HybridCryptography/
├── README.md              # This file
├── ASSIGNMENT.md          # Assignment specification
├── Makefile               # Build configuration
├── Dockerfile             # Ubuntu 24.04 + OpenSSL 3.5 image
├── docker.sh              # Docker helper script
├── .gitignore
│
├── src/
│   ├── common.h           # Shared constants, timing utilities
│   ├── client.c           # TLS client with benchmarking (counting BIO)
│   └── server.c           # TLS server (multi-connection)
│
├── scripts/
│   ├── gen_certs.sh       # Generate self-signed test certificates
│   ├── run_classical.sh   # Run classical TLS test
│   ├── run_hybrid.sh      # Run hybrid TLS test
│   ├── benchmark.sh       # Full benchmark with CSV + statistics
│   └── net_sim.sh         # Optional: tc network simulation
│
├── certs/                 # Generated certificates (not committed)
│
├── report/
│   ├── report.tex         # Project report (LaTeX source)
│   └── report.pdf         # Compiled report
│
└── results/               # Benchmark CSV output
    ├── classical.csv
    └── hybrid.csv
```

---

## 6. AI Usage

AI tools were used during development to support learning and experimentation. All usage is documented per course guidelines.