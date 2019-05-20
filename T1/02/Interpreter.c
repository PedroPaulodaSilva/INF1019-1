#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>

#define BUFFER_SIZE 200
#define NAME_SIZE 20
#define IO_SIZE 200

int main(int argc, char const *argv[]) {
	char programName[NAME_SIZE];
	char io[IO_SIZE];
	char schedule[IO_SIZE];
	char * buffer[BUFFER_SIZE];
	int index = 0;

	printf("Digite o comando 'exec <nomedoprograma> <sequenciadetempo>'\n");

	while(scanf(" exec %s %s", programName, io) == 2){
			 buffer[index] = (char*)malloc(strlen(programName)+1);
			 strcpy(buffer[index], programName);
			 index++;
			 buffer[index] = (char*)malloc(strlen(io)+1);
			 strcpy(buffer[index], io);
			 index++;
	 }

	 execv("SchedulerMLF", buffer);


	 return 0;
}

