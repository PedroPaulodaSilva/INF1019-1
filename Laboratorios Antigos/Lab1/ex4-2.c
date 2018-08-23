#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
	int pid;
	int status;
	
	
	pid = fork();
	if(pid != 0)
	{
		waitpid(pid, &status, 0);
	}
	else
	{
		execve("./hello", NULL, NULL);
		
	}
	return 0;
}

//usamos o comando execve para chamar funções do terminal a partir do processo filho. Rodamos o executável hello, que imprime uma mensagem na tela.
