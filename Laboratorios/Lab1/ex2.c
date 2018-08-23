#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main () {
	int i;
	int contador = 1;
	int pid = fork();	

	if (pid != 0) {
		printf("PID pai: %d\n", getpid());
		for(i = 0; i < 50; i++) {
			contador++;
			printf(" %d, ", contador);
		}
		printf("\n Esperando filho terminar\n");
	}
	else {
		
		printf("PID filho: %d\n", getpid());
		for(i = 0; i <= 100; i++) {
			contador += 2;
			printf(" %d, ", contador);
		}
		printf("\n Finalizando filho\n");
		exit(1);
	}
	return 0;
}
