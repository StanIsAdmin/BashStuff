#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT 5555		//port used to accept connections
#define BACKLOG 10 		//maximum connections awaiting


/* called when a connection is accepted */
int childFunction(int client_fd);

int main(void) {
	/* Waits for new connections and creates a new process for each one */
	
	int connect_fd; // socket file descriptor for connection reception
	int client_fd; // socket file descriptor for new connections
	struct sockaddr_in server_address; // server's address
	struct sockaddr_in client_address; // client's address
	socklen_t address_size; // size of the client's address

	// socket(): initializes the socket and returns socket descriptor
	if ((connect_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("server: socket"); // error initializing the socket
		return EXIT_FAILURE;
	}

	// setsockopt(): reuse address if possible
	int yes = 1;
	if (setsockopt(connect_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 		{
		perror("server: setsockopt"); // error while setting options
		return EXIT_FAILURE;
	}

	// initialize server_address (host's address)
	server_address.sin_family = AF_INET;         // address family
	server_address.sin_port = htons(PORT);     // port number
	server_address.sin_addr.s_addr = INADDR_ANY; // local machine address
	memset(&(server_address.sin_zero), '\0', 8); // initialize what's left with zeroes

	// bind(): links socket to port number
	if (bind(connect_fd, (struct sockaddr *)&server_address, sizeof(struct sockaddr)) == -1) {
		perror("server: bind");
		return EXIT_FAILURE;
	}

	// listen(): listens on socket for new machine connections
	if (listen(connect_fd, BACKLOG) == -1) {
		perror("listen");
		return EXIT_FAILURE;
	}

	// infinite accept() loop
	while(1) {
		printf("Server now accepting connections on port %i\n", PORT);
		
		address_size = sizeof(struct sockaddr_in);

		// accept(): accepts one of the connection requests
		if ((client_fd = accept(connect_fd, (struct sockaddr *)&client_address, &address_size)) == -1) {
		    perror("server: accept");
		}

		// fork(): creates a new process that runs code for one client
		if (!fork())
		{
			// this is the child process
		    close(connect_fd); // child process doesn't need the listening socket

		    printf("Client with address %s has joined\n", inet_ntoa(client_address.sin_addr));
            
			int retval = childFunction(client_fd); // main client function

            printf("Client with address %s has left\n", inet_ntoa(client_address.sin_addr));
		    
			close(client_fd); 
		    return retval;
		}
		
		// this is the parent process
		close(client_fd); // no need to keep the client socket
	}
}


int childFunction(int client_fd) {
	/* Interacts with the client */
	
	int numbytes; // bytes effectively received

	// welcome message
	char welcome_msg[] = "Welcome to this amazing server!";
	
	// message asking for input
	char input_msg[] = "Select 1 for staying, 2 for disconnecting.";
	
	// goodbye message
	char goodbye_msg[] = "In your absence you shall be missed *-*";

	// send welcome message
	if (send(client_fd, welcome_msg, sizeof(welcome_msg), 0) == -1) {
		perror("server: send"); //erreur while sending
		return EXIT_FAILURE;
	}

	// waits for confirmation from client (control)
	char status; // client status
	if ((numbytes=recv(client_fd, &status, 1, 0)) == -1) {
		perror("client: recv"); //error while receiving
		return EXIT_FAILURE;
	}
	
	//if the client has disconnected, end the process (no message)
	if(numbytes==0) return EXIT_FAILURE;

	// main client loop, continues while client chooses to stay
	while (status==1) {
		status=2;//avoid infinite loop if client force-quits

		//send message asking for input
		if (send(client_fd, input_msg, sizeof(input_msg), 0) == -1) {
			perror("server: send"); //erreur lors de l'envoi
			return EXIT_FAILURE;
		}

		//wait for client's decision
		if ((numbytes=recv(client_fd, &status, 1, 0)) == -1) {
			perror("client: recv"); //erreur lors de la réception
			return EXIT_FAILURE;
		}
		
		//if client has disconnected, end the process (no message)
		if(numbytes==0) return EXIT_FAILURE;

		//if client decides to end the game, say goodbye and leave
        if (status!=1) {
			if (send(client_fd, goodbye_msg, sizeof(goodbye_msg), 0) == -1) {
				perror("server: send");
				return EXIT_FAILURE;
			}
			return EXIT_SUCCESS; // leave peacefully
        }
	}
	return EXIT_SUCCESS;
}
