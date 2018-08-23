#include <stdio.h>
#include <signal.h>
#include <stdlib.h>


void killHandler(int signal)
{
	printf("Kill Handler\n");
}

int main(void) 

{

	void (*p)(int);
// ponteiro para função que recebe int como parâmetro 
	p = signal(SIGKILL, killHandler);
	
	while(1);	
 
}
