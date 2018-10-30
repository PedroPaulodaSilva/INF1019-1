#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TAM 10

void termina_interpretador(int signo){
	printf("Fim do escalonamento.\n\n");
	exit(0);
}

int main (void) {
	
	FILE *exec;
	char strPid[TAM], str1[TAM], nome[TAM], temp[255], c;
	int pid, type, prioridade, tempo, duracao, loop;
	char *argv[3];

	int fpFIFO;
	if (access("fifo", F_OK) == -1) {
		if (mkfifo("fifo", S_IRUSR | S_IWUSR) != 0) {
			printf("Erro ao criar fifo\n");
			return -1;
		}
	}
	signal(SIGUSR2, termina_interpretador);
	
	exec = fopen("exec.txt", "r");

	if (exec == NULL){
		printf("\nexec.txt invalido\n");
		exit(0);
	}

	sprintf(strPid, "%d", getpid());

	argv[0]="./Escalonadores";
	argv[1]=strPid;
	argv[2]=NULL;

	pid = fork();
	if(pid == 0) {
		execve("./escalonadores", argv, NULL);	
	}

	if((fpFIFO = open("fifo", O_WRONLY)) < 0) {
		printf("erro ao abrir fifo.\n");
		return -2;
	}
	sleep(3);

	
	while((fscanf(exec, "%s %s %s %d", str1, nome, temp, &prioridade) == 4)) {
		if(prioridade < 1 || prioridade > 7){
			printf("limite invalido (0 < prioridades < 8)");
			exit(1);
		}

		write(fpFIFO, nome, TAM);
		write(fpFIFO, &prioridade, sizeof(int)); 
		fflush(NULL);
		kill(pid, SIGUSR1);
		
		sleep(1);			
	}
	kill(pid, SIGUSR2); 
	
	close(fpFIFO);
	fclose(exec);
	while (1) {}
	
	return 0;
}
