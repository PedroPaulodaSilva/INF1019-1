#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

int lst[9];

void *produtor(void *threadid) {
	int i;
	for(i=0; i<20; i++){
		uau = uau + 1;
		printf("\n%d: valor crescente: %d\n", threadid, crescente);
		printf("\n%d: acumulado: %d\n", threadid, uau);
		crescente = crescente + 1;
		sleep(2);
	}

	pthread_exit(NULL); /*not necessary*/

}

void *consumidor(void *threadid) {
	int i;

	for(i=0;i<30;i++) {
		uau = uau + 5;
		printf("\n%d: valor decrescente: %d\n", threadid, decrescente);
		printf("\n%d: acumulado: %d\n", threadid, uau);
		decrescente = decrescente - 1;
		sleep(1);
	}

	pthread_exit(NULL); /*not necessary*/

}

int main() {
	int t;
	pthread_t threads[2];
	
	printf("Creating thread crescente\n");
	pthread_create(&threads[0], NULL, contCrescente, (void *)0);

	printf("Creating thread decrescente\n");
	pthread_create(&threads[1], NULL, contDecrescente, (void *)1);

	for(t=0; t < 2; t++)
		pthread_join(threads[t],NULL); 
	/* wait for all the threads to terminate*/
}

