/********** Exercicio 1 **********/

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main (void){

	int id, pid, status;
	int i, j;
	int seg1, seg2, seg3;
	int *m1, *m2, *m3;

	seg1 = shmget(IPC_PRIVATE,9*sizeof(int),IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	seg2 = shmget(IPC_PRIVATE,9*sizeof(int),IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	seg3 = shmget(IPC_PRIVATE,9*sizeof(int),IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

	m1 = (int*) shmat(seg1,0,0);
	m2 = (int*) shmat(seg2,0,0);
	m3 = (int*) shmat(seg3,0,0);

	m1[0]=0;
	m1[1]=1;
	m1[2]=2;
	m1[3]=3;
	m1[4]=4;
	m1[5]=5;
	m1[6]=6;
	m1[7]=7;
	m1[8]=8;

	m2[0]=8;
	m2[1]=7;
	m2[2]=6;
	m2[3]=5;
	m2[4]=4;
	m2[5]=3;
	m2[6]=2;
	m2[7]=1;
	m2[8]=0;

	for(i=0; i<9; i++){

		pid = fork();

		if (pid == 0) 
			m3[i] = m1[i]+m2[i];
		else{
			waitpid ( -1, &status, 0);
			exit(0);
		}
	}

	for(i=0;i<9;i++)

		printf("p3[%d] = %d\n", i, m3[i]);


	return 0;
}

/*

Explicação:

	A saida do programa é uma matriz representada por vetor simples m3 que cada cada elemento é a soma dos elementos
correspondentes de m1 e m2. Por exemplo: p3[0] = p1[0] + p2[0].

	Com este programa é possivel entender que mesmo processos diferentes podem
compartilhar um espaćo em comum na memoria, podendo assim assumir valores e
fazer contas que podem ser acessadas por outros processos envolvidos.

Saída:

p3[0] = 8
p3[1] = 8
p3[2] = 8
p3[3] = 8
p3[4] = 8
p3[5] = 8
p3[6] = 8
p3[7] = 8
p3[8] = 8

*/
