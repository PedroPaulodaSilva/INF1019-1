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
	char *memo;
	char msgIN[80];

	printf("Digite a mensagem: \n");

	scanf("%s", msgIN);

	seg = shmget (8752, sizeof(int) * 80, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

	if (seg == -1) {
		printf("Erro\n");
		return -1;
	}

	memo = (char*) shmat(seg,0,0);

	// msg : mem compartilhada / msgIN : memoria dos sistema

	sprintf(memo, msgIN);

	shmdt(memo);

	return 0;

}

/*

Explicação:

Ao alocarmos a memória compartilhada (“shmget”) e atribuirmos sua chave para
8752, tudo o que salvamos naquela memória fica gravado, mesmo após liberar a
memória (shmdt), porém não podemos usar o comando “shmctl” pois dessa forma
iríamos desalocar a memória compartilhada e, como vamos usá-la no programa
seguinte, não faria sentido desaloca-la.

Execução e saída:

Digite a mensagem:

Hello!

*/
