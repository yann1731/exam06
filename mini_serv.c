#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

void handleError(char *message) {
    write(2, message, strlen(message));
    exit(1);
}

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

typedef struct s_client {
    int clientId;
} t_client;



int main(int argc, char *argv[]) {
    if (argc == 2) {
        int sockfd, connfd;
		socklen_t len;
	    struct sockaddr_in servaddr, cli;
        t_client clients[FD_SETSIZE];
        bzero(clients, sizeof(clients));
        char buffer[4096];
        bzero(buffer, 4096);
        int clientId = 0;

	    // socket create and verification 
	    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	    if (sockfd == -1)
            handleError("Fatal error\n");
	    bzero(&servaddr, sizeof(servaddr)); 

	    // assign IP, PORT 
	    servaddr.sin_family = AF_INET; 
	    servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	    servaddr.sin_port = htons(atoi(argv[1])); 
    
	    // Binding newly created socket to given IP and verification 
	    if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
            handleError("Fatal error\n");
	    if (listen(sockfd, FD_SETSIZE) != 0)
            handleError("Fatal error\n");
        fd_set readySockets, currentSockets;
        FD_ZERO(&currentSockets);
        FD_SET(sockfd, &currentSockets);
        while (1) {
            readySockets = currentSockets;
            int numSock = select(FD_SETSIZE, &readySockets, NULL, NULL, NULL);
            if (numSock < 0)
                handleError("Fatal error\n");
            else if (numSock > 0) {
                if (FD_ISSET(sockfd, &readySockets)) { //new client connecting
	                len = sizeof(cli);
	                connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
                    FD_SET(connfd, &currentSockets);
	                if (connfd < 0)
                        handleError("Fatal error\n");
                    clients[connfd].clientId = clientId;
                    clientId++;
                    sprintf(buffer, "server: client %d just arrived\n", clients[connfd].clientId);
                    for (int i = 0; i < FD_SETSIZE; i++)
                        if (i != sockfd && i != connfd)
                            send(i, buffer, 4095, 0);
                    bzero(buffer, 4095);
                }
                else {
                    for (int i = 0; i < FD_SETSIZE; i++) {
                        if (FD_ISSET(i, &readySockets)) {
                            int bytesRead = recv(i, buffer, 4095, 0);
                            if (bytesRead == 0) { //disconnect client
                                FD_CLR(i, &currentSockets);
                                close(i);
                                sprintf(buffer, "server: client %d has left\n", clients[i].clientId);
                                clients[i].clientId = 0;
                                for (int j = 0; j < FD_SETSIZE; j++)
                                    if (j != sockfd && j != i)
                                        send(j, buffer, 4095, 0);
                                bzero(buffer, sizeof(buffer));
                            }
                            else if (bytesRead == 4095) { //most likely should have more shit to send
								printf("Fuck that shit for now\n");
                            }
                            else { // whole message should be received
                                char *bufferCopy = calloc(sizeof(char), strlen(buffer) + 1);
                                char *message = NULL;
                                char *returnMessage = NULL;
                                strcpy(bufferCopy, buffer);
                                bzero(buffer, 4096);
                                sprintf(buffer, "client %d: ", clients[i].clientId);
                                while (extract_message(&bufferCopy, &message) != 0) {
									if (returnMessage)
                                    	returnMessage = realloc(returnMessage, strlen(buffer) + strlen(returnMessage));
									else
										returnMessage = realloc(returnMessage, strlen(buffer));
									strcat(returnMessage, buffer);
									returnMessage = realloc(returnMessage, strlen(returnMessage) + strlen(message));
									strcat(returnMessage, message);
									free(message);
									message = NULL;
                                }
								free(bufferCopy);
								for (int j = 0; j < FD_SETSIZE; j++)
									if (j != sockfd && j != i)
										send(j, returnMessage, strlen(returnMessage), 0);
								free(returnMessage);
								bzero(buffer, 4096);
                            }
                        }
                    }
                }
            }
            else
                continue ;
        }
    }
    else
        handleError("wrong number of arguments\n");
}