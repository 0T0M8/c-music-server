#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 8192

void init_openssl();
SSL_CTX *create_ssl_context();
int create_server_socket(int port);
void handle_request(SSL *ssl);
void cleanup_openssl();

#endif
