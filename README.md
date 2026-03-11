# Hybrid Cryptography – TLS Performance Analysis

This repository contains the implementation and report material for the **Hybrid Cryptography project** in the Applied Cryptography course.

The goal of this project is to investigate how **hybrid cryptography** can be used in practice and to measure the **performance impact of hybrid cryptographic algorithms in TLS 1.3 connections**.

The project combines **research, implementation, and benchmarking**.

---

## Background

With the development of **quantum computers**, some classical cryptographic algorithms may become insecure. Algorithms based on problems such as:

- integer factorization (RSA)
- discrete logarithms (Diffie–Hellman, Elliptic Curve cryptography)

could potentially be broken by quantum algorithms such as **Shor's algorithm**.

To address this threat, **post-quantum cryptographic algorithms** have been developed and standardized. Examples include:

### Key Encapsulation Mechanisms (KEM)
- ML-KEM (Kyber)
- HQC

### Digital Signature Schemes
- ML-DSA (Dilithium)
- SLH-DSA (SPHINCS+)

However, post-quantum cryptography is still relatively new and its long-term security is not yet as well studied as classical cryptography.

For this reason, organizations such as **ENISA** recommend using **hybrid cryptography**, where classical and post-quantum algorithms are used **together**.

In a hybrid construction, security is preserved as long as **at least one of the algorithms remains secure**.

---

## Project Objectives

The project consists of three main objectives:

### 1. Investigate Hybrid Cryptography Standards

Study the **ETSI recommendations** on hybrid cryptographic algorithms for:

- digital signatures
- key encapsulation mechanisms

The report will summarize:

- the types of hybrid constructions described in the ETSI document
- the advantages and disadvantages of each approach.

---

### 2. Analyze EU Cryptographic Recommendations

Investigate **ENISA recommendations** regarding which:

- post-quantum algorithms
- hybrid algorithms
- parameter levels

are recommended for use within the European Union.

---

### 3. Implement and Benchmark TLS with Hybrid Cryptography

Using **OpenSSL (version 3.5 or higher)**, we will implement a simple:

- TLS client
- TLS server

that establish an authenticated **TLS 1.3 connection**.

Two configurations will be tested:

1. **Classical TLS**
   - TLS 1.3 using only classical cryptography

2. **Hybrid TLS**
   - TLS 1.3 using both classical and post-quantum key exchange mechanisms

After establishing the TLS connection, the client and server will exchange data.

---

## Benchmark Experiments

The experiments will measure the impact of hybrid cryptography on:

- **Handshake time** – time required to establish the TLS connection
- **Data transfer time** – time required to send data after connection establishment
- **Data transmitted** – total amount of data exchanged during the TLS handshake

Each experiment will be executed **multiple times** to reduce measurement noise.

The benchmark will compare **one classical algorithm combination** and **one hybrid algorithm combination**.

---

## Implementation Overview

The implementation will consist of:

- a **TLS client**
- a **TLS server**

Both programs will run locally using the **Linux loopback interface (127.0.0.1)**.

The programs will use the **OpenSSL library** to establish secure TLS connections.

---

## Repository Structure

```
HybridCryptography/
│
├── README.md
├── ASSIGNMENT.md
│
├── src/
│   ├── client.c
│   ├── server.c
│   └── common.h
│
├── scripts/
│   ├── build.sh
│   ├── run_classical.sh
│   ├── run_hybrid.sh
│   └── benchmark.sh
│
├── report/
│   └── report.tex
│
└── results/
    ├── classical/
    └── hybrid/
```

---

## Development Plan

The project will be implemented in the following stages:

1. Review ETSI and ENISA recommendations.
2. Implement a basic TLS client/server using OpenSSL.
3. Establish a TLS 1.3 connection using classical cryptography.
4. Extend the implementation to support hybrid key exchange.
5. Implement benchmarking scripts to measure performance.
6. Run experiments and collect results.
7. Document findings in the project report.

---

## Requirements

The code is intended to run on **Ubuntu 24.04.2 LTS**.

Required dependencies will include:

- OpenSSL 3.5+
- GCC
- standard Linux networking libraries

Detailed compilation instructions will be provided as the implementation progresses.

---

## AI Usage

Artificial intelligence tools may be used during the development of this project to support learning and experimentation.

Any use of AI for generating code or report content will be clearly documented in accordance with the course guidelines.