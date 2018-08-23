#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
	int fd[2];
	int pid;
	char msg[] = "Hello World!";
	char recebido[20];
	if(pipe(fd) < 0)
	{
		printf("Erro ao criar Pipe.\n");
		exit(1);
	}
	if((pid = fork()) == 0)
	//processo filho
	{
		close(fd[0]);
		write(fd[1], msg, sizeof(msg));
		close(fd[1]);
		return 0;
	}
	else
	//processo pai
	{
		close(fd[1]);
		read(fd[0], recebido, sizeof(recebido));
		printf("%s \n", recebido);
		close(fd[0]);
		return 0;
	}
}
