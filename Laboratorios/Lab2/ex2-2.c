/********** Exercicio 2 **********/

#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main (void) {

	int seg;
	char *msg;

	seg = shmget (8752, sizeof (int) * 80, S_IRUSR | S_IWUSR);

	if (seg == -1) {

		printf("Erro\n");
		return -1;
	}

	msg = (char*) shmat(seg,0,0);

	printf("A mensagem do dia eh: %s\n", msg);

	shmdt(msg);

	shmctl(seg, IPC_RMID, 0);

	return 0;
}

/*

Explicação:

Nesse programa, tivemos que utilizar o comando “shmctl” para desalocarmos a memória
já utilizada. Na hora dos testes tivemos que mudar várias vezes o valor das chaves de
memória, pois quando utilizamos uma vez a mesma fica “usada” e não é possível
fazer modificações em cima dela.

Esse programa mostra que o processo pode ser utilizado por mais de um programa,
sendo possível alocar e utilizar a mesma memória compartilhada.
Execução e saída:

A mensagem do dia eh: Hello!

*/
