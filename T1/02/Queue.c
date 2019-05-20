#include "Queue.h"

Queue * q_create() {
	Queue * q = (Queue *) malloc(sizeof(Queue));

	q->first = NULL;
	q->last = NULL;
	q->size = 0;

	return q;
}

void q_enqueue(Process * item, Queue * queue) {
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

Process * q_dequeue(Queue * queue) {
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

Process * q_remove(Process * item, Queue * queue) {
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

void q_print(Queue * queue) {
	Node * auxNode = queue->first;
	int count = 1;

	if (q_isEmpty(queue)) {
		printf(" - Empty\n");
		return;
	}

	while (auxNode != NULL) {
		printf(" - Process %d: %s\n", count, auxNode->value->name);

		auxNode = auxNode->next;
		count++;
	}

}

int q_isEmpty(Queue * queue) {
	return queue->first == NULL;
}

void q_free(Queue * queue) {
	while (queue->first != NULL) {
		q_dequeue(queue);
	}

	free(queue);
}

