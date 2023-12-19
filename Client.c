#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>


void error(char *msg)
{
    perror(msg);
    exit(0);
}

int login(int sockfd, char * buffer){
	int n;
	printf("Welcome to Gupta Bank\n Enter UserName ");
	memset(buffer, 0, 1024);
	fgets(buffer, 1023, stdin);
	n = write(sockfd, buffer, strlen(buffer));
	memset(buffer, 0, 1024);
	n = read(sockfd, buffer, 1023);
	if(strstr(buffer, "Enter Pass") != NULL){
		printf("%s ", buffer);
		memset(buffer, 0, 1024);
		fgets(buffer, 1023, stdin);
		n = write(sockfd, buffer, strlen(buffer));
		memset(buffer, 0, 1024);
		n = read(sockfd, buffer, 1023);
		if(strstr(buffer, "Yes") != NULL){
			memset(buffer, 0, 1024);
			return 1;
		}
		else{
			memset(buffer, 0, 1024);
			printf("Wrong Pass Try Again");
			return 0;
		}
	}
	else{
		printf("%s ", buffer);
		memset(buffer, 0, 1024);
		return 0;
	}	
}

void commandMode(int sockfd, char * buffer){
	int n;
	printf("Enter Op: ");
	memset(buffer, 0, 1024);
	fgets(buffer, 1023, stdin);
	n = write(sockfd, buffer, strlen(buffer));
	memset(buffer, 0, 1023);
	n = read(sockfd, buffer, 1023);
	printf("%s\n", buffer);
	memset(buffer, 0, 1023);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[1024];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
        error("ERROR opening socket");
  
  server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    int t;
    
    int l;
    l = login(sockfd, buffer);
    
    if(l != 0){
    	n = read(sockfd, buffer, 1023);
    	printf("%s", buffer);
    	memset(buffer, 0, 1024);
    	while(SIGINT){
    		commandMode(sockfd, buffer);
    	}
    }
    
    close(sockfd);
    
}
