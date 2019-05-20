#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main (void){

	int tempo = 19;
	fflush(stdout);
	while(tempo > 0){
		printf("Faltam %d segundos.\n", tempo);
		fflush(stdout);
		sleep(1);
		tempo--;
	}
	
	fflush(stdout);
	
	return 0;
}
