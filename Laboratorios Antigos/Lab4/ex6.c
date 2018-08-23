#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int flag = 0;
int time = 0;
int value = 0;

void initHandler (int sinal);
void quitHandler (int sinal);

int main (void){

	signal(SIGUSR1, initHandler);
	signal(SIGUSR2, quitHandler);

	printf("Preparado\n");
	
	pause();
	
	return 0;
}

void initHandler (int sinal){

	printf("Ligação iniciada.\n");
	while (1){

		if(flag == 0){

			sleep(1);

			time++;

			if(time <= 60)
				value = value + 2;
			else
				value++;
		}
	}
}

void quitHandler (int sinal){

	flag = 1;
	printf("Terminado.\nTempo gasto: %d segundos\nPreco: %d centavos\n", time, value);
	pause();

	exit(0);
}
