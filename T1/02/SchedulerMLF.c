#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "RoundRobin.h"

#define TIME_SLICE 1
#define TRUE 1

static int queueRunning;
static int currentTS;
static RRScheduler * firstPriorityQueue;
static RRScheduler * secondPriorityQueue;
static RRScheduler * thirdPriorityQueue;
static Queue * waitingIOProcs;

RRScheduler * getRunningQueue (void);

void w4ioHandler (int signal);
void finishedIoHandler ();
void childEndedHandler (int signal);

int main (int argc, const char * argv[]) {
	printf("\nScheduler initialized (pid = %d)\n", getpid());
	Node * auxNode = NULL;
	int totalRunningTime = 0, i;
	currentTS = 0;

	firstPriorityQueue = rr_create(TIME_SLICE);
	secondPriorityQueue = rr_create(2 * TIME_SLICE);
	thirdPriorityQueue = rr_create(4 * TIME_SLICE);
	queueRunning = 1;

	if (signal(SIGUSR1, w4ioHandler) == SIG_ERR) {
	    printf("Error installing handlers.\nFinishing scheduler.\n");
	    exit(-1);
	}

	// Put all processes on the major priority queue
	int times[50];
	int j = 0;
	printf("\nInitializing all processes\n");

	for (i = 0; i < argc; i+=2) {
		char * testvar = strdup(argv[i + 1]);
		char * pch;
		pch = strtok (testvar, ",");

		while (pch != NULL) {
			times[j] = atoi(pch);
			if (times[j] < 0){
				times[j] = 0;
			}
			pch = strtok (NULL, ",");
			j++;
		}

		Process * newProcess = p_create(j);
		newProcess->slices = times;
		strcpy(newProcess->name, argv[i]);

		char * newArgs [3];
		newArgs[0] = (char *) argv[i];
		newArgs[1] = (char *) malloc(sizeof(char) * 10);
		sprintf(newArgs[1], "%d", getpid());
		newArgs[2] = strdup(argv[i + 1]);


		printf("Process number %d\n", i);
		printf(" - name => %s\n", argv[i]);
		printf(" - slices => %s\n", newArgs[2]);
		newProcess->pid = fork();
		printf("\nForked (pid = %d)\n", newProcess->pid);
		if (newProcess->pid == 0) {
			printf("------- EXEC of program %s\n", newProcess->name);
			execv(newProcess->name, newArgs);
		}
		else if (newProcess->pid != 0) {
			usleep(2000);
			printf("Will send SIGSTOP newProcess\n");
			kill(newProcess->pid, SIGSTOP);
			printf("Enqueued new program\n");
			q_enqueue(newProcess, firstPriorityQueue->readyQueue);
		}
		else {
			printf("Error forking this process.\nFinishing scheduler.\n");
			exit(-1);
		}
		printf("--->  Finished parsing this program\n");
	}
	printf("--->  Finished parsing programs\n\n");

	// Initializing IO handlers
	waitingIOProcs = q_create();

	// Run the 3 priority queues while there is any process to run
	while (TRUE) {
		printf("\n\n---------------------->  Clock (%d)\n", totalRunningTime + 1);
		Process * current = getRunningQueue()->inExec;
		printf("\nCurrent timeSlice = %d \n\n", currentTS);

		if (currentTS == 0) {
			// Pause current running process
			if (current != NULL) {
				printf("- Stopping current (program %s)\n", current->name);
				kill(current->pid, SIGSTOP);

				int nextSlice = p_findNextExeTime(current);
				printf("---------- nextSlice = %d | timeSlice = %d\n", nextSlice, getRunningQueue()->timeSlice);
				if (nextSlice >= getRunningQueue()->timeSlice) {
					if (queueRunning == 1) {
						printf("- Enqueued program %s on queue 2\n", current->name);
						q_enqueue(current, secondPriorityQueue->readyQueue);
					}
					else { // queueRunning == 2
						printf("- Enqueued program %s on queue 3\n", current->name);
						q_enqueue(current, thirdPriorityQueue->readyQueue);
					}
				}
			}

			if (!q_isEmpty(firstPriorityQueue->readyQueue)) {
				queueRunning = 1;
			}
			else if (!q_isEmpty(secondPriorityQueue->readyQueue)) {
				queueRunning = 2;
			}
			else if (!q_isEmpty(thirdPriorityQueue->readyQueue)) {
				queueRunning = 3;
			}
			else if (q_isEmpty(waitingIOProcs)) { // All queues are empty
				break;
			}

			if (!q_isEmpty(firstPriorityQueue->readyQueue) ||
					!q_isEmpty(secondPriorityQueue->readyQueue) ||
					!q_isEmpty(thirdPriorityQueue->readyQueue)) {
						printf("\n- Running queue %d\n", queueRunning);
						currentTS = getRunningQueue()->timeSlice - 1;
						rr_runNextProcess(getRunningQueue());
						Process * proc2Run = getRunningQueue()->inExec;
						printf("-> SIGCONT sent to program %s\n", proc2Run->name);
						kill(proc2Run->pid, SIGCONT);
						sleep(1); usleep(1000);// Clock time
						printf("-> SIGSTOP sent to program %s\n", proc2Run->name);
						kill(proc2Run->pid, SIGSTOP);
			}
		}
		else {
			Process * proc2Run = getRunningQueue()->inExec;
			if (proc2Run != NULL) {
				printf("-> SIGCONT sent to program %s\n", proc2Run->name);
				kill(proc2Run->pid, SIGCONT);
				sleep(1); usleep(1000);// Clock time
				printf("-> SIGSTOP sent to program %s\n", proc2Run->name);
				kill(proc2Run->pid, SIGSTOP);
			}

			currentTS--;
		}

		if (!q_isEmpty(waitingIOProcs)) {
			printf("\n- waitingIOProcs queue not empty\n");
			auxNode = waitingIOProcs->first;

			while (auxNode != NULL) {
				auxNode->value->ioTime--;
				auxNode->value->runningTime++;
				printf("- %s ioTime = %d\n", auxNode->value->name, auxNode->value->ioTime);
				if (auxNode->value->ioTime == 0) {
					finishedIoHandler();
				}

				auxNode = auxNode->next;
			}
		}

		if (q_isEmpty(firstPriorityQueue->readyQueue) &&
				q_isEmpty(secondPriorityQueue->readyQueue) &&
				q_isEmpty(thirdPriorityQueue->readyQueue) &&
				q_isEmpty(waitingIOProcs) &&
				getRunningQueue()->inExec == NULL) {
					printf("---  Nothing else to do  ---\n");
					break;
		}

		totalRunningTime++;
	}

	printf("\n\n---  Finalizing Scheduler  ---\n\n");

	rr_free(firstPriorityQueue);
	rr_free(secondPriorityQueue);
	rr_free(thirdPriorityQueue);

	return 0;
}

