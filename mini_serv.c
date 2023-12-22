#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "sys/select.h"

int main(int argc, char *argv[]) {
	if (argc == 2) {
		int port = atoi(argv[1]);

	}
	else {
		write(STDERR_FILENO, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}
	
	return 0;
}