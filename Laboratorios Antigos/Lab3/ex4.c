#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void childhandler(int signo);

int delay;

int main(int argc, char *argv[]) 
{ 
	pid_t pid1, pid2;
	 
	int cont, processo, trocas;
	 
	if((pid1 = fork()) < 0) 
	{ 
		fprintf(stderr, "Erro ao criar filho\n");
		 
		exit(-1); 
	} 
	if(pid1 == 0) /*child 1 */
	{
		while (1){
			printf("O processo filho 1 esta sendo executado.\n");
			sleep(1); //Sleep para facilitar a visualizacao
		}
	} 

	if((pid2 = fork()) < 0) 
	{ 
		fprintf(stderr, "Erro ao criar filho\n");
		 
		exit(-1); 
	} 
	if(pid2 == 0) /*child 2 */
	{
		while (1){
			printf("O processo filho 2 esta sendo executado.\n");
			sleep(1); //Sleep para facilitar a visualizacao
		}
	} 

	kill(pid1, SIGSTOP);
	kill(pid2, SIGSTOP);

	processo = 1;
	trocas = 0;

	for(cont=0; cont<10; cont++){ // 10 loops para fazer a troca de processo

		if(processo == 1){
			kill(pid2, SIGSTOP);
    		kill(pid1, SIGCONT);
    		printf("TROCA DE PROCESSO!\n");
    		processo = 2;
    		trocas++; //Só para teste
		}
		else if(processo == 2){
			kill(pid1, SIGSTOP);
    		kill(pid2, SIGCONT);
    		printf("TROCA DE PROCESSO!\n");
    		processo = 1;
    		trocas++; //Só para teste
		}

		sleep(2); //Tempo de espera para efetuar a troca de processos
	}

	//Processo pai mata os processos filhos
	kill(pid1, SIGKILL); 
	kill(pid2, SIGKILL); 

	printf("Total de trocas de processo: %d\n", trocas); //Só para teste

	return 0; 
}

