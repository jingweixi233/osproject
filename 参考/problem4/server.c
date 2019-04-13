#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
void *serve(void * newsockfd);
int size_server = 0;
pthread_mutex_t serve_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wait_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]){
	int sockfd, newsockfd, portno, clilen, n;
	pthread_t serve_thread;
	struct sockaddr_in serv_addr, cli_addr;
	char buffer[256];
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if ( sockfd < 0){
		printf("ERROR opening socket");
		exit(1);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	//assign port number(>2000 generally)
	portno = 2050;
	serv_addr.sin_port = htons(portno);
	//the ip address of server
	serv_addr.sin_addr.s_addr = INADDR_ANY;
 	n = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(n < 0)
	{
		printf("God knows something wrong happened on binding!\n");
		exit(1);
	}
	else
		printf("Binding successfully!\n");
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	printf("Server initiating...\n");
	while(1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen); 
		if(newsockfd < 0)
		{
			printf("God knows something wrong happened on acceptting!\n");
		}
		if(pthread_create(&serve_thread, NULL, serve, &newsockfd) != 0)
			printf("God knows something wrong happened on creating!\n");
		else
			printf("Client %d connected!\n", newsockfd);
					
	}

	close(newsockfd);
	close(sockfd);
	return 0;
}

void solve1(char buffer[], int lenth)
{
	int i = -1;
	while((i++)<lenth)
	{
		if( (buffer[i] >= 'A' && buffer[i] <= 'W') || (buffer[i] >= 'a' && buffer[i] <= 'w'))
			buffer[i] += 3;
		if( (buffer[i] >= 'X' && buffer[i] <= 'Z') || (buffer[i] >= 'x' && buffer[i] <= 'z'))
			buffer[i] -= 23;
	}
}				
void solve2(char buffer[])
{
	int i;
	char waitstr[13] = "Please wait!";
	for(i = 0; i< 13;++i)
		buffer[i] = waitstr[i];
}
void *serve(void *sockfd){
	int newsockfd = (int)(*((int*)sockfd));
	//int n;
	char buffer[256];
	while (1)
	{
		if(size_server <2)
		{
			int n;
			pthread_mutex_lock(&serve_mutex);
			size_server++;
			pthread_mutex_unlock(&serve_mutex);
			while(1){
				bzero(buffer, 256);
				n = read(newsockfd, buffer, 255);
				if (n < 0)
				{
					printf("God knows something wrong happened on reading \n");
					close(newsockfd);
					pthread_exit(NULL);
					break;
				}
				if (buffer[0] == ':' && buffer[1] == 'q' && strlen(buffer) == 3)
				{
					printf("Client %d finish its job.\n", newsockfd);
					pthread_mutex_lock(&serve_mutex);
					size_server--;
					pthread_mutex_unlock(&serve_mutex);
					close(newsockfd);
					pthread_exit(NULL);
					break;
				}
				else
					printf("Get information from client %d\n", newsockfd);
				solve1(buffer, n - 1);
				n = write(newsockfd, buffer, n);
				if (n < 0){
					printf("God knows something wrong happened on writing!\n");
					break;
				}
				else
					printf("Send the solutions to %d!\n", newsockfd);
			}
		}
		if(size_server == 2)
		{
			int n;
			while(size_server == 2){
				bzero(buffer, 256);
				n = read(newsockfd, buffer, 255);
				pthread_mutex_lock(&wait_mutex);
				if(size_server != 2)
				{
					pthread_mutex_lock(&serve_mutex);
					size_server++;
					pthread_mutex_unlock(&serve_mutex);
					
					pthread_mutex_unlock(&wait_mutex);
					while(1)
					{
						if (n < 0)
						{
							printf("God knows something wrong happened on reading \n");
							close(newsockfd);
							pthread_exit(NULL);
							break;
						}

						if (buffer[0] == ':' && buffer[1] == 'q' && strlen(buffer) == 3)
						{
							printf("Client %d finish its job.\n", newsockfd);
							pthread_mutex_lock(&serve_mutex);
							size_server--;
							pthread_mutex_unlock(&serve_mutex);
							close(newsockfd);
							pthread_exit(NULL);
							break;
						}
						else
							printf("Get information from client %d\n", newsockfd);

						solve1(buffer, n - 1);
						n = write(newsockfd, buffer, n);
						if (n < 0)
						{
							printf("God knows something wrong happened on writing!\n");
							pthread_exit(NULL);
							break;
						}
						else
							printf("Send the solutions to client %d!\n", newsockfd);
						bzero(buffer, 256);
						n = read(newsockfd, buffer, 255);
					}
				}
				else
				{
					pthread_mutex_unlock(&wait_mutex);
					if(buffer[0] == ':' && buffer[1] == 'q' && strlen(buffer) == 3)
					{
						printf("Client %d finish its job.\n", newsockfd);
						close(newsockfd);
						pthread_exit(NULL);
						break;
					}
					else
					{
						solve2(buffer);
						n = write(newsockfd, buffer, 255);
					}
				}
			}		
		}
	}
}
