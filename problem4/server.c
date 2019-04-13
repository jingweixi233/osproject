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
#include <pthread.h>

#define MYPORT  2050
#define QUEUE   20
#define BUFFER_SIZE 256

void *serve(void * sockfd);
int count = 0;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wait_mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    ///定义sockfd
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);
    pthread_t server_thread;
    ///定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(MYPORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ///bind，成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }

    ///listen，成功返回0，出错返回-1
    if(listen(server_sockfd,QUEUE) == -1)
    {
        perror("listen");
        exit(1);
    }

    ///客户端套接字
    
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);

    ///成功返回非负描述字，出错返回-1

    while(1)
    {
        int conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
        if(conn<0)
        {
            perror("connect");
            exit(1);
        }
        else{
            printf("Creating a thread\n");
            pthread_create(&server_thread, NULL, serve, &conn);
        }
    }
    close(server_sockfd);
    return 0;
}



void *serve(void *clientfd){
    
    int i;
    int sockfd = (int)(*((int*)clientfd));
    printf("%d is connected\n", sockfd);
    while(1){
        if(count >= 2){
            pthread_mutex_lock(&wait_mutex);
        }
        else{
            printf("Serve for %d start\n", sockfd);
            pthread_mutex_lock(&count_mutex);
            count++;
            pthread_mutex_unlock(&count_mutex);

            char buffer[BUFFER_SIZE];
            while(1){
                memset(buffer,0,sizeof(buffer));
                int len = recv(sockfd, buffer, sizeof(buffer),0);
                if(strcmp(buffer,":q\n")==0){
                    pthread_mutex_lock(&count_mutex);
                    count--;
                    pthread_mutex_unlock(&count_mutex);
                    pthread_mutex_unlock(&wait_mutex);
                    printf("Serve for %d end\n", sockfd);
                    close(sockfd);
                    pthread_exit(NULL);
                    break;
                }
                
                fputs(buffer, stdout);
                for(i = 0; i < len; i++){
                    if( (buffer[i] >= 'A' && buffer[i] <= 'W') || (buffer[i] >= 'a' && buffer[i] <= 'w'))
                        buffer[i] += 3;
                    if( (buffer[i] >= 'X' && buffer[i] <= 'Z') || (buffer[i] >= 'x' && buffer[i] <= 'z'))
                        buffer[i] -= 23;
                }
                fputs(buffer, stdout);
                send(sockfd, buffer, len, 0);
            }

        }
    }


}

