#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	int pid = fork();
	int status;
	if(pid != 0)
	{
		waitpid(pid, &status, 0); 
		printf("Sou o pai e meu pid é %d\n", getpid());
	}
	else 
	{
		printf("Sou o filho e meu pid é %d\n", getpid());
	}
	return 0;
}

//Saída:
//	Sou o filho e meu pid é 3653
//	Sou o pai e meu pid é 3652
// 
//Criamos dois processos distintos, usando a função fork. Ela retorna o pid do filho criado. Assim, no processo pai pid é != de 0, e no filho igual a 0(não inicializado). Depois, usamos a função getpid() para obter os valores identificadores de cada processo. O filho foi impresso antes já que o pai esperou seu término, usando a função waitpid. 
