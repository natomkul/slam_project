#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include "RecvParser.h"

#define BACKLOG 10
#define PORT "3000"
#define BUF_SIZE 100


void hexdump(const void *data, size_t size) {
    const unsigned char *buf = (const unsigned char *)data;

    for (size_t i = 0; i < size; i += 16) {
        // Print offset
        printf("%08zx  ", i);

        // Print hex bytes
        for (size_t j = 0; j < 16; j++) {
            if (i + j < size)
                printf("%02x ", buf[i + j]);
            else
                printf("   ");
        }

        printf(" ");

        // Print ASCII representation
        for (size_t j = 0; j < 16 && i + j < size; j++) {
            unsigned char c = buf[i + j];
            printf("%c", isprint(c) ? c : '.');
        }

        printf("\n");
    }
}

int main()
{
    int status;
    struct addrinfo hints,*servinfo, *p;

    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        perror("getaddrinfo");
        return -1;
    }

    int sfd;
    int yes = 1;

    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("socket");
            continue;
        }

        printf("Server socket = %d\n", sfd); 

        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            return -1;
        }

        if (bind(sfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sfd);
            perror("server: bind");
            continue;
        }

        break;
    }
    
    freeaddrinfo(servinfo);

    if (p == NULL)
    {
        perror("server: failed to bind");
        return -1;
    }

    if (listen(sfd, BACKLOG) == -1)
    {
        perror("listen");
        return -1;
    }

    printf("server: waiting to connect...\n");

    struct sockaddr_in cAddr;
    socklen_t sin_size = sizeof cAddr;

    int cfd = accept(sfd,(struct sockaddr *)&cAddr, &sin_size);

    if (cfd == -1)
    {
        perror("accept");
        return -1;
    }

    printf("Accepted socket fd = %d\n", cfd);

    PayloadRecv(cfd, sfd);

    close(cfd);
    close(sfd);

    return 0;
}
