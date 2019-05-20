#include <stdio.h>
#include <signal.h>

void contHandler(int signal);

int main () {
  signal(SIGCONT, contHandler);
  printf("\n\nat child program\n\n");
}

void contHandler(int signal) {
  printf("SIGCONT ARRIVED AT PROGRAM\n");
}

