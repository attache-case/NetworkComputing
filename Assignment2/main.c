#include <stdio.h> // printf(), sprintf(), perror()
#include <stdlib.h> // atoi(), exit()
#include <sys/socket.h> // socket(), bind()
#include <arpa/inet.h> // inet_pton()
#include <string.h> // memset()
#include <unistd.h> // close(), fork()
#include <netdb.h> // getaddrinfo()

#define BUFSIZE 2048

int main() {

    int sock0;
    struct sockaddr_in client;
    socklen_t len;
    int sock;
    int yes = 1;
    struct addrinfo *res, hints;
    char buf[BUFSIZE];
    int n;
    char inbuf[BUFSIZE];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    char* nodename = NULL; // node name e.g., IPv4/IPv6 IP address
    char* servname = "31605"; // service name e.g., port number

    int rc = getaddrinfo(nodename, servname, &hints, &res);

    if (rc != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }
    
    sock0 = socket(res->ai_family, res->ai_socktype, 0);
    if (sock0 < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));
    
    if (bind(sock0, res->ai_addr, res->ai_addrlen) != 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    if (listen(sock0, 5) != 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    memset(&buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf),
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<font color=red><h1>HELLO</h1></font>\r\n");
    
    for (;;) {
        sock = accept(sock0, (struct sockaddr *)&client, &len);
        memset(&inbuf, 0, sizeof(inbuf));
        recv(sock, inbuf, sizeof(inbuf), 0);
        printf("%s", inbuf);
        send(sock, buf, (int)strlen(buf), 0);
        close(sock);
    }
    
    close(sock0);
    
    exit(EXIT_SUCCESS);
}