#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main () {
	int pid = fork();

	if (pid != 0) {
		printf("PID pai: %d\n", getpid());
	}
	else {
		printf("PID filho: %d\n", getpid());
		exit(1);
	}
	return 0;
}
