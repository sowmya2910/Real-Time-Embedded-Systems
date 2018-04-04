/*-----------------------------------------------------------------------------------------------------------------------------------*/
 /* 0.Documentation section
  -----------------------------------------------------------------------------------------------------------------------------------
  @file-posixmq.c
 @brief:Message passing using queue
 @author: Vikrant Waje and Sowmya Ramakrishnan
 @date:4 March 2018
 */
/*--------------------------------------------------------------------------------------------------------------------------------*/
/* Preprocessor directives*/
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<pthread.h>
#include<mqueue.h>
#include<sys/stat.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<mqueue.h>

#define SNDRCV_MQ	"/send_receive_mq"
#define MAX_MSG_SIZE	128
#define ERROR	(-1)
#define THREADS (2)
// Defining the thread IDs for two threads
pthread_t send, receive;
//Attributes for structure
struct mq_attr mq_attr;
// Attribute for threads
pthread_attr_t attribute[THREADS];
//Scheduling parameter
struct sched_param parameter[THREADS];

/*---------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Receiver thread
 
 @param:None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void *receiver(void *arg){
mqd_t mymq;
char buffer[MAX_MSG_SIZE];		// Define buffer
int prio;
int nbytes;
mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, 0664, &mq_attr); //Create queue with name SNDRCV_MQ with attributes given in fourth parameter structure

/*if(mymq == (mqd_t)ERROR){
printf("receiver mq_open");
exit(-1);
}*/
if((nbytes = mq_receive(mymq,buffer, MAX_MSG_SIZE, &prio)) == ERROR)	// Check for error
{
printf("mq_receive");
}
else{	// Message received successfully
buffer[nbytes]='\0';
printf("recive:msg %s received with priority =%d, length =%d\n",buffer, prio, nbytes);
}
}

static char canned_msg[] = "This is a test, and only a test, in the event of real emergency, you would be instructed...."; // Message to be sent

/*---------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Sender thread
 
 @param:None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void *sender(void *arg){
mqd_t mymq;
int prio;
int nbytes;

mymq=mq_open(SNDRCV_MQ, O_RDWR, 0664, &mq_attr);	// Create a message queue with name SNDRCV_MQ with read write permission

/*if(mymq < 0){
printf("sender mq_open");
exit(-1);
}
else{
printf("sender opened mq\n");
}*/

if((nbytes = mq_send(mymq, canned_msg, sizeof(canned_msg),30))== ERROR){	// Check for error
printf("mq_send");
}
else{	// Message sent successfully
printf("send: message successfully sent\n");
}
}
/*---------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Main function
 
 @param:None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void main(){
int i=0;
for(i=0;i<THREADS;i++){
pthread_attr_init(&attribute[i]);	// Attribute initialisation for both threads
pthread_attr_setinheritsched(&attribute[i],PTHREAD_EXPLICIT_SCHED);//Setting the scheduling policy
pthread_attr_setschedpolicy(&attribute[i], SCHED_FIFO);//Scheduling policy is set to SCHED_FIFO
parameter[i].sched_priority=99-i;	//Priority assignment
pthread_attr_setschedparam(&attribute[i],&parameter[i]);	// Assigning the remaining parameters
}


mq_attr.mq_maxmsg=10;		//Message numbers
mq_attr.mq_msgsize= MAX_MSG_SIZE;	//Message size

mq_attr.mq_flags=0;

pthread_create(&receive, &attribute[0], receiver, NULL);	// Creating receive thread

pthread_create(&send, &attribute[1] ,sender, NULL);	//Creating sender thread




pthread_join(receive,NULL);	// Join receive thread
pthread_join(send,NULL);	// Join sender thread
}
