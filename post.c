#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<sys/sem.h>
#include<sys/stat.h>
#include<fcntl.h>

#define SEMNAME "mysemaphore"

int main()
{
	sem_t *sem=sem_open(SEMNAME,0); ////Initializing semaphore to 0 and opening it
	printf("Thread is unlocked\n"); 
	sem_post(sem); //Incrementing the semaphore after bringing it out of wait state
	printf("Exiting\n");
	return 0;
}
	
