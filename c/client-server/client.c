#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 5555 // client connects to that port
#define MAXDATASIZE 255	// max. bytes received at once
#define h_addr h_addr_list[0] // used for backward compatibility


int main(int argc, char *argv[])
{
	int socket_fd; // socket file descriptor
	int numbytes; // bytes effectively received
	char buffer[MAXDATASIZE]; // bufferfer for data reception
	struct hostent *server_entity; // host (server) entity
	struct sockaddr_in server_address; // host address

	//argc: argument count
	if (argc != 2) {
		fprintf(stderr,"usage: client hostname\n");
		return EXIT_FAILURE;
	}

	//gethostbyname(name): returns hostent struct
	if ((server_entity = gethostbyname(argv[1])) == NULL) {
		perror("client: gethostbyname"); //error during lookup
		return EXIT_FAILURE;
	}
	
	printf("server %s found\n", argv[1]);

	//socket(): initializes socket and returns socket file descriptor
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("client: socket"); // error while initializing socket
		return EXIT_FAILURE;
	}

	//initialize server_address
	server_address.sin_family = AF_INET; // address family
	server_address.sin_port = htons(PORT); // port number
	server_address.sin_addr = *((struct in_addr*) server_entity->h_addr); // address (converted from h_addr)
	memset(&(server_address.sin_zero), '\0', 8); // initialize what's left with zeroes
	
	printf("connecting...");

	//connect(): establish a connection with host
	if (connect(socket_fd, (struct sockaddr*)&server_address, sizeof(struct sockaddr)) == -1) {
        printf("\n");
		perror("client: connect"); //error during connection
		return EXIT_FAILURE;
	}
    printf("done\n");

	// receive and display the welcome message
	if ((numbytes=recv(socket_fd, &buffer, MAXDATASIZE-1, 0)) == -1) {
		perror("client: recv"); //error during reception (connection lost)
		return EXIT_FAILURE;
	}
	buffer[numbytes] = '\0'; //ignore the rest of the bufferfer
	printf("\n--Message from the server:\n%s\n\n",buffer);

	// confirm reception of message (control)
	signed char status=1;
	if (send(socket_fd, &status, 1, 0) == -1) {
		perror("client: send");
		return EXIT_FAILURE;
	}

	// main loop
	while (status==1) {

		// receive and display game invitation
		if ((numbytes=recv(socket_fd, &buffer, MAXDATASIZE-1, 0)) == -1) {
			perror("client: recv"); //erreur lors de réception
			return EXIT_FAILURE;
		}
		buffer[numbytes] = '\0';
		printf("--Message from the server:\n%s\n",buffer);

		// ask for input
		int select;
		do {
			printf("Your choice: ");
			scanf("%d", &select);
			while(getchar() != '\n') continue;
		} while (select!=1 && select!=2);

		// send the input (status)
		status = (signed char)select; //convert select
		if (send(socket_fd, &status, 1, 0) == -1) {
			perror("client: send");
			return EXIT_FAILURE;
		}

		// otherwise, display game over message
        if ((numbytes = recv(socket_fd, &buffer, MAXDATASIZE-1, 0)) == -1) {
			perror("client: recv");
			return EXIT_FAILURE;
		}
		buffer[numbytes] = '\0';
		printf("\n--Message from the server:\n%s\n",buffer);

		// sonfirm message reception
		if (send(socket_fd, &status, 1, 0) == -1) {
			perror("client: send");
			return EXIT_FAILURE;
		}
	}

	close(socket_fd); //close connection (close socket file descriptor)
	return EXIT_SUCCESS;
}
