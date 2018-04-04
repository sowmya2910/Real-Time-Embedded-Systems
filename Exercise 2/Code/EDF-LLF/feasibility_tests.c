/**
* @file feasibility_tests.c
* @brief This code tests the feasibility of given service set using EDF and LLF scheduling policies.
   It has been adapted from Wordpress site of Bhavesh(blog)(Refer References)
* @author Sowmya Ramakrishnan, Mounika Edula Reddy
* @date February 22, 2018
*
*/

#include <math.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define U32_T unsigned int

/* Defining all the Examples' Period, Capacity and Deadline. */

//U=0.7333
U32_T ex0_period[] = {2, 10, 15};
U32_T ex0_wcet[] = {1, 1, 2};

//U=0.9857
U32_T ex1_period[] = {2, 5, 7};
U32_T ex1_wcet[] = {1, 1, 2};

//U=0.9967
U32_T ex2_period[] = {2, 5, 7, 13};
U32_T ex2_wcet[] = {1, 1, 1, 2};

//U=0.93
U32_T ex3_period[] = {3, 5, 15};
U32_T ex3_wcet[] = {1, 2, 3};

//U=1.0
U32_T ex4_period[] = {2, 4, 16};
U32_T ex4_wcet[] = {1, 1, 4};

//U=1.0
U32_T ex5_period[] = {2, 5, 10};
U32_T ex5_wcet[] = {1, 2, 1};

//U=0.9967
U32_T ex6_period[] = {2, 5, 7, 13};
U32_T ex6_wcet[] = {1, 1, 1, 2};

//U=1.0
U32_T ex7_period[] = {3, 5, 15};
U32_T ex7_wcet[] = {1, 2, 4};

//U=0.9967
U32_T ex8_period[] = {2, 5, 7, 13};
U32_T ex8_wcet[] = {1, 1, 1, 2};


/* Function to calculate the Greatest Common Divisor */

int greatest_common_divisor (int a, int b)
{
	if(b == 0)
		return a;
	else
		greatest_common_divisor(b,a%b);
}

/* Function to calculate the Least Common Multiple */

int lcm_calculate(int array[], int n)
{
    int ans = array[0];

    for (int i=1; i<n; i++)
        ans = ( ((array[i]*ans)) /
                (greatest_common_divisor(array[i], ans)) );
    return ans;
}

/* Function to calculate the Hyperperiod which matches with the Deadline */

int hyperperiod(U32_T ex_period[], int n)
{
	int x = ex_period[0];
	x = lcm_calculate(ex_period, n);

	printf("Hyperperiod = %d \t", x);
	return x;
}

/* Function to calculate the Utilization */

float utilization_calculate(U32_T numServices, U32_T period[], U32_T t[], U32_T deadline[])
{
	float utilization = 0.0, temp;
	for(int j = 0; j < numServices; j++)
	{

		temp = ((float)t[j]/(float)period[j]);
		utilization = utilization + temp;
	}
	
	return utilization;
}

/* Function returns the smallest value of the array */

int smallest_value(int array[], int n)
{
	int small = array[0];
	int service_number = 0;
	for(int i = 0; i < n; i++)
	{
		if(small > array[i])
		{
			small = array[i];
			service_number = i;
		}	
	}
	return service_number;
}

/* Function to Calculate if Feasible through EDF */

int edf(int numServices, int period[], int wcet[], int deadline[])
{
	float utilization = 0.0f;
	int feasible=1;             
	int flag[numServices];             
	int service_num, time, j, hyperperiod_time;

    int temp_array[numServices];             
    int count_array[numServices];             

	hyperperiod_time = hyperperiod(period,numServices);  

	utilization = utilization_calculate(numServices, period, wcet, deadline);

	printf("U = %4.3f \n\n",utilization);

	if(!(utilization <= 1))
	{
		printf("utilization = %4.3f -- ", utilization);
		feasible = -1;
		return feasible;
	}
             
	time = 0;
    while(time < hyperperiod_time)           
    {
        for (j = 0; j < numServices; j++)     
        {
            if(time % period[j] == 0)           
            {
                if(flag[j] == 1 && time != 0)      
                {
                    feasible = 0;     
                    //printf("Deadline miss at t=%d -- Service %d \n", time, j+1);
                }
                flag[j] = 1;          
                temp_array[j] += period[j];     
                count_array[j] = wcet[j];
                
            }
            if(flag[j] != 1)      
            {
                deadline[j] = 50;
                continue;
                
            }
            deadline[j] = temp_array[j] - time;      
        }
        service_num = smallest_value(deadline,numServices);        
        if(flag[0] == 0 && flag[1] == 0 && flag[2] == 0)      
        {
            //printf("IDLE\n");
        }
        else
        {
            //printf("Service%d running --- time = %d\n", service_num+1, time);
        }
        
        count_array[service_num] = count_array[service_num] - 1;       
        
        if(count_array[service_num] == 0)        
        {
            flag[service_num]=0;
            
        }
	time++;
    }
 
	return feasible;
}

/* Function to Calculate if Feasible through LLF */

