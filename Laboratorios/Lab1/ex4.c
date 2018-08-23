#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main () {
	int i;
	int contador = 1;
	int pid = fork();
	int status;

	if (pid != 0) {

		int pid2 = fork();
		int status2;

		if (pid2 != 0) {
			printf("PID pai: %d\n", getpid());
			for(i = 0; i < 50; i++) {
				contador++;
				printf(" %d, ", contador);
			}
			printf("\n Esperando filhos terminarem\n");
		}
		else {
			waitpid(pid, &status, 0);
			waitpid(pid2, &status2, 0);
			printf("PID filho 2: %d\n", getpid());
			for(i = 0; i <= 100; i++) {
				contador += 2;
				printf(" %d, ", contador);
			}
			printf("\n Finalizando filho 2\n");
			exit(1);
		}
	}
	else {
		printf("PID filho 1: %d\n", getpid());
		for(i = 0; i <= 100; i++) {
			contador += 2;
			printf(" %d, ", contador);
		}
		printf("\n Esperando filho 2 terminar\n");
	}
	return 0;
}
