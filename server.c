#include "server.h"

int main() {
    init_openssl();  // Initialize OpenSSL
    SSL_CTX *ctx = create_ssl_context();
    int server_fd = create_server_socket(SERVER_PORT);

    printf("🎵 Music Web Server Running on https://localhost:%d 🎶\n", SERVER_PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) continue;

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_fd);
        handle_request(ssl);

        close(client_fd);
    }

    close(server_fd);
    SSL_CTX_free(ctx);
    cleanup_openssl();
}


int create_server_socket(int port) {
    int server_fd;
    struct sockaddr_in server_addr;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Allow address reuse
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Set socket options failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Bind to port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("Listening failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", port);
    return server_fd;
}
