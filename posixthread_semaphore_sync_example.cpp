#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "common.h"

sem_t semaphore;

void * threadfunc(void * args) {
	while (1) {
		sem_wait(&semaphore);
		printf("Hello from da thread!\n");
		printf("Press a key to close the application.\n");
		sleep(1);
	}
}

int main(void) {
	int sem_rc; // Return code
	// initialize semaphore, only to be used with threads in this process, set value to 1
	sem_rc = sem_init(&semaphore, 0, 0); // pshared(2nd arg):0 means semaphore is for threads not processes,
										//3rd argument is semaphore value: unavailable at the beginning.. This is meant to be for synchronization purpose
	CHECK (sem_rc == 0);

	pthread_t mythread;

	// start the thread
	printf("Starting thread, semaphore is initially unlocked. i.e. count 0\n");
	CHECK (pthread_create(&mythread, NULL, &threadfunc, NULL) == 0);

	printf("From an ISR (which must not include huge codes, only triggering primitives), trigger an event using an initially unavailable semaphore using: sem_post. Press a key to sem_post.\n");
	getchar();

	sem_post(&semaphore);
	printf("Semaphore unlocked. Now two threads will work together.\n");

	getchar();

	return 0;
}
