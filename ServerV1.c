#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h> 
#include<signal.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include<time.h>
#include <arpa/inet.h>
#include<ctype.h>
#include<pthread.h>

typedef struct AccountHolder{
	char userName[20];
	char pass[5];
	int amount;
}AccountHolder;

typedef struct ClientInfo{
	int clientSocket;
	AccountHolder * list;
	int TotalUsers;
	pthread_mutex_t *mutex;
}ClientInfo;

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int userExist(char * buffer, AccountHolder * list, int totalUsers){
	for(int i = 0; i < totalUsers; i++){
		if(strstr(buffer, list[i].userName) != NULL){
			return i + 1;
		}
	}
	return 0;
}

int authenticate(int clientSocket, char * buffer, AccountHolder * list, int totalUsers){
	int n;
	memset(buffer, 0, 1024);
	n = read(clientSocket, buffer, 1023);
	int check;
	check = userExist(buffer, list, totalUsers);
	memset(buffer, 0, 1024);
	if(check != 0){
		int userN = check - 1;
		n = write(clientSocket, "Enter Pass", strlen("Enter Pass"));
		n = read(clientSocket, buffer, 1023);
		char temp[5];
		strcpy(temp, buffer);
		memset(buffer, 0, 1024);
		temp[4] = '\0';
		if(strcmp(temp, list[userN].pass) == 0){
			n = write(clientSocket, "Yes", strlen("Yes"));	
			return userN + 1;
		}
		else{	
			n = write(clientSocket, "No", strlen("No"));
			return 0;
		}
	}
	else{
		n = write(clientSocket, "No User Found", strlen("No User Found"));
		return 0;
	}
	
}

int welcome(int clientSocket, char * buffer, int UserN, AccountHolder * list){
	int n;
	fprintf(stderr, "Connected to %s\n", list[UserN-1].userName);
	memset(buffer, 0, 1024);
	strcpy(buffer, "Welcome ");
	strcat(buffer, list[UserN-1].userName);
	strcat(buffer, "\n");
//	sprintf(buffer, "Welcome %s\n", list[UserN-1].userName);
	n = write(clientSocket, buffer, 1023);
	memset(buffer, 0, 1024);
	return 0;
}
int extractNumber(const char *str) {
    int result = 0;
    int sign = 1;  // To handle negative numbers

    // Skip leading whitespace
    while (isspace(*str)) {
        str++;
    }

    // Check for a possible sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Process digits
    while (isdigit(*str)) {
        result = result * 10 + (*str - '0');
        str++;
    }

    // Apply the sign
    return sign * result;
}

int commandMode(int clientSocket, char * buffer, int UserN, AccountHolder * list, int totalUsers, pthread_mutex_t *mutex){
	int n;
	int t;
	n = read(clientSocket, buffer, 1023);
	if(strstr(buffer, "Status")){
		memset(buffer, 0, 1024);
		strcpy(buffer, "Total Amount is ");
		char tempS[10];
		sprintf(tempS, "%d", list[UserN - 1].amount);
		strcat(buffer, tempS);
		n = write(clientSocket, buffer, strlen(buffer));
		memset(buffer, 0, 1023);
		return 1;
	}
	
	else if(strstr(buffer, "T") != NULL){
		pthread_mutex_lock(mutex);
		int nUser;
		int n;
		nUser = userExist(buffer, list, totalUsers);
		if((nUser != 0) && (nUser != UserN)){
			int a;
			//a = extractNumber(buffer);
			a = atoi(buffer + 4);
			//fprintf(stderr, "%d", a);
			if(a < list[UserN - 1].amount){
				list[UserN - 1].amount = list[UserN - 1].amount - a;
				//fprintf(stderr,"%d", list[UserN - 1].amount);
				list[nUser - 1].amount = list[nUser - 1].amount + a;
				n = write(clientSocket, "Transferred", strlen("Transferred"));
			}
			else{
				n = write(clientSocket, "Insufficient Balance", strlen("Insufficient Balance")); 
			}
		}
		else{
			n = write(clientSocket, "Can't Pay", strlen("Can't Pay"));
		}
		memset(buffer, 0, 1023);
		pthread_mutex_unlock(mutex);
		return 1;
	}
	
	else if(strstr(buffer, "Break")){
		return 0;
	}
}

void * handleConnections(void * inf){
	ClientInfo * info = (ClientInfo *)inf;
	int clientSocket = info->clientSocket;
	AccountHolder * list = info->list;
	int totalUsers = info->TotalUsers;
	pthread_mutex_t *mutex = info->mutex;
	
	int UserN;
	char buffer[1024];
	UserN = authenticate(clientSocket, buffer, list, totalUsers);
	
	if(UserN != 0){
		int t;
		t = welcome(clientSocket, buffer, UserN, list);
		while(1){
		int t;
			t = commandMode(clientSocket, buffer, UserN, list, totalUsers, mutex);
			if(t==0){
				break;
			}
		}
	}
	else{
		close(clientSocket);
		return NULL;
	}

	close(clientSocket);
	free(inf);
	return NULL;
}


int main(int argc, char * argv[]){

	pthread_mutex_t mutex;
        pthread_mutex_init(&mutex, NULL);
        
	AccountHolder list[10] = {{"A", "1234", 5000},{"B", "2341", 5000}};
	int TotalUsers = 2;
	
	int sockfd, newsockfd, portno, clilen, pid;
	struct sockaddr_in serv_addr, cli_addr;
	int n; // for reading and writing
	
	if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
        }
        
        sockfd = socket(AF_INET, SOCK_STREAM, 0);		// Create a socket
        
        if (sockfd < 0) 
	        error("ERROR opening socket");			// Check if socket is created or not
	        
	bzero((char *) &serv_addr, sizeof(serv_addr));
	
	portno = atoi(argv[1]);	
	
		serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);
        
       	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)  // Binding
       		error("ERROR on binding");

        listen(sockfd,5);   					// Server is on, it is listeing for connections
        
        clilen = sizeof(cli_addr);
        while(1){   						// This is done so that Server is always on.
        
	         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); // Accepting the request
	         
	         pthread_t  t;
	         
	         ClientInfo * info = (ClientInfo *)malloc(sizeof(ClientInfo));
	         info->clientSocket = newsockfd;
	         info->list = list;
	         info->TotalUsers = TotalUsers;
	         info->mutex = &mutex;
	         
	         //handleConnections(info);
	         pthread_create(&t, NULL, handleConnections, info);
	}
	         
}





