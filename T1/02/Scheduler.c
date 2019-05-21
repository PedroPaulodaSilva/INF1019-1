#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "Scheduler.h"

#define TIME_SLICE 1
#define TRUE 1

static int queueRunning;
static int currentTS;
static Lista * firstQueue;
static Lista * secondQueue;
static Lista * thirdQueue;
static Fila * waitIO;

int main (int argc, char * argv[]) {
	Node * auxNode = NULL;
	int totalRunningTime = 0, i;
	currentTS = 0;

	firstQueue = Lista_Create(TIME_SLICE);
	secondQueue = Lista_Create(2 * TIME_SLICE);
	thirdQueue = Lista_Create(4 * TIME_SLICE);
	queueRunning = 1;

	if (signal(SIGUSR1, IOHandler) == SIG_ERR) {
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

		Process * newProcess = Process_Create(j);
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
			Fila_Insert(newProcess, firstQueue->readyQueue);
		}
		else {
			printf("Erro no fork, abortando\n");
			exit(-1);
		}
	}

	waitIO = Fila_Create();

	while (TRUE) {
		printf("\n============== Tempo total de execucao: %d ==============\n", totalRunningTime + 1);
		Process * current = Fila_GetCurrent()->inExec;
		printf("\nFatia de Tempo = %d \n\n", currentTS);

		if (currentTS == 0) {
			if (current != NULL) {
				printf("- Interrompendo %s\n", current->name);
				kill(current->pid, SIGSTOP);

				int nextSlice = Process_findNextExeTime(current);
				printf("Proxima fatia de tempo = %d | Atual = %d\n", nextSlice, Fila_GetCurrent()->timeSlice);
				if (nextSlice >= Fila_GetCurrent()->timeSlice) {
					if (queueRunning == 1) {
						printf("- %s movido para lista 2\n", current->name);
						Fila_Insert(current, secondQueue->readyQueue);
					}
					else { 
						printf("- %s movido para lista 3\n", current->name);
						Fila_Insert(current, thirdQueue->readyQueue);
					}
				}
			}

			if (!Fila_isEmpty(firstQueue->readyQueue)) {
				queueRunning = 1;
			}
			else if (!Fila_isEmpty(secondQueue->readyQueue)) {
				queueRunning = 2;
			}
			else if (!Fila_isEmpty(thirdQueue->readyQueue)) {
				queueRunning = 3;
			}
			else if (Fila_isEmpty(waitIO)) {
				break;
			}

			if (!Fila_isEmpty(firstQueue->readyQueue) ||
					!Fila_isEmpty(secondQueue->readyQueue) ||
					!Fila_isEmpty(thirdQueue->readyQueue)) {
						printf("\n- Executando lista %d\n", queueRunning);
						currentTS = Fila_GetCurrent()->timeSlice - 1;
						Lista_getNext(Fila_GetCurrent());
						Process * proc2Run = Fila_GetCurrent()->inExec;
						printf("-> SIGCONT enviado para %s\n", proc2Run->name);
						kill(proc2Run->pid, SIGCONT);
						sleep(1); usleep(1000);
						printf("-> SIGSTOP enviado para %s\n", proc2Run->name);
						kill(proc2Run->pid, SIGSTOP);
			}
		}
		else {
			Process * proc2Run = Fila_GetCurrent()->inExec;
			if (proc2Run != NULL) {
				printf("-> SIGCONT enviado para %s\n", proc2Run->name);
				kill(proc2Run->pid, SIGCONT);
				sleep(1); usleep(1000);
				printf("-> SIGSTOP enviado para %s\n", proc2Run->name);
				kill(proc2Run->pid, SIGSTOP);
			}

			currentTS--;
		}

		if (!Fila_isEmpty(waitIO)) {
			auxNode = waitIO->first;

			while (auxNode != NULL) {
				auxNode->value->ioTime--;
				auxNode->value->runningTime++;
				printf("- %s I/O = %d\n", auxNode->value->name, auxNode->value->ioTime);
				if (auxNode->value->ioTime == 0) {
					IOHandler_Finished();
				}

				auxNode = auxNode->next;
			}
		}

		if (Fila_isEmpty(firstQueue->readyQueue) &&
				Fila_isEmpty(secondQueue->readyQueue) &&
				Fila_isEmpty(thirdQueue->readyQueue) &&
				Fila_isEmpty(waitIO) &&
				Fila_GetCurrent()->inExec == NULL) {
					printf("---  Lista Vazia  ---\n");
					break;
		}

		totalRunningTime++;
	}

	Lista_Free(firstQueue);
	Lista_Free(secondQueue);
	Lista_Free(thirdQueue);

	return 0;
}

void IOHandler_Finished() {
	Node * auxNode = waitIO->first;

	while (auxNode != NULL) {
		if (auxNode->value->ioTime == 0) {
			kill(auxNode->value->pid, SIGSTOP);
			Fila_Pop(auxNode->value, waitIO);

			if (!Process_Finished(auxNode->value)) {
				if (auxNode->value->nextQueue == 1) {
					Fila_Insert(auxNode->value, firstQueue->readyQueue);
				}
				else if (auxNode->value->nextQueue == 2) {
					Fila_Insert(auxNode->value, secondQueue->readyQueue);
				}
				else if (auxNode->value->nextQueue == 3) {
					Fila_Insert(auxNode->value, thirdQueue->readyQueue);
				}
				auxNode->value->nextQueue = 0;
			}
		}

		auxNode = auxNode->next;
	}
}

