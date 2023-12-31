#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

void fatalError() {
	write(STDERR_FILENO, "Fatal error\n", strlen("Fatal error\n"));
	exit(1);
}

typedef struct s_client {
	int clientId;
	int clientFd;
} client;

char *getMessage(int fd) {
	char *message = NULL;
	char buffer[4097];
	memset(buffer, 0, 4097);
	int bytesRead = 0;
	do {
		message = realloc(message, bytesRead + 1);
		if (!message)
			fatalError();
		message = strcat(message, buffer);
		message[bytesRead] = 0;
	}
	while (bytesRead = recv(fd, buffer, 4096, 0));
	
	return message;
}

int main(int argc, char *argv[]) {
	if (argc == 2) {
		printf("Server starting\n");
		int server;
		char *message = NULL;
		char *fullMessage = NULL;
		char buffer[4097];
		int clientList[FD_SETSIZE];
		int currentClients = 0;
		memset(buffer, 0, 4097);
		socklen_t len;
		struct sockaddr_in addr;
		int port = atoi(argv[1]);
		if (port <= 0)
			fatalError();
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		addr.sin_port = htons(port);
		int clientId = 0;
		client clients[FD_SETSIZE];
		memset(clients, 0, sizeof(clients));
		server = socket(AF_INET, SOCK_STREAM, 0);
		if (server == -1) {
			dprintf(2, "Call to socket failed\n");
			fatalError();
		}
		if (bind(server, (struct sockaddr *) &addr, (socklen_t) sizeof(addr))) {
			dprintf(2, "Call to bind failed\n");
			fatalError();
		}
		if (listen(server, 4096)) {
			dprintf(2, "Call to listen failed\n");
			fatalError();
		}
		fd_set currentSocket, readySocket;
		FD_ZERO(&currentSocket);
		FD_SET(server, &currentSocket);
		while (1) {
			readySocket = currentSocket;
			if (select(FD_SETSIZE, &readySocket, NULL, NULL, NULL) < 0) {
				fatalError();
			}
			if (FD_ISSET(server, &readySocket)) { //handle client connection
				int client = accept(server, (struct sockaddr *) &addr, &len);
				clients[client].clientFd = client;
				clients[client].clientId = clientId;
				clientList[clientId] = client;
				FD_SET(client, &currentSocket);
				sprintf(buffer, "Client %d just arrived\n", clientId);
				currentClients++;
				clientId++; 
				for (int i = 0; i < currentClients; i++) {
					send(clientList[i], buffer, strlen(buffer), 0);
				}
				memset(buffer, 0, 4097);
			}
			else { //handle possible client messages
				for (int i = 0; i < currentClients; i++) {
					if (FD_ISSET(clientList[i], &readySocket)) {
						message = getMessage(clientList[i]);
					}
				}
			}
		}
	}
	else {
		write(STDERR_FILENO, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}
}