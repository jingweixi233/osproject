//xi jingwei
//oprating system problem4
//client

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>

#define MYPORT  2050
#define BUFFER_SIZE 256

int main()
{
    int sock_client = socket(AF_INET,SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);  
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    if (connect(sock_client, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }

    char sendbuf[BUFFER_SIZE];
    char recvbuf[BUFFER_SIZE];
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        send(sock_client, sendbuf, strlen(sendbuf),0);    //send a message
        if(strcmp(sendbuf,":q\n")==0)
            break;
        recv(sock_client, recvbuf, sizeof(recvbuf),0);    //receive the message
        fputs(recvbuf, stdout);
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }

    close(sock_client);
    return 0;
}