RRScheduler * getRunningQueue() {
	if (queueRunning == 2) {
		return secondPriorityQueue;
	}

	if (queueRunning == 3) {
		return thirdPriorityQueue;
	}

	return firstPriorityQueue;
}

void finishedIoHandler() {
	printf("--- Finished IO Handler\n");
	Node * auxNode = waitingIOProcs->first;
	RRScheduler * currentQueue = getRunningQueue();

	while (auxNode != NULL) {
		if (auxNode->value->ioTime == 0) {
			kill(auxNode->value->pid, SIGSTOP);
			q_remove(auxNode->value, waitingIOProcs);
			printf("--- Removed program %s\n", auxNode->value->name);

			if (!p_hasFinishedRunning(auxNode->value)) {
				if (auxNode->value->nextQueue == 1) {
					q_enqueue(auxNode->value, firstPriorityQueue->readyQueue);
				}
				else if (auxNode->value->nextQueue == 2) {
					q_enqueue(auxNode->value, secondPriorityQueue->readyQueue);
				}
				else if (auxNode->value->nextQueue == 3) {
					q_enqueue(auxNode->value, thirdPriorityQueue->readyQueue);
				}

				printf("--- Enqueued program %s on queue %d\n", auxNode->value->name, auxNode->value->nextQueue);
				auxNode->value->nextQueue = 0;
			}
		}

		auxNode = auxNode->next;
	}
}

void w4ioHandler(int signal) {
	if (signal == SIGUSR1) {
		printf("\n--- Began IO Handler\n");
		Process * procInIO = getRunningQueue()->inExec;

		getRunningQueue()->inExec = NULL;
		procInIO->ioTime = 4;
		q_enqueue(procInIO, waitingIOProcs);

		if (currentTS > 0 && queueRunning > 1) {
			procInIO->nextQueue = queueRunning - 1;
		}
		else {
			procInIO->nextQueue = queueRunning;
		}

		if (!q_isEmpty(firstPriorityQueue->readyQueue)) {
			queueRunning = 1;
		}
		else if (!q_isEmpty(secondPriorityQueue->readyQueue)) {
			queueRunning = 2;
		}
		else if (!q_isEmpty(thirdPriorityQueue->readyQueue)) {
			queueRunning = 3;
		}
		rr_runNextProcess(getRunningQueue());
	}
}

void childEndedHandler(int signal) {
	Process * current = getRunningQueue()->inExec;
	if (current == NULL) {
		return;
	}

	if (signal == SIGCHLD && p_hasFinishedRunning(current)) {
		printf("\n--- Child Ended Handler\n");
		printf("\n--- Child Ended test\n");

		printf("\n--- Child Ended post getRunningQueue %d\n", queueRunning);
		if (getRunningQueue()->inExec != NULL) {
			getRunningQueue()->inExec = NULL;

			printf("getRunningQueue()->inExec = NULL\n");
			printf("Finishing process %s.\n", current->name);
			kill(current->pid, SIGKILL);
			// p_free(current);
		}
	}
}

