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

int main(int argc, char *argv[]) {
	if (argc == 2) {
		int server;
		int bytesRead;
		char buffer[4097];
		char *message = NULL;
		memset(buffer, '\0', 4097);
		struct sockaddr_in addr;
		int port = atoi(argv[1]);
		int clientId = 0;
		client clients[FD_SETSIZE];
		server = socket(AF_INET, SOCK_STREAM, 0);
		if (server == -1)
			fatalError();
		if (bind(server, (struct sockaddr *) &addr, sizeof(addr)))
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
						clients[clientId].clientFd = accept(server, (struct sockaddr *) &addr, sizeof(addr));
						clients[clientId].clientId = clientId;
						FD_SET(clients[clientId++].clientFd, &currentSocket);
					}
					else {
						do {
							message = malloc(bytesRead * sizeof(char));
						}
						while (bytesRead = recv(i, buffer, 4096, 0));
					}
				}
			}
		}
	}
	else {
		write(STDERR_FILENO, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}
	
	return 0;
}