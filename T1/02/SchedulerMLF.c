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

int main (int argc, char * argv[]) {
	Node * auxNode = NULL;
	int totalRunningTime = 0, i;
	currentTS = 0;

	firstPriorityQueue = rr_create(TIME_SLICE);
	secondPriorityQueue = rr_create(2 * TIME_SLICE);
	thirdPriorityQueue = rr_create(4 * TIME_SLICE);
	queueRunning = 1;

	if (signal(SIGUSR1, w4ioHandler) == SIG_ERR) {
	    printf("Erro na inicializacao, finalizando scheduler\n");
	    exit(-1);
	}

	int times[50];
	int j = 0;
	printf("\n - Inicializando todos os processos - \n");

	for (i = 1; i < argc; i+=2) {
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

		char * newArgs [4];
		newArgs[0] = (char *) argv[i];
		newArgs[1] = (char *) malloc(sizeof(char) * 10);
		sprintf(newArgs[1], "%d", getpid());
		newArgs[2] = strdup(argv[i + 1]);
		newArgs[3] = NULL;

		printf("Numero do Processo %d\n", i);
		printf(" - Nome => %s\n", argv[i]);
		printf(" - Fatias de Tempo => %s\n", newArgs[2]);
		newProcess->pid = fork();
		printf(" - Pid => %d\n", newProcess->pid);
		if (newProcess->pid == 0) {
			printf("------- Executando %s\n", newProcess->name);
			execv(newProcess->name, newArgs);
			perror("Falha ao iniciar programa\n");
		}
		else if (newProcess->pid != 0) {
			usleep(2000);
			printf("Enviando SIGSTOP pois PID != 0\n");
			kill(newProcess->pid, SIGSTOP);
			q_enqueue(newProcess, firstPriorityQueue->readyQueue);
		}
		else {
			printf("Erro no fork, abortando\n");
			exit(-1);
		}
	}

	waitingIOProcs = q_create();

	while (TRUE) {
		printf("\n============== Tempo total de execucao: %d ==============\n", totalRunningTime + 1);
		Process * current = getRunningQueue()->inExec;
		printf("\nFatia de Tempo = %d \n\n", currentTS);

		if (currentTS == 0) {
			if (current != NULL) {
				printf("- Interrompendo %s\n", current->name);
				kill(current->pid, SIGSTOP);

				int nextSlice = p_findNextExeTime(current);
				printf("Proxima fatia de tempo = %d | Atual = %d\n", nextSlice, getRunningQueue()->timeSlice);
				if (nextSlice >= getRunningQueue()->timeSlice) {
					if (queueRunning == 1) {
						printf("- %s movido para lista 2\n", current->name);
						q_enqueue(current, secondPriorityQueue->readyQueue);
					}
					else { 
						printf("- %s movido para lista 3\n", current->name);
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
			else if (q_isEmpty(waitingIOProcs)) {
				break;
			}

			if (!q_isEmpty(firstPriorityQueue->readyQueue) ||
					!q_isEmpty(secondPriorityQueue->readyQueue) ||
					!q_isEmpty(thirdPriorityQueue->readyQueue)) {
						printf("\n- Executando lista %d\n", queueRunning);
						currentTS = getRunningQueue()->timeSlice - 1;
						rr_runNextProcess(getRunningQueue());
						Process * proc2Run = getRunningQueue()->inExec;
						printf("-> SIGCONT enviado para %s\n", proc2Run->name);
						kill(proc2Run->pid, SIGCONT);
						sleep(1); usleep(1000);
						printf("-> SIGSTOP enviado para %s\n", proc2Run->name);
						kill(proc2Run->pid, SIGSTOP);
			}
		}
		else {
			Process * proc2Run = getRunningQueue()->inExec;
			if (proc2Run != NULL) {
				printf("-> SIGCONT enviado para %s\n", proc2Run->name);
				kill(proc2Run->pid, SIGCONT);
				sleep(1); usleep(1000);
				printf("-> SIGSTOP enviado para %s\n", proc2Run->name);
				kill(proc2Run->pid, SIGSTOP);
			}

			currentTS--;
		}

		if (!q_isEmpty(waitingIOProcs)) {
			auxNode = waitingIOProcs->first;

			while (auxNode != NULL) {
				auxNode->value->ioTime--;
				auxNode->value->runningTime++;
				printf("- %s I/O = %d\n", auxNode->value->name, auxNode->value->ioTime);
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
					printf("---  Lista Vazia  ---\n");
					break;
		}

		totalRunningTime++;
	}

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
	Node * auxNode = waitingIOProcs->first;

	while (auxNode != NULL) {
		if (auxNode->value->ioTime == 0) {
			kill(auxNode->value->pid, SIGSTOP);
			q_remove(auxNode->value, waitingIOProcs);

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
				auxNode->value->nextQueue = 0;
			}
		}

		auxNode = auxNode->next;
	}
}

void w4ioHandler(int signal) {
	if (signal == SIGUSR1) {
		printf("\n======= I/O =======\n");
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
		if (getRunningQueue()->inExec != NULL) {
			getRunningQueue()->inExec = NULL;
			kill(current->pid, SIGKILL);
			
		}
	}
}

