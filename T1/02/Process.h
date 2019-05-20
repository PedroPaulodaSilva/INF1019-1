#ifndef Process_h
#define Process_h

#include <stdio.h>
#include <stdlib.h>

struct process {
	pid_t pid;
	char name[20];

	int numTS;
	int * slices;

	int runningTime;
	int ioTime;
	int nextQueue;
};
typedef struct process Process;

Process * p_create (int numberOfTimeSlices);

int p_findNextExeTime (Process * proc);

int p_hasFinishedRunning (Process * proc);

void p_free (Process * proc);

#endif /* Process_h */

