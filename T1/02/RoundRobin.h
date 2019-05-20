#ifndef RoundRobin_h
#define RoundRobin_h

#include <stdio.h>
#include <stdlib.h>

#include "Queue.h"

struct sch_rr {
	Process * inExec;
	Queue * readyQueue;
	int timeSlice;
};
typedef struct sch_rr RRScheduler;

RRScheduler * rr_create (int timeSlice);

void rr_runNextProcess (RRScheduler * scheduler);

int rr_isEmpty (RRScheduler * scheduler);

void rr_free (RRScheduler * scheduler);

#endif /* RoundRobin_h */

