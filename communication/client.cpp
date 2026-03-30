#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3000"
#define BUF_SIZE 99

int main(int argc, char* argv[])
{
    int status;

    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
    {
        perror("getaddrinfo");
        return -1;
    }

    int sfd;

    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        printf("Server socket = %d\n", sfd);

        if ((connect(sfd, p->ai_addr, p->ai_addrlen)) == -1)
        {
            perror("client: connect");
            close(sfd);
            continue;
        }
        
        break;
    }

    if (p == NULL) 
    {
        perror("client: failed to connect");
        return -1;
    }

    freeaddrinfo(servinfo);
    
    char mesg[BUF_SIZE];
    char buf[BUF_SIZE];
    int numbytes;

    while (true)
    {
        fgets(mesg, sizeof(mesg), stdin);

        if (send(sfd, mesg, strlen(mesg), 0) == -1)
        {
            perror("client send");
            close(sfd);
            return -1;
        }

        if ((numbytes = recv(sfd, buf, BUF_SIZE - 1, 0)) <= 0)
        {
             perror("client recv");
             close(sfd);
             return -1;
        }

        buf[numbytes] = '\0';

        printf("%s\n", buf);
    }

    close(sfd);
    return -1;
}

