/**
* @file threadsafe.c
* @brief This code implements a thread-safe function.Credit: Dr. Sam Siewart
* @author Sowmya Ramakrishnan
* @date March 06, 2018
*
*/


#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>


#define NUM_THREADS (2)
#define NUM_CPUS (1)
#define NSEC_PER_SEC (1000000000)
#define NSEC_PER_MSEC (1000000)
#define NSEC_PER_MICROSEC (1000)


typedef struct      //defining structure to store data
{
  struct timespec timestamp ;
  double X;
  double Y;
  double Z;
  double Yaw;
  double Pitch;
  double Roll;
}info_t ;

info_t info; 
struct timespec start_time = {0, 0};   //timespec to store start time 
pthread_mutex_t mymutex;               //global mutex

typedef struct
{
    int threadIdx;
} threadParams_t;

pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];
pthread_attr_t rt_sched_attr[NUM_THREADS];
int rt_max_prio, rt_min_prio;
struct sched_param rt_param[NUM_THREADS];
struct sched_param main_param;
pthread_attr_t main_attr;
pid_t mainpid;

//Time difference calculation
int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t)
{
  int dt_sec=stop->tv_sec - start->tv_sec;
  int dt_nsec=stop->tv_nsec - start->tv_nsec;

  if(dt_sec >= 0)
  {
    if(dt_nsec >= 0)
    {
      delta_t->tv_sec=dt_sec;
      delta_t->tv_nsec=dt_nsec;
    }
    else
    {
      delta_t->tv_sec=dt_sec-1;
      delta_t->tv_nsec=NSEC_PER_SEC+dt_nsec;
    }
  }
  else
  {
    if(dt_nsec >= 0)
    {
      delta_t->tv_sec=dt_sec;
      delta_t->tv_nsec=dt_nsec;
    }
    else
    {
      delta_t->tv_sec=dt_sec-1;
      delta_t->tv_nsec=NSEC_PER_SEC+dt_nsec;
    }
  }

  return(1);
}

//Reading values
void *read(void *threadp)
{
  double X,Y,Z,Yaw,Pitch,Roll,a;
  struct timespec stop_time = {0, 0};    //timespec for stop time
  for(a=0;a<4;a++)
  {
  pthread_mutex_lock(&mymutex);     //locking mutex while reading
  clock_gettime(CLOCK_REALTIME, &stop_time);
  delta_t(&stop_time, &start_time, &info.timestamp);  //getting time stamp for reading
  X=info.X;
  Y=info.Y;
  Z=info.Z;
   Yaw=info.Yaw;
   Pitch=info.Pitch;
   Roll=info.Roll;
   pthread_mutex_unlock(&mymutex);
printf("Read:\n");
printf("Time:%d sec, %d nsec\n",info.timestamp.tv_sec,info.timestamp.tv_nsec);    //printing timestamp of reading the data
printf("Acceleration->X=%f Y=%f Z=%f Yaw=%f Pitch=%f Roll=%f\n",X,Y,Z,Yaw,Pitch,Roll);  //printing the read values
 usleep(30);
 }
}

//Updating values
void *update(void *threadp)
{
   int i=0,j=1,a;
   struct timespec stop_time = {0, 0};
   for(a=0;a<4;a++)
   {
   pthread_mutex_lock(&mymutex);     //locking mutex while uodating the data
   clock_gettime(CLOCK_REALTIME, &stop_time);
   delta_t(&stop_time, &start_time, &info.timestamp);   //updating time stamp
   info.X=i;                                            //updating all parameters
   info.Y=i;
   info.Z=i;
   info.Yaw=j;
   info.Pitch=j;
   info.Roll=j;
   pthread_mutex_unlock(&mymutex);                      //unlocking the mutex
   i++;
   j++;
   printf("Updating:\n");
   printf("Time:%d sec, %d nsec\n",info.timestamp.tv_sec,info.timestamp.tv_nsec);  //printing timestamp while updating
   printf("Acceleration->X=%f Y=%f Z=%f Yaw=%f Pitch=%f Roll=%f\n",info.X,info.Y,info.Z,info.Yaw,info.Pitch,info.Roll);
   usleep(20);
   }
}

//Main 
int main (int argc, char *argv[])
{
   int rc;
   int i, scope;
   cpu_set_t cpuset;

   mainpid=getpid();

   rt_max_prio = sched_get_priority_max(SCHED_FIFO);
   rt_min_prio = sched_get_priority_min(SCHED_FIFO);

   rc=sched_getparam(mainpid, &main_param);
   main_param.sched_priority=rt_max_prio;
   rc=sched_setscheduler(getpid(), SCHED_FIFO, &main_param);


   pthread_attr_getscope(&main_attr, &scope);

         for(i=0; i < NUM_THREADS; i++)
   {
       rc=pthread_attr_init(&rt_sched_attr[i]);
       rc=pthread_attr_setinheritsched(&rt_sched_attr[i], PTHREAD_EXPLICIT_SCHED);
       rc=pthread_attr_setschedpolicy(&rt_sched_attr[i], SCHED_FIFO);
       rc=pthread_attr_setaffinity_np(&rt_sched_attr[i], sizeof(cpu_set_t), &cpuset);

       rt_param[i].sched_priority=rt_max_prio-i-1;
       pthread_attr_setschedparam(&rt_sched_attr[i], &rt_param[i]);

       threadParams[i].threadIdx=i;

   }

    pthread_mutex_init(&mymutex,NULL);          //intializing mutex
    clock_gettime(CLOCK_REALTIME, &start_time); //getting start time for time stamp
     //creating update thread followed by the read thread
       pthread_create(&threads[0],   // pointer to thread descriptor
                      (void *)0,     // use default attributes
                      update, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );


       pthread_create(&threads[1],   // pointer to thread descriptor
                      (void *)0,     // use default attributes
                      read, // thread function entry point
                      (void *)&(threadParams[1]) // parameters to pass in
                     );

   for(i=0;i<NUM_THREADS;i++)
       pthread_join(threads[i], NULL);  //waiting for the 2 threads to end
   pthread_mutex_destroy(&mymutex);     //destroying the mutex

   printf("\nTEST COMPLETE\n");
}
