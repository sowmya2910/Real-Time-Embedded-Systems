/*-----------------------------------------------------------------------------------------------------------------------------------*/
 /* 0.Documentation section
  -----------------------------------------------------------------------------------------------------------------------------------
  @file-problem4b.c
 @brief:Message passing using queue with heap
 @author: Vikrant Waje and Sowmya Ramakrishnan
 @date:5 March 2018
 */
/*--------------------------------------------------------------------------------------------------------------------------------*/
                                                                    
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<pthread.h>
#include<mqueue.h>
#include<sys/stat.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<mqueue.h>

#define SNDRCV_MQ "/send_receive_mq_heap"

struct mq_attr mq_attr;				//message queue attribute variable

static mqd_t mymq;

pthread_t receive, send;	//declare threads 

pthread_attr_t attribute[2];		//thread attributes

struct sched_param parameter[2];		//thread params

/* receives pointer to heap, reads it, and deallocate heap memory */
/*---------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Receiver thread
 
 @param:None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void *receiver(void *threadp)
{
  char buffer[sizeof(void *)+sizeof(int)];
  void *buffptr; 
  int prio;
  int nbytes;
  int count = 0;
  int id;
 
  while(1) {

    /* read oldest, highest priority msg from the message queue */

    printf("\nReading %ld bytes\n", sizeof(void *));
  
    if((nbytes = mq_receive(mymq, buffer, (size_t)(sizeof(void *)+sizeof(int)), &prio)) == -1)

//    if((nbytes = mq_receive(mymq, (void *)&buffptr, (size_t)sizeof(void *), &prio)) == -1)

    {
      perror("mq_receive");
    }
    else
    {
      memcpy(&buffptr, buffer, sizeof(void *));
      memcpy((void *)&id, &(buffer[sizeof(void *)]), sizeof(int));
      printf("\nReceive: ptr msg 0x%p received with priority = %d, length = %d, id = %d\n", buffptr, prio, nbytes, id);

      printf("\nContents of ptr = \n%s\n", (char *)buffptr);

      free(buffptr);

      printf("\nHeap space memory freed\n");

    }
    
  }

}


static char imagebuff[4096];
/*---------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Sender thread
 
 @param:None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void *sender(void *threadp)
{
  char buffer[sizeof(void *)+sizeof(int)];
  void *buffptr;
  int prio;
  int nbytes;
  int id = 999;


  while(1) {

    /* send malloc'd message with priority=30 */

    buffptr = (void *)malloc(sizeof(imagebuff));
    strcpy(buffptr, imagebuff);
    printf("\nMessage to send = %s\n", (char *)buffptr);

    printf("\nSending %ld bytes\n", sizeof(buffptr));

    memcpy(buffer, &buffptr, sizeof(void *));
    memcpy(&(buffer[sizeof(void *)]), (void *)&id, sizeof(int));

    if((nbytes = mq_send(mymq, buffer, (size_t)(sizeof(void *)+sizeof(int)), 30)) == -1)
    {
      perror("mq_send");
    }
    else
    {
      printf("\nSend: message ptr 0x%p successfully sent\n", buffptr);
    }

    usleep(3000000);

  }
  
}


static int sid, rid;
/*---------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Main function
 
 @param:None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void main()
{
  int i, j;
  char pixel = 'A';
  int rc;

  for(i=0;i<4096;i+=64) {
    pixel = 'A';
    for(j=i;j<i+64;j++) {
      imagebuff[j] = (char)pixel++;
    }
    imagebuff[j-1] = '\n';
  }
  imagebuff[4095] = '\0';
  imagebuff[63] = '\0';

  printf("buffer =\n%s", imagebuff);
  //printf("%ld", sizeof(imagebuff));

  /* setup common message q attributes */
  mq_attr.mq_maxmsg = 100;
  mq_attr.mq_msgsize = sizeof(void *)+sizeof(int);

  mq_attr.mq_flags = 0;

  /* note that VxWorks does not deal with permissions? */
  mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, 0664, &mq_attr);

for (int i=0; i<2; i++)
{
  rc=pthread_attr_init(&attribute[i]);
  rc=pthread_attr_setinheritsched(&attribute[i], PTHREAD_EXPLICIT_SCHED);
  rc=pthread_attr_setschedpolicy(&attribute[i], SCHED_FIFO);
  parameter[i].sched_priority=99-i;
  pthread_attr_setschedparam(&attribute[i], &parameter[i]);
}

if(pthread_create(&receive, (void*)&attribute[0], receiver, NULL)==0)
	printf("\n\rReceiver Thread Created Sucessfully!\n\r");
  else perror("thread creation failed");
  
  if(pthread_create(&send, (void*)&attribute[1], sender, NULL)==0)
	printf("\n\rSender Thread Created Sucessfully!\n\r");
  else perror("thread creation failed");

  pthread_join(receive, NULL);
  pthread_join(send, NULL);


  
}


/*void shutdown(void)
{
  mq_close(mymq);
  taskDelete(sid);
  taskDelete(rid);

}*/
