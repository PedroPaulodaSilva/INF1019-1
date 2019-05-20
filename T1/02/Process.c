#include "Process.h"

Process * p_create(int numberOfTimeSlices) {
	Process * p = (Process *) malloc(sizeof(Process));

	p->numTS = numberOfTimeSlices;
	p->slices = (int *) malloc(sizeof(int) * numberOfTimeSlices);
	p->runningTime = 0;
	p->ioTime = 0;
	p->nextQueue = 0;

	return p;
}

int p_findNextExeTime(Process * proc) {
	int i = 0, sumExeTime, nextExecTime = proc->slices[i];
	sumExeTime = nextExecTime;

	while (proc->runningTime > sumExeTime && i < proc->numTS) {
		nextExecTime = proc->slices[i];
		sumExeTime += nextExecTime;

		i++;
	}

	if (sumExeTime == proc->runningTime) {
		nextExecTime = proc->slices[i + 1];
	}
	else {
		nextExecTime -= proc->runningTime;
	}

	return nextExecTime;
}

int p_hasFinishedRunning (Process * proc) {
	int i, totalTime = 0;

	for (i = 0; i < proc->numTS; i++) {
		totalTime += proc->slices[i];
	}

	return totalTime == proc->runningTime;
}

void p_free(Process * proc) {
	free(proc->slices);
	free(proc);
}

