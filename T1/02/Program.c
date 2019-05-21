#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	printf("\n>>>>>> Programa %s iniciado (pid = %d)\n", argv[0], getpid());
	char * testvar = strdup(argv[2]), * pch;
	int fatherPid = strtol(argv[1], NULL, 10), timeSlice;
	pch = strtok (testvar, ",");
	while (pch != NULL) {
		timeSlice = atoi(pch);		
		for (int i = 0; i < timeSlice; i++) {
			sleep(1);
			printf("\n>>>>>> Pid: %d\n", getpid());
		}
		pch = strtok (NULL, ",");

		if (pch != NULL) {
			printf("\n>>>>>> Pausa para I/O\n");
			kill(fatherPid, SIGUSR1);
			kill(getpid(), SIGSTOP);
		}
	}

	printf("\n>>> Processo %d finalizado \n", getpid());
	kill(fatherPid, SIGCHLD);

    return 0;
}
