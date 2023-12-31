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
		printf("Server starting\n");
		int server;
		char bufferOne[4097];
		memset(bufferOne, 0, 4097);
		char bufferTwo[4097];
		memset(bufferTwo, 0, 4097);
		int clientList[FD_SETSIZE];
		int currentClients = 0;
		struct sockaddr_in addr;
		socklen_t len;
		int port = atoi(argv[1]);
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		addr.sin_port = htons(port);
		int clientId = 0;
		client clients[FD_SETSIZE];
		memset(clients, 0, sizeof(clients));
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
			int valueFromSelect = 0;
			if ((valueFromSelect = select(FD_SETSIZE, &readySocket, NULL, NULL, NULL)) < 0) {
				fatalError();
			}
			if (FD_ISSET(server, &readySocket)) { //handle client connection
				int client = accept(server, (struct sockaddr *) &addr, &len);
				clients[client].clientFd = client;
				clients[client].clientId = clientId;
				clientList[clientId] = client;
				FD_SET(client, &currentSocket);
				sprintf(bufferOne, "Client %d just arrived\n", clientId);
				currentClients++;
				clientId++; 
				for (int i = 0; i < currentClients; i++) {
					send(clientList[i], bufferOne, strlen(bufferOne), 0);
				}
				memset(bufferOne, 0, 4097);
			}
			else { //handle possible client messages
				for (int i = 0; i < currentClients; i++) {
					if (FD_ISSET(clientList[i], &readySocket)) {
						int bytesRead = recv(clientList[i], bufferTwo, 4096, 0);
						if (bytesRead > 0) { //handle message
							sprintf(bufferOne, "Client %d: ", clients[clientList[i]].clientId);
							strcat(bufferOne, bufferTwo);
							for (int j = 0; j < currentClients; j++) {
								if (j != i) {
									send(clientList[j], bufferOne, strlen(bufferOne), 0);
								}
							}
						}
						else { //handle disconnect
							sprintf(bufferOne, "server: client %d just left\n", clients[clientList[i]].clientId);
							FD_CLR(clientList[i], &currentSocket);
							clients[clientList[i]].clientId = 0;
							clients[clientList[i]].clientFd = 0;
							close(clientList[i]);
							clientList[i] = 0;
							for (int j = 0; j < currentClients; j++) {
								if (j != i) {
									send(clientList[j], bufferOne, strlen(bufferOne), 0);
								}
							}
						}
						memset(bufferOne, 0, 4097);
						memset(bufferTwo, 0, 4097);
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