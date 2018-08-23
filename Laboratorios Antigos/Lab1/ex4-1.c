#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
	int pid;
	int status;
	char *argv[] = {"/bin/echo", "abc", " efgh",  " 9 8 7 ", NULL};
	
	
	pid = fork();
	if(pid != 0)
	{
		waitpid(pid, &status, 0);
	}
	else
	{
		execve("/bin/echo", argv, NULL);
	}
	return 0;
}


//usamos o comando execve para chamar funções do terminal a partir do processo filho. Rodamos o executável echo, passando argumentos através de argv, um vetor de strings. 
