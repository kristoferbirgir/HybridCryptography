CC = gcc
CFLAGS = -Wall -O2

# OpenSSL 3.5+ built from source installs here by default.
# Adjust OPENSSL_DIR if you installed it elsewhere.
OPENSSL_DIR = /usr/local
CFLAGS += -I$(OPENSSL_DIR)/include
LDFLAGS = -L$(OPENSSL_DIR)/lib -Wl,-rpath,$(OPENSSL_DIR)/lib
LIBS = -lssl -lcrypto

SRC_DIR = src
BIN_DIR = bin

CLIENT = $(BIN_DIR)/client
SERVER = $(BIN_DIR)/server

all: $(BIN_DIR) $(CLIENT) $(SERVER)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(CLIENT): $(SRC_DIR)/client.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

$(SERVER): $(SRC_DIR)/server.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean
