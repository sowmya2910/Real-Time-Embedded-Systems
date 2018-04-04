/*-----------------------------------------------------------------------------------------------------------------------------------*/
 /* 0.Documentation section
  -----------------------------------------------------------------------------------------------------------------------------------
  @file-problem5.c
 @brief:This file includes code for timedlockmutex.
 @author: Vikrant Waje and Sowmya Ramakrishnan
 @date:3 March 2018
 */
/*--------------------------------------------------------------------------------------------------------------------------------*/
/* Preprocessor directives*/
#include<stdio.h>
#include<stdint.h>
#include<time.h>
#include<stdlib.h>
#include<pthread.h>

#define LOOPS (2)
pthread_mutex_t mutex;	// Initialising mutex
int i=1;
struct timespec tensecond;// Defining timing parameter for timed mutex
//Defining the structure under which various parameters are defined
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

state attitude;	// State attitude which contains above defined parameters
 //struct timespec timestamp;

/*---------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Update thread
 
 @param:None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void *update(void *arg){
pthread_mutex_lock(&mutex);
printf("---------------------------------------------------\n");
i++;
printf("Updating the values. Please wait.\n");
attitude.iteration++;
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
usleep(12000000);

pthread_mutex_unlock(&mutex);


}

/*---------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Get the value in the  thread
 
 @param:None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void *getvalues(void *arg){
int i=-1;
while(i!=0){
tensecond.tv_sec=10;
i=pthread_mutex_timedlock(&mutex,&tensecond);	// Mutex timed lock
printf("No new data available at %ld\n",attitude.timestamp.tv_sec);
}

//pthread_mutex_lock(&mutex);


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

 
/*---------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Main function
 
 @param:None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void main(){

pthread_mutex_init(&mutex,NULL);	// Initialising mutex
pthread_t write[LOOPS],read[LOOPS],update10;
int i=0;
for(i=0;i<LOOPS;i++){
pthread_create(&read[i],NULL,&getvalues,NULL);	// Creating read thread
pthread_create(&write[i],NULL,&update,i);		// Creating write thread
//pthread_create(&update10,NULL,&updateevery10,NULL);

pthread_join(write[i],NULL);		// Join write thread
pthread_join(read[i],NULL);			// Join read thread
//pthread_join(updateevery10,NULL);
}


}
