#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>



int main(void)
{
	int pid;
	int status;
	int var = 1;
	
	
	pid = fork();
	if(pid != 0)
	{
		printf("pai-var: %d\n", var);
		waitpid(pid, &status, 0); 
		printf("pai-var: %d\n", var);
	}
	else 
	{
		var = 5;
		printf("filho-var: %d\n", var);
	}
	return 0;
}

//Saída:
//	pai-var: 1
//	filho-var: 5
//	pai-var: 1
//Isso se justifica, porque o pai esperou o filho usando waitpid antes de imprimir pela segunda vez. Porém, ele não foi afetado pela alteração que o filho fez na variável, já que eles após o fork não utilizam o mesmo espaço de memória, já que são processos distintos.
