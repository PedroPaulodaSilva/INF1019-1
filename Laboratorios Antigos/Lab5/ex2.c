#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(void)
{
	char recebido[20];
	int fd, fr;
	
	fd = open("temp.txt",O_CREAT|O_RDWR|O_TRUNC,0666);
	fr = open("read.txt", O_RDWR,0666);
	
	dup2(fd, 1);
	
	dup2(fr, 0);
	
	
	
	scanf("%s", recebido);
	
	printf("%s", recebido);	
	
	return 0;
	
}
