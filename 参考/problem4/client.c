#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];

	portno = 2050;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if ( sockfd < 0){
		printf("ERROR opening socket");
		exit(1);
	}
	server = gethostbyname("127.0.0.1");
	if ( server == NULL )
	{
		printf("ERROR, no such host");
		exit(0);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("ERROR connecting!");
		exit(1);
	}
	printf("Caesar cipher!!!\n");
	while(1){
		bzero(buffer, 256);
		fgets(buffer, 255, stdin);
		n = write(sockfd, buffer, strlen(buffer));
		if (n < 0) 
       		{
        		printf("God knows something wrong happened in writing!\n");
        		exit(1);
       		}
		if (buffer[0] == ':' && buffer[1] == 'q' && strlen(buffer) == 3)
		{
			printf("End your job!\n");
			break;
		}
        	bzero(buffer,256);
    		n = read(sockfd,buffer,255);
    		if (n < 0) {
    			printf("God knows something wrong happened in reading!\n");
			exit(1);
		}
    		else
			printf("%s",buffer);
   	}
	close(sockfd);
   	return 0;
}
