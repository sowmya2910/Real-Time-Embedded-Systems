#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SEMNAME "mysemaphore"

sem_t *sem; //Semaphore Definition

int main()
{
   sem=sem_open(SEMNAME,O_CREAT,0644,0); //Creating, Initializing semaphore to 0 and opening it
   printf("Creating waiting process\n");
   printf("Waiting on semaphore\n");
   sem_wait(sem); //Making the semaphore wait until the other process unlocks it
   printf("No longer waiting on semaphore\n"); //After the other process unlocks the semaphore
   sem_destroy(sem); //Destroying semaphore
   exit(EXIT_SUCCESS);
}