void IOHandler(int signal) {
	if (signal == SIGUSR1) {
		printf("\n======= I/O =======\n");
		Process * procInIO = Fila_GetCurrent()->inExec;

		Fila_GetCurrent()->inExec = NULL;
		procInIO->ioTime = 4;
		Fila_Insert(procInIO, waitIO);

		if (currentTS > 0 && queueRunning > 1) {
			procInIO->nextQueue = queueRunning - 1;
		}
		else {
			procInIO->nextQueue = queueRunning;
		}

		if (!Fila_isEmpty(firstQueue->readyQueue)) {
			queueRunning = 1;
		}
		else if (!Fila_isEmpty(secondQueue->readyQueue)) {
			queueRunning = 2;
		}
		else if (!Fila_isEmpty(thirdQueue->readyQueue)) {
			queueRunning = 3;
		}
		Lista_getNext(Fila_GetCurrent());
	}
}

void Handler_ChildEnded(int signal) {
	Process * current = Fila_GetCurrent()->inExec;
	if (current == NULL) {
		return;
	}

	if (signal == SIGCHLD && Process_Finished(current)) {
		if (Fila_GetCurrent()->inExec != NULL) {
			Fila_GetCurrent()->inExec = NULL;
			kill(current->pid, SIGKILL);
			
		}
	}
}

Process * Process_Create(int numberOfTimeSlices) {
	Process * p = (Process *) malloc(sizeof(Process));

	p->numTS = numberOfTimeSlices;
	p->slices = (int *) malloc(sizeof(int) * numberOfTimeSlices);
	p->runningTime = 0;
	p->ioTime = 0;
	p->nextQueue = 0;

	return p;
}

int Process_findNextExeTime(Process * proc) {
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

int Process_Finished (Process * proc) {
	int i, totalTime = 0;

	for (i = 0; i < proc->numTS; i++) {
		totalTime += proc->slices[i];
	}

	return totalTime == proc->runningTime;
}

void Process_Free(Process * proc) {
	free(proc->slices);
	free(proc);
}

Lista * Fila_GetCurrent() {
	if (queueRunning == 2) {
		return secondQueue;
	}

	if (queueRunning == 3) {
		return thirdQueue;
	}

	return firstQueue;
}

Fila * Fila_Create() {
	Fila * q = (Fila *) malloc(sizeof(Fila));

	q->first = NULL;
	q->last = NULL;
	q->size = 0;

	return q;
}

void Fila_Insert(Process * item, Fila * queue) {
	Node * newNode = (Node *) malloc(sizeof(Node));

	newNode->value = item;
	newNode->next = NULL;

	if (queue->first == NULL) {
		queue->first = newNode;
	}
	else {
		queue->last->next = newNode;
	}
	queue->last = newNode;

	(queue->size)++;
}

Process * Fila_Remove(Fila * queue) {
	Node * removingNode = queue->first;
	void * value;

	if (removingNode == NULL) {
		return NULL;
	}

	value = removingNode->value;

	queue->first = removingNode->next;
	(queue->size)--;
	free(removingNode);

	return value;
}

Process * Fila_Pop(Process * item, Fila * queue) {
	Node * removNode = NULL, * auxNode = queue->first;

	if (queue->first == queue->last) {
		queue->first = NULL;
		queue->last = NULL;

		return auxNode->value;
	}

	while (auxNode != NULL) {
		if (item == auxNode->next->value) {
			removNode = auxNode->next;
			auxNode->next = removNode->next;
			break;
		}

		auxNode = auxNode->next;
	}

	return removNode->value;
}

int Fila_isEmpty(Fila * queue) {
	return queue->first == NULL;
}

void Fila_Free(Fila * queue) {
	while (queue->first != NULL) {
		Fila_Remove(queue);
	}

	free(queue);
}

Lista * Lista_Create (int timeSlice) {
	Lista * scheduler = (Lista *) malloc(sizeof(Lista));

	scheduler->inExec = NULL;
	scheduler->readyQueue = Fila_Create();
	scheduler->timeSlice = timeSlice;

	return scheduler;
}

void Lista_getNext(Lista * scheduler) {
	if (Fila_isEmpty(scheduler->readyQueue)) {
		return;
	}

	Process * proc2Run = NULL;
	proc2Run = Fila_Remove(scheduler->readyQueue);
	int nextSlice = Process_findNextExeTime(proc2Run);

	scheduler->inExec = proc2Run;

	if (nextSlice > scheduler->timeSlice) {
		proc2Run->runningTime += scheduler->timeSlice;
	}
	else {
		proc2Run->runningTime += nextSlice;
	}
}

int Lista_isEmpty(Lista * scheduler) {
	return Fila_isEmpty(scheduler->readyQueue) &&
		   scheduler->inExec == NULL;
}

void Lista_Free (Lista * scheduler){
	Fila_Free(scheduler->readyQueue);

	free(scheduler);
}
