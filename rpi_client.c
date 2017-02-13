#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LEN 500
#define N_ARGS 3
#define FAILURE -1
#define DELAY 50000

int main(int argc, char* argv[]){
	wiringPiSetup();
	pinMode(0, INPUT);
	
	int wasHigh = 1;
	int door_count = 0;
	int loop_count = 0;
	int r = rand();
	
	if(argc != N_ARGS){
		printf("Usage:\n \t%s <host> <port>\n", argv[0]);
		return(FAILURE);
	}

	char address[sizeof argv[1]]; 
	char port[sizeof argv[2]];

	strcpy(address, argv[1]);
	strcpy(port, argv[2]);

	struct addrinfo details, *results;
	
	memset(&details, 0, sizeof details);
	details.ai_family = AF_UNSPEC;
	details.ai_socktype = SOCK_STREAM;
	details.ai_flags = AI_PASSIVE;

	int status = 0;
	if(status = getaddrinfo(argv[1], port, &details, &results) != 0){
		printf("getaddrinfo produced error\n");
		printf("Reason: %s\n", gai_strerror(status));
		printf("Port: %s, Address: %s\n", port, address);
		return(FAILURE);
	}
	
	int sd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
	if(sd == -1){
		printf("Failure to get socket\n");
		return(FAILURE);
	}

	if(connect(sd, results->ai_addr, results->ai_addrlen) != 0){
		printf("Could not connect\n");
		return(FAILURE);
	}

	printf("Connected\n");

	char buf[BUFFER_LEN];

	int time = 1;
	int curr = 1;

	int previous_value = 0;
	while(1){
		//I wish this could block, but it can't. 
		//The gpio don't have any interrupt mechanism
		//Even if I wrote a K-module to block for it, the kernal would just do the same thing.
		usleep(DELAY);
		previous_value = curr;
		curr = digitalRead(0);
		if(curr != previous_value && curr == 0){
			send(sd, buf, 1, 0);	
			printf("Sent data\n");	
		}
	}

	return(0);

}
