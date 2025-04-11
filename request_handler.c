#include "server.h"

void serve_static(SSL *ssl, const char *filepath, const char *content_type) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        SSL_write(ssl, "HTTP/1.1 404 Not Found\r\n\r\nFile Not Found", 44);
        return;
    }

    // Send HTTP response header
    char header[256];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", content_type);
    SSL_write(ssl, header, strlen(header));

    // Send file contents
    char buffer[BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        SSL_write(ssl, buffer, bytes);
    }

    fclose(file);
}

void handle_request(SSL *ssl) {
    char buffer[BUFFER_SIZE] = {0};
    int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (bytes <= 0) return;

    printf("📥 Received Request:\n%s\n", buffer);

    // Check request type
    if (strstr(buffer, "GET / ")) {
        serve_static(ssl, "static/index.html", "text/html");
    } else if (strstr(buffer, "GET /styles.css")) {
        serve_static(ssl, "static/styles.css", "text/css");
    } else if (strstr(buffer, "GET /script.js")) {
        serve_static(ssl, "static/script.js", "application/javascript");
    } else {
        SSL_write(ssl, "HTTP/1.1 404 Not Found\r\n\r\nPage Not Found", 42);
    }
}

void handle_upload(SSL *ssl) {
    char buffer[BUFFER_SIZE];
    FILE *file = fopen("uploads/song.mp3", "wb");

    int bytes;
    while ((bytes = SSL_read(ssl, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, 1, bytes, file);
    }

    fclose(file);
    SSL_write(ssl, "HTTP/1.1 200 OK\r\n\r\nUpload Successful!", 41);
}

void handle_download(SSL *ssl, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        SSL_write(ssl, "HTTP/1.1 404 Not Found\r\n\r\nFile Not Found", 44);
        return;
    }

    SSL_write(ssl, "HTTP/1.1 200 OK\r\nContent-Type: audio/mpeg\r\n\r\n", 46);

    char buffer[BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        SSL_write(ssl, buffer, bytes);
    }

    fclose(file);
}

void handle_auth(SSL *ssl) {
    char buffer[BUFFER_SIZE];
    int bytes = SSL_read(ssl, buffer, BUFFER_SIZE - 1);
    buffer[bytes] = '\0';

    if (strstr(buffer, "Authorization: Basic YWRtaW46cGFzc3dvcmQ=")) {
        SSL_write(ssl, "HTTP/1.1 200 OK\r\n\r\nAuthentication Success", 44);
    } else {
        SSL_write(ssl, "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"MusicServer\"\r\n\r\n", 75);
    }
}
