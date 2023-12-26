#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {
	if (argc == 2) {
		int sock;
		struct sockaddr_in addr;
		int port = atoi(argv[1]);
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1)	{
			write(STDERR_FILENO, "Fatal error\n", strlen("Fatal error\n"));
			exit(1);
		}
		if (bind(sock, (struct sockaddr *) &addr, sizeof(addr))) {
			write(STDERR_FILENO, "Fatal error\n", strlen("Fatal error\n"));
			exit(1);
		}

	}
	else {
		write(STDERR_FILENO, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}
	
	return 0;
}