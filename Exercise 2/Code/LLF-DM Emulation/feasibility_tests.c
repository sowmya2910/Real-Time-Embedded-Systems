#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/**************************************************************************************
*@Filename:llf_scheduler.c
*
*@Description: Emulation of LLF and DM
*
*@Author:Mounika Reddy Edula
*        Sowmya Ramakrishnana
*@Date:2/23/2018
*@compiler:gcc
*@debugger:gdb
**************************************************************************************/
#define U32_T unsigned int
// U=0.7333
U32_T ex0_period[] = {2, 10, 15};
U32_T ex0_deadline[] = {2, 10, 15};
U32_T ex0_wcet[] = {1, 1, 2};

// U=0.9857
U32_T ex1_period[] = {2, 5, 7};
U32_T ex1_deadline[] = {2, 5, 7};
U32_T ex1_wcet[] = {1, 1, 2};

// U=0.9967
U32_T ex2_period[] = {2, 5, 7, 13};
U32_T ex2_deadline[] = {2, 5, 7,13};
U32_T ex2_wcet[] = {1, 1, 1, 2};

// U=0.93
U32_T ex3_period[] = {3, 5, 15};
U32_T ex3_deadline[] = {3, 5, 15};
U32_T ex3_wcet[] = {1, 2, 3};

// U=1.0
U32_T ex4_period[] = {2, 4, 16};
U32_T ex4_deadline[] = {2, 4, 16};
U32_T ex4_wcet[] = {1, 1, 4};


U32_T ex5_period[] = {2, 5, 10};
U32_T ex5_deadline[] = {2, 5, 10};
U32_T ex5_wcet[] = {1, 2, 1};

U32_T ex6_period[] = {2, 5, 7,13};
U32_T ex6_deadline[] = {2, 3,7,15};
U32_T ex6_wcet[] = {1, 1, 1, 2};

U32_T ex7_period[] = {3, 5, 15};
U32_T ex7_deadline[] = {3, 5, 15};
U32_T ex7_wcet[] = {1, 2, 4};

U32_T ex8_period[] = {2, 5, 7,13};
U32_T ex8_deadline[] = {2, 5,7,13};
U32_T ex8_wcet[] = {1, 1, 1, 2};

//Emulation of differnt Services

void service1()
{
  printf("***Service1***\n");
  sleep(1);
}

void service2()
{
  printf("***Service 2***\n");
  sleep(1);
}

void service3()
{
  printf("***Service 3***\n");
  sleep(1);
}

void service4()
{
  printf("***Service 4***\n");
  sleep(1);
}

//Calculate U for Deadmonotonic Scheduler
float utilization_calculate_dm(U32_T numServices,U32_T period[],U32_T wcet[],U32_T deadline[])
{
  float utilization = 0.0,temp;
  for(int j = 0;j<numServices;j++)
  {
    temp = (float)wcet[j]/(float)deadline[j];
    utilization = utilization + temp;
  }
  return utilization;
}

//Emulate LLF scheduler
void llf_scheduler(U32_T numServices,U32_T period[],U32_T wcet[],U32_T deadline[])
{

  U32_T priority = 0;
  U32_T current_time = 0;
  int laxity[numServices];
  U32_T serice_completed[numServices];
  for(int i = 0;i <numServices;i++)
  serice_completed[i] = 0;
  int exec1 = wcet[0];
  int exec2 = wcet[1];
  int exec3 = wcet[2];
  int exec4 = wcet[3];
  int deadline0 = deadline[0];
  int deadline1 = deadline[1];
  int deadline2 = deadline[2];
  int deadline3 = deadline[3];
  int period0 = period[0];
  int period1 = period[1];
  int period2 = period[2];
  int period3 = period[3];
  U32_T no_cycles_service[numServices];
  for(int i = 0;i <numServices;i++)
  no_cycles_service[i] = 0;
  while(1)
  {
    printf("****Cycle%d*****\n",current_time);
    if(current_time == period[0])
    {
      printf("new service 1\n ");
      no_cycles_service[0] = 0;
      serice_completed[0] = 0;
      exec1 = wcet[0];
      deadline[0] = deadline[0] + deadline0;
      period[0] = period[0] + period0;
      //printf("deadline[0]:%d period :%d\n",deadline[0],period[0]);
    }
    if(current_time == period[1])
    {
      printf("new service 2\n ");
      no_cycles_service[1] = 0;
      serice_completed[1] = 0;
      exec2 = wcet[1];
      deadline[1] = deadline[1] + deadline1;
      period[1] = period[1] + period1;
    }
    if(current_time == period[2])
    {
      printf("new service 3\n ");
      no_cycles_service[2] = 0;
      serice_completed[2] = 0;
      exec3 = wcet[2];
      deadline[2] = deadline[2] + deadline2;
      period[2] = period[2] + period2;
    }
    if(current_time == period[3])
    {
      printf("new service 4\n ");
      no_cycles_service[3] = 0;
      serice_completed[3] = 0;
      exec4 = wcet[3];
      deadline[3] = deadline[3] + deadline3;
      period[3] = period[3] + period3;
    }
    for(int i = 0;i<numServices;i++)
    {
      if(serice_completed[i] == 0)
      {
      laxity[i] = deadline[i] - current_time - wcet[i] + no_cycles_service[i];
      //printf("deadline %d,current_time %d,wcet %d\n",deadline[i],current_time,wcet[i]);
      }
      if(laxity[i] < 0 )
      {
        printf("Scheduling not feasible\n");
        printf("Exiting the scheduler...\n");
        exit(0);
      }
      if(serice_completed[i] == 1)
      laxity[i] = 63576;
      printf("laxity S%d is %d\n",i,laxity[i]);
    }
    U32_T min = 0;
    for(int i = 0;i<numServices;i++)
    {
      if(laxity[i]<laxity[min])
      min = i;
    }

    if(min == 0)
    {
    service1();
    exec1--;
    no_cycles_service[0]++;
    if(exec1 == 0 )
    serice_completed[0] = 1;
    }
    if(min == 1)
    {
    service2();
    exec2--;
    no_cycles_service[1]++;
    if(exec2 == 0 )
    serice_completed[1] = 1;
    }
    if(min == 2)
    {
    service3();
    exec3--;
    no_cycles_service[2]++;
    if(exec3 == 0 )
    serice_completed[2] = 1;
    }
    if(min == 3)
    {
    service4();
    exec4--;
    no_cycles_service[3]++;
    if(exec4 == 0 )
    serice_completed[3] = 1;
    }
    current_time++;
   }
}

