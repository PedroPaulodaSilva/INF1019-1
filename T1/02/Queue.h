#ifndef Queue_h
#define Queue_h

#include <stdio.h>
#include <stdlib.h>

#include "Process.h"

typedef struct node Node;
struct node {
	Process * value;
	Node * next;
};

typedef struct queue Queue;
struct queue {
	Node * first;
	Node * last;

	int size;
};

Queue * q_create (void);

void q_enqueue (Process * item, Queue * queue);

Process * q_dequeue (Queue * queue);

Process * q_remove (Process * item, Queue * queue);

void q_print (Queue * queue);

int q_isEmpty (Queue * queue);

void q_free (Queue * queue);

void q_print(Queue * queue);

#endif /* Queue_h */

