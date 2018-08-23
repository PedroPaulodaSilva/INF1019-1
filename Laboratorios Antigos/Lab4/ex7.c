#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main (void) {

	int pid1, pid2, pid3;

	pid1 = fork();
	if(pid1 == 0){
		execve("./prog1", NULL, NULL);
	}
	kill(pid1, SIGSTOP);
	
	pid2 = fork();
	if(pid2 == 0){
		execve("./prog2", NULL, NULL);
	}
	kill(pid2, SIGSTOP);
	
	pid3 = fork();
	if(pid3 == 0){
		execve("./prog3", NULL, NULL);
	}
	kill(pid3, SIGSTOP);
	
	while(1){

		kill(pid3, SIGSTOP);	
		kill(pid1, SIGCONT);
		sleep(1);
		
		kill(pid1, SIGSTOP);
		kill(pid2, SIGCONT);
		sleep(2);
		
		kill(pid2, SIGSTOP);
		kill(pid3, SIGCONT);
		sleep(2);
	}
	
	return 0;

}
