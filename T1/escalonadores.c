#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define TAM 10
#define MAX_PROG 10

int fpFIFO;
typedef struct programa {
	char nome[TAM];
	int prioridade;
	int pid;
	bool terminado;
} Programa;

Programa *lista;
int qtd=0;
int terminado=0;
bool reinicia=false;

void carrega_programa(int signo);
void encerra(int signo);

int main (int argc, char *argv[]) {
	int pid=0, pidPai=0, status, fd;
	int loop=0, i=0, j=0, contPrioridade=0, contador, flag;
	int ordem[MAX_PROG];

	time_t s;
	time_t inicio;

	signal(SIGUSR1, carrega_programa);
	signal(SIGUSR2, encerra);
	
	if ((fd=open("saida.txt",O_RDWR|O_CREAT|O_TRUNC,0666)) == -1) {
		perror("Error open()");
		return -1;
	}
	dup2(fd, 1);	

	if(access("fifo", F_OK) == 0) 	{
		if((fpFIFO = open("fifo", O_RDONLY)) <= 0) {
			printf("error opening fifo\n");
			return -2;
		}
	}
	for(i=0;i<=7;i++) {
		contPrioridade = 0;
		sleep(3);
		fflush(stdout);	
		for(j=0;j<qtd;j++) {
			if(lista[j].prioridade == i) {
				contPrioridade++;
			}
		}
		if(contPrioridade == 0) {
			if(i==7) {
				i=0;
			}
			continue;
		}
		else {
			printf("lista de %d programas de prioridade %d\n", contPrioridade, i);
		}

		j=0;
		while(contPrioridade > 0) {
			for(loop=0;loop<qtd;loop++) { 
				kill(lista[loop].pid, SIGSTOP);
			}
			fflush(stdout);
			if(lista[j].prioridade == i && lista[j].terminado == false) {
				printf("%s ira começar (prioridade: %d)\n", lista[j].nome, i);
				if(waitpid(lista[j].pid, &status, WNOHANG) != 0) { 
					printf("%s terminado\n", lista[j].nome);
					lista[j].terminado = true;
					lista[j].prioridade = 0; 
					contPrioridade--;

				}
				for(loop=0;loop<qtd;loop++) {
					if(lista[loop].terminado == true){
						terminado++;
					}
				}
				if(terminado == qtd) {
					printf("================================================\n");
					printf("Todos os programas escalonados foram finalizados\n");
					printf("================================================\n");
					kill(pidPai, SIGUSR2);
					return 0;
				}
				terminado = 0;
				
				if (i == 1 || i == 2) {
					printf("Real-Time\n");
					fflush(stdout);
					sleep(1);
					kill(lista[j].pid, SIGCONT);
					fflush(stdout);
					flag = 0;
					while (flag == 0) {
						flag = waitpid(lista[j].pid, &status, WNOHANG);
						for (contador = 0; contador < qtd; contador++) {
							if((lista[j].prioridade > lista[contador].prioridade) && (lista[contador].prioridade != 0)) {
								flag = 1;
								break;
							}
							if (flag == 1) break;
							else sleep(1);
						}
					}
					kill(lista[j].pid, SIGSTOP);
				} else if (i == 3 || i == 4 || i == 5) {
					printf("Prioridade\n");
					fflush(stdout);
					sleep(1);
					kill(lista[j].pid, SIGCONT); 
					fflush(stdout);
					sleep(3);
					kill(lista[j].pid, SIGSTOP);
				} else if ( i == 6 ) {
					printf("I/O Bound\n");
					fflush(stdout);
					sleep(1);
					kill(lista[j].pid, SIGCONT); 
					fflush(stdout);
					sleep(1);
					kill(lista[j].pid, SIGSTOP);
				} else {
					printf("CPU Bound\n");
					fflush(stdout);
					sleep(1);
					kill(lista[j].pid, SIGCONT); 
					fflush(stdout);
					sleep(2);
					kill(lista[j].pid, SIGSTOP);
				}
				if(reinicia == true){
					i=0;
					break;
				}
			}
			j++;
			if(j==qtd) {
				j=0;
			}
		}
		if(reinicia == true) {
			i=0;
			reinicia=false;
			continue;
		}
		if(i==7) {
			i=0;
		}
	}
}


void carrega_programa(int signo){

	int prioridade;
	char nome_aux[TAM];
	int pid, status;
	int pid_teste;
	read(fpFIFO, nome_aux, TAM);
	read(fpFIFO, &prioridade, sizeof(int));

	lista = realloc(lista, (qtd+1)*sizeof(Programa));
	if(lista == NULL){
		printf("Erro de alocação de memória\n");
		exit(1);
	}
	lista[qtd].prioridade = prioridade;

	pid = fork();

	strcpy(lista[qtd].nome,"");
	strcat(lista[qtd].nome, "./");
	strcat(lista[qtd].nome, nome_aux);

	
	
	if(pid!=0){
		lista[qtd].pid = pid;
	}
	else{
		lista[qtd].terminado = false;
		execve(lista[qtd].nome, NULL, NULL);
	}
	
	kill(lista[qtd].pid, SIGSTOP);

	printf("Programa: %s, PID: %d, Prioridade: %d inserido.\n", lista[qtd].nome, lista[qtd].pid, lista[qtd].prioridade);
	fflush(stdout);
	qtd++;
	reinicia = true;
}

void encerra(int signo){	
	close(fpFIFO);
	return;
}
