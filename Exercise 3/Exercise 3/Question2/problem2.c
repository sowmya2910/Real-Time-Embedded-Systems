/*-----------------------------------------------------------------------------------------------------------------------------------*/
 /* 0.Documentation section
  -----------------------------------------------------------------------------------------------------------------------------------
  @file-problem2.c
 @brief:This file includes Completion test for edf scheduling
 @author: Vikrant Waje and Sowmya Ramakrishnan
 @date:17 February 2018
 */
/*--------------------------------------------------------------------------------------------------------------------------------*/
/*0. Documentation Section*/
#include<stdio.h>
#include<stdint.h>
#include<time.h>
#include<stdlib.h>
#include<pthread.h>
#define LOOPS (2)

pthread_mutex_t mutex;	// Declaration of mutex
int i=1;

// Structure that contains parameters X, Y,Z ,ACCELERATION, YAW, PITCH, ROLL, ITERATION, TIME STRUCTURE
typedef struct {
float X;
float Y;
float Z;
float acceleration;
float yaw;
float pitch;
float roll;
float iteration;
struct timespec timestamp;
}state;

state attitude;	// Declaring the attitude which contains all the parameters defined above
 //struct timespec timestamp;

/*---------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Update thread
 
 @param:None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void *update(void *arg){
pthread_mutex_lock(&mutex);	// Mutex lock
printf("---------------------------------------------------\n");
i++;
printf("Updating the values. Please wait.\n");
attitude.iteration++;
// Updating the attitude variables X,Y, Z, yaw ,pitch, Roll
attitude.X+=i;
attitude.Y+=i;
attitude.Z+=i;
attitude.acceleration+=i;
attitude.yaw+=i;
attitude.pitch+=i;
attitude.roll+=i;
clock_gettime(CLOCK_REALTIME,&(attitude.timestamp));
printf("Value of X = %f\n",attitude.X);
printf("Value of Y = %f\n",attitude.Y);
printf("Value of Z = %f\n",attitude.Z);
printf("Value of acceleration = %f\n",attitude.acceleration);
printf("Value of yaw = %f\n",attitude.yaw);
printf("Value of pitch = %f\n",attitude.pitch);
printf("Value of roll = %f\n",attitude.roll);
printf("Timestamp = %ld\n",attitude.timestamp.tv_nsec);


pthread_mutex_unlock(&mutex);	// Mutex unlock


}

/*---------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Get the values stored in update  thread
 
 @param:None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void *getvalues(void *arg){

pthread_mutex_lock(&mutex);	// Mutex lock

// Reading the variable values
printf("Reading the updated values\n");
printf("Value of X = %f\n",attitude.X);
printf("Value of Y = %f\n",attitude.Y);
printf("Value of Z = %f\n",attitude.Z);
printf("Value of acceleration = %f\n",attitude.acceleration);
printf("Value of yaw = %f\n",attitude.yaw);
printf("Value of pitch = %f\n",attitude.pitch);
printf("Value of roll = %f\n",attitude.roll);
printf("Timestamp = %ld\n",attitude.timestamp.tv_nsec);

pthread_mutex_unlock(&mutex);	// Mutex unlock

printf("----------------------------------------------------------------------\n");

}
void main(){

pthread_mutex_init(&mutex,NULL); // Initialising mutex
pthread_t write[LOOPS],read[LOOPS];// Initiating two threads
int i=0;
for(i=0;i<LOOPS;i++){
pthread_create(&read[i],NULL,&getvalues,NULL);	// Creating two threads
pthread_create(&write[i],NULL,&update,i);
pthread_join(write[i],NULL);	//Join threads
pthread_join(read[i],NULL);
}


}
