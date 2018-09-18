#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
int crescente = 1;
int decrescente = 30;

void *contCrescente(void *threadid) {
	int i;
	for(i=0; i<20; i++){
		printf("\n%d: valor %d\n", threadid, crescente);
		crescente = crescente + 1;
		sleep(2);
	}

	pthread_exit(NULL); /*not necessary*/

}

void *contDecrescente(void *threadid) {
	int i;

	for(i=0;i<30;i++) {
		printf("\n%d: valor %d\n", threadid, decrescente);
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