/*****Emulating Deadline Monotonic Scheduler *******/
void deadline_monotonic_scheduler(U32_T numServices,U32_T period[],U32_T wcet[],U32_T deadline[])
{
  U32_T priority[numServices];
  int temp = 0;
  int min = 0;
  int current_time = 0;
  int service_completed[numServices];
  int deadline0 = deadline[0];
  int deadline1 = deadline[1];
  int deadline2 = deadline[2];
  int deadline3 = deadline[3];
  int period0 = period[0];
  int period1 = period[1];
  int period2 = period[2];
  int period3 = period[3];
  for(int i = 0;i<numServices;i++)
  service_completed[i] = 0;
  for(int j = 0;j<(numServices-1);j++)
  {
  for(int i = j+1;i<(numServices);i++)
  {
    if( deadline[j] < deadline[i])
    {
      min = j;
    }
  }
  priority[min] = temp;
  printf("priority[%d]:%d\n",min,priority[min]);
  temp++;
  }
  priority[numServices-1] = 3;
  while(1)
  {
    printf("****Cycle %d****",current_time);
    if(current_time == period[0])
    {
      period[0] = period[0] + period0;
      service_completed[0] = 0;
    }
    if(current_time == period[1])
    {
      period[1] = period[1] + period1;
      service_completed[1] = 0;
    }
    if(current_time == period[2])
    {
      period[2] = period[2] + period2;
      service_completed[2] = 0;
    }
    if(current_time == period[3])
    {
      period[3] = period[3] + period3;
      service_completed[3] = 0;
    }

    for(int i = 0;i<numServices;i++)
    {
    if(priority[i] == 0 && service_completed[i] != wcet[i])
    {
      service1();
      current_time++;
      service_completed[i]++;
      break;
    }
    if(priority[i] == 1 && service_completed[i] != wcet[i])
    {
      service2();
      current_time++;
      service_completed[i]++;
      break;
    }
    if(priority[i] == 2 && service_completed[i] != wcet[i])
    {
      service3();
      current_time++;
      service_completed[i]++;
      break;
    }
    if(priority[i] == 3 && service_completed[i] != wcet[i])
    {
      service4();
      current_time++;
      service_completed[i]++;
      break;
    }
    else if((service_completed[0] == wcet[0]) && (service_completed[1] == wcet[1]) && (service_completed[2] == wcet[2]) && (service_completed[3] == wcet[3]))
    {
      printf("Empty Cycle\n");
      current_time++;
      break;
    }
    if(current_time > deadline[i])
     {
       if(service_completed[i] == 0)
       {
       printf("Scheduling not feasible\n" );
       exit(0);
       }
     }
    }
  }
}

int main()
{
  int i;
  U32_T numServices;
  float u;
  numServices = sizeof(ex8_period)/sizeof(U32_T);
  u = utilization_calculate_dm(numServices,ex6_period,ex6_wcet,ex6_deadline);
  printf("Utilization %f\n",u );
  if(u <= 1)
  printf("FEASIBLE\n");
  else
  printf("INFEASIBLE\n");
  deadline_monotonic_scheduler(numServices,ex6_period,ex6_wcet,ex6_deadline);
  //llf_scheduler(numServices, ex0_period, ex0_wcet, ex0_deadline);
  //llf_scheduler(numServices, ex1_period, ex1_wcet, ex1_deadline);
  //llf_scheduler(numServices, ex2_period, ex2_wcet, ex2_deadline);
  //llf_scheduler(numServices, ex3_period, ex3_wcet, ex3_deadline);
  //llf_scheduler(numServices, ex4_period, ex4_wcet, ex4_deadline);
  //llf_scheduler(numServices, ex5_period, ex5_wcet, ex5_deadline);
  //llf_scheduler(numServices, ex7_period, ex7_wcet, ex7_deadline);
  //llf_scheduler(numServices, ex2_period, ex2_wcet, ex2_deadline);
}
