/********** Exercicio 3 **********/

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

int main (void) {

	int *vet, n = 9, values[5] = {3,5,1,9,1000};
	int seg, pid;
	int ini = 0,i;
	int j, procurado = 0, encontrado = 0;

	seg = shmget (IPC_PRIVATE, sizeof(int)*n, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if (seg == -1) {
		printf ("Erro!"); 
		exit(1);
	}

	vet = (int*) shmat (seg, 0, 0);
	for (i = 0; i < n; i++) {
		vet[i] = values[i];
	}

	printf("Digite um dos seguintes valores para procurar:\n");
	for(j=0;j<5;j++){
		printf("%d\n", values[j]);
	}
	printf("\nValor: ");
	scanf("%d", &procurado);

	pid = fork();

	if (pid != 0) {
		ini += 3;
		pid = fork();
		if (pid != 0) {
			ini+=3;
			pid = fork();
		}
	}
	if (pid == 0) {
		for(i=0; i < 3; i++) {	
			if(vet[i+ini] == procurado){
				printf("O valor esta na posicao %d\n", i+ini);
				encontrado = 1;
				exit(1);
			}
		}
	}

	shmdt (vet); 
	shmctl (seg, IPC_RMID, 0); 

	return 0;
}

/*

Explicação:

O programa possui um vetor de 5 posicoes que é colocado na memoria compartilhada.
O usuario recebe uma mensagem com os valores de cada posicoes e deve escolher qual quer procurar.
Posteriormente dois processos filhos sao criados e cada um deles eh responsavel por encontrar
o valor em uma range do vetor.

Execucao e saida:

Digite um dos seguintes valores para procurar:
3
5
1
9
1000

Valor: 1
O valor esta na posicao 2

*/
