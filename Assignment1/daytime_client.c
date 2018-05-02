#include <stdio.h> // printf(), sprintf(), perror()
#include <stdlib.h> // atoi(), exit()
#include <sys/socket.h> // socket(), bind()
#include <arpa/inet.h> // inet_pton()
#include <string.h> // memset()
#include <unistd.h> // close()
#include <netdb.h> // getaddrinfo()

#define BUFSIZE 1024

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "USAGE: %s string(IP address)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct addrinfo hints;
    struct addrinfo *servAddrInfo;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;

    char* nodename = argv[1]; // node name e.g., IPv4/IPv6 IP address
    char* servname = "13"; // service name e.g., port number

    int rc = getaddrinfo(nodename, servname, &hints, &servAddrInfo);

    if (rc != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    int sock = socket(servAddrInfo->ai_family, servAddrInfo->ai_socktype, servAddrInfo->ai_protocol);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int connect_success = connect(sock, servAddrInfo->ai_addr, servAddrInfo->ai_addrlen);
    if (connect_success != 0) {
        perror("connect");
        close(sock);
        exit(EXIT_FAILURE);
    }

    char buf[BUFSIZE];

    memset(buf, 0, sizeof(buf));
    int n = read(sock, buf, sizeof(buf));
    if (n < 0) {
        perror("read");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("%s\n", buf);

    close(sock);

//    if (argc != 3) {
//        fprintf(stderr, "USAGE: %s {i4 | i6 | <num>} string\n", argv[0]);
//        exit(EXIT_FAILURE);
//    }
//
//    int domain;
//
//    domain = (strcmp(argv[1], "i4") == 0) ? AF_INET :
//             (strcmp(argv[1], "i6") == 0) ? AF_INET6 : atoi(argv[1]);
//
//    char* deststr = argv[2];
//    unsigned char dest_addr[sizeof(struct in6_addr)];
//
//    int translate_addr_success = inet_pton(domain, argv[2], dest_addr);
//    if (translate_addr_success <= 0) {
//        if (translate_addr_success == 0)
//            fprintf(stderr, "Not in presentation format\n");
//        else
//            perror("inet_pton");
//        exit(EXIT_FAILURE);
//    }
    exit(EXIT_SUCCESS);
}