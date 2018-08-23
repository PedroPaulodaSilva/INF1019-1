#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void handler (int param);

int main (void) {

	int x,y;

	signal(SIGFPE, handler);

	printf("Digite o 1 numero: ");
	scanf("%d", &x);
	
	printf("Digite o 2 numero: ");
	scanf("%d", &y);

	printf("Adição: %d + %d = %d \n Subtração: %d - %d = %d \n Divisão: %d/%d = %d \n Multiplicação: %d x %d = %d \n", x, y, x+y, x, y, x-y, x, y, x/y, x, y, x*y);

	return 0;

}

void handler (int param) {

	printf ("Erro de floating point. Signal : %d\n", param);

	exit(0);

}
