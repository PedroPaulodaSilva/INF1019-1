#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>



int compare(const void* a, const void* b)
{
	const int *A = (const int*)a;
	const int *B = (const int*)b;
	return *A-*B;
}


int main(void)
{
	int pid;
	int status;
	int i;
	int vet[10] = {2,6,9,3,1,0,5,7,4,8};
	
	
	pid = fork();
	if(pid != 0)
	{
		for(i=0; i < 10; i++) { printf("vet[%d]:%d ||", i, vet[i]); }
		waitpid(pid, &status, 0); 
		printf("\n\n");
		for(i=0; i < 10; i++) { printf("vet[%d]:%d||", i, vet[i]); }
	}
	else
	{
		qsort(vet, 10, sizeof(int), compare);
	}
	return 0;
}

/*
Saída:
	vet[0]:2 ||vet[1]:6 ||vet[2]:9 ||vet[3]:3 ||vet[4]:1 ||vet[5]:0 ||vet[6]:5 ||vet[7]:7 ||vet[8]:4 ||vet[9]:8 ||

	vet[0]:2||vet[1]:6||vet[2]:9||vet[3]:3||vet[4]:1||vet[5]:0||vet[6]:5||vet[7]:7||vet[8]:4||vet[9]:8||

	ou seja, o vetor no processo pai não foi alterado. Isso se justifica pelo fato de, após o fork, toda a memória anterior ser copiada em outros endereços a serem usados pelo filho, e os ponteiros serem revalorados de acordo. Assim, quando o processo filho chama qsort, ele altera somente o vetor em sua própria memória, e não afeta a memória do pai. 
*/
