#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>


static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();

}

static void process_connection(int connfd) {
    char reading_buffer[64] = {};
    ssize_t n = read(connfd, reading_buffer, sizeof(reading_buffer) - 1);
    if (n < 0) {
        msg("read() error");
        return;
    }

    printf("client says: %s\n", reading_buffer);
    char writing_buffer[] = "world!";
    write(connfd, writing_buffer,  strlen(writing_buffer));


}

int main(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = ntohs(1234);
    address.sin_addr.s_addr = ntohl(0);  // wildcard address 0.0.0.0
    int rv = bind(fd, (const sockaddr *) &address, sizeof(address));

    if (rv) {
        die("bind()");
    }

    rv = listen(fd, SOMAXCONN);
    if (rv) {
        die("listen()");
    }

    while (true) {
        // accepting client connections

        struct sockaddr_in client_address = {};
        socklen_t socklen = sizeof(client_address);
        int connfd = accept(fd, (struct sockaddr *) &client_address, &socklen);
        if (connfd < 0) {
            continue;
        }

        process_connection(connfd);
        close(connfd);
    }

    return 0;

}