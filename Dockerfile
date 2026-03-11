FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    zlib1g-dev \
    perl \
    && rm -rf /var/lib/apt/lists/*

# Build OpenSSL 3.5 from source, install to /usr/local with libdir=lib
WORKDIR /tmp
RUN wget https://github.com/openssl/openssl/releases/download/openssl-3.5.0/openssl-3.5.0.tar.gz \
    && tar xzf openssl-3.5.0.tar.gz \
    && cd openssl-3.5.0 \
    && ./Configure --prefix=/usr/local --libdir=lib --openssldir=/usr/local/ssl \
    && make -j$(nproc) \
    && make install \
    && echo "/usr/local/lib" > /etc/ld.so.conf.d/openssl.conf \
    && ldconfig \
    && cd / && rm -rf /tmp/openssl-3.5.0* \
    && LD_LIBRARY_PATH=/usr/local/lib /usr/local/bin/openssl version

ENV LD_LIBRARY_PATH=/usr/local/lib

# Set working directory to the mounted project
WORKDIR /project