int llf(int numServices, int period[], int wcet[], int laxity[])
{
	float utilization;
	int feasible=1;             
	int flag[numServices];             
	int service_num, time, j, hyperperiod_time;

    int temp_array[numServices];             
    int count_array[numServices];             

	hyperperiod_time = hyperperiod(period,numServices);  

	utilization = utilization_calculate(numServices, period, wcet, laxity);

	printf("U = %4.3f \n",utilization);

	if(!(utilization <= 1))
	{
		printf("utilization = %4.3f -- ", utilization);
		feasible = -1;
		return feasible;
	}
             
	time = 0;
    while(time < hyperperiod_time)           
    {
        for (j = 0;j < numServices; j++)     
        {
            if(time % period[j] == 0)           
            {
                if(flag[j] == 1 && time != 0)      
                {
                    feasible = 0;     
                    //printf("Laxity Miss at t=%d -- Service %d \n", time, j+1);
                }
                flag[j] = 1;          
                temp_array[j] += period[j];     
                count_array[j] = wcet[j];
                
            }
            if(flag[j] != 1)      
            {
                laxity[j] = 50;
                continue;
                
            }
            laxity[j] = temp_array[j] - time - count_array[j];      
        }
        service_num = smallest_value(laxity,numServices);        
        if(flag[0] == 0 && flag[1] == 0 && flag[2] == 0)      
        {
            //printf("IDLE\n");
        }
        else
        {
            //printf("Service%d running --- time = %d\n", service_num+1, time);
        }
        
        count_array[service_num] = count_array[service_num] - 1;       
        
        if(count_array[service_num] == 0)        
        {
            flag[service_num]=0;
            
        }
	time++;
    }
 
	return feasible;
}

/* Main Function */
int main(void)
{
	
    int edf_llf;
    int NumberofServices;                    //Number of services
    printf("Enter 1 for EDF and 2 for LLF: ");
    scanf("%d",&edf_llf);

    int deadline[5];          //Deadline array
    int laxity[5];          //Laxity array
    int flag;	
 
	if(edf_llf == 1)
	{ 
		printf("*** EDF Schedule *** \n");
		
		printf("Ex-0 \n");
		NumberofServices = sizeof(ex0_period)/sizeof(U32_T);
		int flag = edf(NumberofServices, ex0_period, ex0_wcet, deadline);
		if(flag == 1)
		{
		printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}

/*		
		printf("Ex-1 \n");
		NumberofServices = sizeof(ex1_period)/sizeof(U32_T);
		flag = edf(NumberofServices, ex1_period, ex1_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
/*
		printf("Ex-2 \n");
		NumberofServices = sizeof(ex2_period)/sizeof(U32_T);
		flag = edf(NumberofServices, ex2_period, ex2_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
/*
		printf("Ex-3 \n");
		NumberofServices = sizeof(ex3_period)/sizeof(U32_T);
		flag = edf(NumberofServices, ex3_period, ex3_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
/*
		printf("Ex-4 \n");
		NumberofServices = sizeof(ex4_period)/sizeof(U32_T);
		flag = edf(NumberofServices, ex4_period, ex4_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
/*		
		printf("Ex-5 \n");
		NumberofServices = sizeof(ex5_period)/sizeof(U32_T);
		flag = edf(NumberofServices, ex5_period, ex5_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}	
*/
/*
		printf("Ex-6 \n");
		NumberofServices = sizeof(ex6_period)/sizeof(U32_T);
		flag = edf(NumberofServices, ex6_period, ex6_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
/*
		printf("Ex-7 \n");
		NumberofServices = sizeof(ex7_period)/sizeof(U32_T);
		flag = edf(NumberofServices, ex7_period, ex7_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
/*		printf("Ex-8 \n");
		NumberofServices = sizeof(ex8_period)/sizeof(U32_T);
		flag = edf(NumberofServices, ex8_period, ex8_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}

*/
	}	
	else
	{

		
		printf("*** LLF Schedule *** \n");
/*
		printf("Ex-0 \n");
		NumberofServices = sizeof(ex0_period)/sizeof(U32_T);
		flag = llf(NumberofServices, ex0_period, ex0_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
/*		
		printf("Ex-1 \n");
		NumberofServices = sizeof(ex1_period)/sizeof(U32_T);
		flag = llf(NumberofServices, ex1_period, ex1_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
/*
		printf("Ex-2 \n");
		NumberofServices = sizeof(ex2_period)/sizeof(U32_T);
		flag = llf(NumberofServices, ex2_period, ex2_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
/*
		printf("Ex-3 \n");
		NumberofServices = sizeof(ex3_period)/sizeof(U32_T);
		flag = llf(NumberofServices, ex3_period, ex3_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
/*
		printf("Ex-4 \n");
		NumberofServices = sizeof(ex4_period)/sizeof(U32_T);
		flag = llf(NumberofServices, ex4_period, ex4_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
/*		
		printf("Ex-5 \n");
		NumberofServices = sizeof(ex5_period)/sizeof(U32_T);
		flag = llf(NumberofServices, ex5_period, ex5_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}	

*/
/*
		printf("Ex-6 \n");
		NumberofServices = sizeof(ex6_period)/sizeof(U32_T);
		flag = llf(NumberofServices, ex6_period, ex6_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
/*
		printf("Ex-7 \n");
		NumberofServices = sizeof(ex7_period)/sizeof(U32_T);
		flag = llf(NumberofServices, ex7_period, ex7_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
/*
		printf("Ex-8 \n");
		NumberofServices = sizeof(ex8_period)/sizeof(U32_T);
		flag = llf(NumberofServices, ex8_period, ex8_wcet, deadline);
		if(flag == 1)
		{
			printf("FEASIBLE\n");
		}
		else
		{
			printf("INFEASIBLE\n");
		}
*/
	}
	return 0;
}



