#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
	if (argc == 2) {
		int sock;
		int port = atoi(argv[1]);
		if (sock = socket(AF_INET, SOCK_STREAM, 0)) {
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