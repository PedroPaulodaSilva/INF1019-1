#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main (void){

	int soma = 0;

	while(soma < 5){
		sleep(1);
		soma++;
	}
	
	return 0;
}
