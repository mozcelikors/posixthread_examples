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
		sem_post(&semaphore);
		sleep(1);
	}
}

int main(void) {
	int sem_rc; // Return code
	// initialize semaphore, only to be used with threads in this process, set value to 1
	sem_init(&semaphore, 0, 1); // pshared(2nd arg):0 means semaphore is for threads not processes, 3rd argument is semaphore value: 1 thread only
	CHECK (sem_rc == 0);

	pthread_t mythread;

	// start the thread
	printf("Starting thread, semaphore is unlocked.\n");
	CHECK (pthread_create(&mythread, NULL, &threadfunc, NULL) == 0);

	printf("Press a key to sem_wait.\n");
	getchar();

	sem_wait(&semaphore);
	printf("Semaphore locked.\n");

	// do stuff with whatever is shared between threads
	printf("Press a key to sem_post.\n");
	getchar();

	printf("Semaphore unlocked.\n");
	sem_post(&semaphore);

	printf("Press a key to close the application.\n");
	getchar();

	return 0;
}
