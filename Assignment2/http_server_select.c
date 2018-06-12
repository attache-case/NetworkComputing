#include <stdio.h> // printf(), sprintf(), perror()
#include <stdlib.h> // atoi(), exit()
#include <sys/types.h>
#include <sys/socket.h> // socket(), bind()
#include <arpa/inet.h> // inet_pton()
#include <string.h> // memset()
#include <unistd.h> // close(), fork()
#include <netdb.h> // getaddrinfo()
#include <sys/select.h>
#include <netinet/in.h>

#define BUFSIZE 2048

int main() {

    int sock0;
    struct sockaddr_in client;
    socklen_t len;
    int sock;
    int yes = 1;
    struct addrinfo *res, hints;
    int n;

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
    
    fd_set fds, readfds;
    
    struct timeval waitval;
    
    waitval.tv_sec = 2;
    waitval.tv_usec = 500;
    
    int i;
    int client[FD_SETSIZE];
    for (i=0; i < FD_SETSIZE; i++) {
        client[i] = -1;
    }
    FD_ZERO(&readfds);
    FD_SET(sock0, &readfds);
    
    for (;;) {
        memcpy(&fds, &readfds, sizeof(fd_set));
        n = select(sock0 + 1, &fds, NULL, NULL, &waitval);
        for (i=0; i < n; i++) {
            if (FD_ISSET(sock0, &fds)) {
                sock = accept(sock0, (struct sockaddr *)&client, &len);
                client[i] = sock;
                char buf[BUFSIZE];
                char inbuf[BUFSIZE];
                
                memset(&buf, 0, sizeof(buf));
                snprintf(buf, sizeof(buf),
                    "HTTP/1.0 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "\r\n"
                    "<font color=red><h1>HELLO</h1></font>\r\n");
                
                memset(&inbuf, 0, sizeof(inbuf));
                recv(client[i], inbuf, sizeof(inbuf), 0);
                printf("%s", inbuf);
                send(client[i], buf, (int)strlen(buf), 0);
                close(client[i]);
            }
        }
    }
    
    close(sock0);
    
    exit(EXIT_SUCCESS);
}