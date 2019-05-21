typedef struct process {
	pid_t pid;
	char name[20];

	int numTS;
	int * slices;

	int runningTime;
	int ioTime;
	int nextQueue;
} Process;

typedef struct node {
	Process * value;
	struct node * next;
} Node;

typedef struct queue {
	Node * first;
	Node * last;

	int size;
} Fila;

typedef struct lst_rr {
	Process * inExec;
	Fila * readyQueue;
	int timeSlice;
} Lista;

void Lista_Free (Lista * scheduler);
int Lista_isEmpty(Lista * scheduler);
void Lista_getNext(Lista * scheduler);
Lista * Lista_Create (int timeSlice);

void Fila_Free(Fila * queue);
int Fila_isEmpty(Fila * queue);
Process * Fila_Pop(Process * item, Fila * queue);
Process * Fila_Remove(Fila * queue);
void Fila_Insert(Process * item, Fila * queue);
Fila * Fila_Create();
Lista * Fila_GetCurrent();

void Process_Free(Process * proc);
int Process_Finished (Process * proc);
int Process_findNextExeTime(Process * proc);
Process * Process_Create(int numberOfTimeSlices);

void Handler_ChildEnded(int signal);
void IOHandler(int signal);
void IOHandler_Finished();

