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
		printf("PID pai: %d\n", getpid());
		for(i = 0; i < 50; i++) {
			contador++;
			printf(" %d, ", contador);
		}
		printf("\n Esperando filho terminar\n");
	}
	else {
		waitpid(pid, &status, 0);

		int pid2 = fork();
		int status2;

		if (pid2 != 0) {
			printf("PID filho: %d\n", getpid());
			for(i = 0; i < 100; i++) {
				contador += 2;
				printf(" %d, ", contador);
			}
			printf("\n Esperando neto terminar\n");
		}
		else {
			waitpid(pid, &status, 0);
			waitpid(pid2, &status2, 0);
			printf("PID neto: %d\n", getpid());
			for(i = 0; i < 150; i++) {
				contador += 3;
				printf(" %d, ", contador);
			}
			printf("\n Finalizando neto\n");
		}

		exit(1);
	}
	return 0;
}
