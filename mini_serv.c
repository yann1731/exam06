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
		int server;
		int bytesRead;
		char *message = NULL;
		char *fullMessage = NULL;
		char buffer[4097];
		memset(buffer, 0, 4097);
		struct sockaddr_in addr;
		int port = atoi(argv[1]);
		if (port <= 0)
			fatalError();
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		addr.sin_port = htons(port);
		int clientId = 0;
		client clients[FD_SETSIZE];
		server = socket(AF_INET, SOCK_STREAM, 0);
		if (server == -1)
			fatalError();
		if (bind(server, (struct sockaddr *) &addr, (socklen_t) sizeof(addr)))
			fatalError();
		if (listen(server, 4096))
			fatalError();
		fd_set currentSocket, readySocket;
		FD_ZERO(&currentSocket);
		FD_SET(server, &currentSocket);
		while (1) {
			readySocket = currentSocket;
			if (select(FD_SETSIZE, &readySocket, NULL, NULL, NULL) < 0) {
				fatalError();
			}
			for (int i = 0; i < FD_SETSIZE; ++i) {
				if (FD_ISSET(i, &readySocket)) {
					if (i == server) {
						printf("new client %d is connected", clientId);
						clients[i].clientFd = accept(server, (struct sockaddr *) &addr, sizeof(addr));
						sprintf(buffer, "server: client %d just arrived\n", clientId);
						clients[i].clientId = clientId++;
						FD_SET(clients[i].clientFd, &currentSocket);
						for (int i = 0; i < FD_SETSIZE; i++) {
							send(clients[i].clientFd, buffer, strlen(buffer), 0);
						}
						memset(buffer, 0, 4097);
					}
					else {
						// message = getMessage(i);
						// snprintf(buffer, 4096, "Client %d: ", clients[i])
						// fullMessage = realloc()
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