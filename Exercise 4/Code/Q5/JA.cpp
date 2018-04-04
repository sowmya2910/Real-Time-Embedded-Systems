/****************************************************************************************************
* Author - Virag Gada, Sowmya Ramakrishnan
* Reference examples from http://mercury.pr.erau.edu/~siewerts/cs415/code/computer_vision_cv3_tested/
*           - example-stereo-transform-improved
*           - simple-canny-interactive
*           - simple-hough-eliptical-interactive
****************************************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <sys/param.h>
#include <pthread.h>
#include <semaphore.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#define canny_deadline 20
#define hough_deadline 30
#define hough_elip_deadline 40

#define HRES 160
#define VRES 120
#define MSEC 1000000

/*Define structure for timestamp*/
struct timespec timestamp;

/*Define semaphores*/
sem_t semCanny, semHough, semHoughElip;

long start_sec;
long stop_sec;
long start_nsec;
long stop_nsec;
long delta_time_sec;
long delta_time_nsec;
long jitter;

/* Threads for each transform */
pthread_t thread_canny;
pthread_t thread_hough_elip;
pthread_t thread_hough;

/*pthread attribute structures*/
pthread_attr_t canny_attr;
pthread_attr_t hough_elip_attr;
pthread_attr_t hough_attr;
pthread_attr_t main_sched_attr;

int max_prio, min_prio;


struct sched_param canny_param;
struct sched_param hough_elip_param;
struct sched_param hough_param;
struct sched_param main_param;
struct sched_param nrt_param;


// Transform display window
char timg_window_name[] = "Edge Detector Transform";

/* Canny transform parameters */
int lowThreshold = 0;
int const max_lowThreshold = 100;
int kernel_size = 3;
int edgeThresh = 1;
int ratio = 3;
Mat canny_frame,cdst, timg_gray, timg_grad;

/*Define variable to store each frame as image*/
IplImage* frameCanny;
IplImage* frameHough;
IplImage* frameHoughElip;

CvCapture* capture;

/* Function to perform Canny transform based on threshold */
void CannyThreshold(int, void*)
{
    Mat mat_frame(frameCanny);

    cvtColor(mat_frame, timg_gray, CV_RGB2GRAY);

    // Reduce noise with a kernel 3x3
    blur( timg_gray, canny_frame, Size(3,3) );

    // Canny detector
    Canny( canny_frame, canny_frame, lowThreshold, lowThreshold*ratio, kernel_size );

    // Using Canny's output as a mask, we display our result
    timg_grad = Scalar::all(0);

    mat_frame.copyTo( timg_grad, canny_frame);

    /*Open window to display result*/
    //imshow( "Canny Tranasform", timg_grad );

}

/* Thread to perform canny transform*/
void *canny_func(void *threadid)
{
  long val = 0;
  while(1){
    /*Hold semaphore*/
    sem_wait(&semCanny);

    /*Get start time*/
    clock_gettime(CLOCK_REALTIME, &timestamp);
    printf("\n\rTimestamp when Canny capture started: Seconds:%ld and Nanoseconds:%ld",timestamp.tv_sec, timestamp.tv_nsec);
    start_sec = timestamp.tv_sec;
    start_nsec = timestamp.tv_nsec;

    /*Capture and store frame*/
    frameCanny=cvQueryFrame(capture);

    if(!frameCanny)
      return(0);

    CannyThreshold(0, 0);

    char q = cvWaitKey(33);
    if( q == 'q' )
    {
        printf("got quit\n");
        return(0);
    }

    clock_gettime(CLOCK_REALTIME, &timestamp);
    printf("\n\rTimestamp when CANNY capture stopped Seconds:%ld and Nanoseconds:%ld",timestamp.tv_sec, timestamp.tv_nsec);
    stop_sec = timestamp.tv_sec;
    stop_nsec = timestamp.tv_nsec;
    delta_time_sec = stop_sec-start_sec;
    delta_time_nsec = stop_nsec-start_nsec;

    val = (delta_time_sec*1000+delta_time_nsec/MSEC);

    jitter = canny_deadline-val;
    printf("\n\rCanny Jitter obtained is %ld nanoseconds\n\r", jitter);

    /*release semaphore for next thread*/
    sem_post(&semHough);
  }
  pthread_exit(NULL);
}

/* Thread to perform hough transform*/
void *hough_func(void *threadid)
{
  long val;
  while(1){

    /*Hold semaphore*/
    sem_wait(&semHough);

    Mat gray, canny_frame, cdst;
    vector<Vec4i> lines;

    /*Get start time*/
    clock_gettime(CLOCK_REALTIME, &timestamp);
    printf("\n\rTimestamp when HOUGH capture started: Seconds:%ld and Nanoseconds:%ld",timestamp.tv_sec, timestamp.tv_nsec);
    start_sec = timestamp.tv_sec;
    start_nsec = timestamp.tv_nsec;

    /*Capture and store frame*/
    frameHough=cvQueryFrame(capture);

    /*Convert to matrix*/
    Mat mat_frame(frameHough);
    Canny(mat_frame, canny_frame, 50, 200, 3);

    cvtColor(canny_frame, cdst, CV_GRAY2BGR);
    cvtColor(mat_frame, gray, CV_BGR2GRAY);

    HoughLinesP(canny_frame, lines, 1, CV_PI/180, 50, 50, 10);

    for( size_t i = 0; i < lines.size(); i++ )
    {
      Vec4i l = lines[i];
      line(mat_frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
    }


    if(!frameHough)
      return 0;

    //cvShowImage("Capture Example", frameHough);

    char q = cvWaitKey(33);
    if( q == 'q' )
    {
        printf("got quit\n");
        return(0);
    }


    /*Get capture stop time*/
    clock_gettime(CLOCK_REALTIME, &timestamp);
    printf("\n\rTimestamp when HOUGH captures stopped Seconds:%ld and Nanoseconds:%ld",timestamp.tv_sec, timestamp.tv_nsec);
    stop_sec = timestamp.tv_sec;
    stop_nsec = timestamp.tv_nsec;
    delta_time_sec = stop_sec-start_sec;
    delta_time_nsec = stop_nsec-start_nsec;

    /*Calculate jitter*/
    val = (delta_time_sec*1000+delta_time_nsec/MSEC);
    jitter = hough_deadline-val;
    printf("\n\rHough Jitter obtained is %ld nanoseconds\n\r", jitter);

    /*Release semaphore foor next thread*/
    //usleep(311000);
    sem_post(&semHoughElip);
  }
    pthread_exit(NULL);
}

/* Thread to perform hough eliptical transform*/
void *hough_elip_func(void *threadid)
{
  long val;
  while(1){
    /*Hold semaphhore*/
    sem_wait(&semHoughElip);

    Mat gray;
    vector<Vec3f> circles;

    /*Get capture start time*/
    clock_gettime(CLOCK_REALTIME, &timestamp);
    printf("\n\rTimestamp when HOUGH ELIPTICAL capture started: Seconds:%ld and Nanoseconds:%ld",timestamp.tv_sec, timestamp.tv_nsec);
    start_sec = timestamp.tv_sec;
    start_nsec = timestamp.tv_nsec;

    /*Capture and store frame*/
    frameHoughElip=cvQueryFrame(capture);

    /*convert image to matrix and then to grayscale*/
    Mat mat_frame(frameHoughElip);
    cvtColor(mat_frame, gray, CV_BGR2GRAY);
    GaussianBlur(gray, gray, Size(9,9), 2, 2);

    HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, gray.rows/8, 100, 50, 0, 0);

    for( size_t i = 0; i < circles.size(); i++ )
    {
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      // circle center
      circle( mat_frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
      // circle outline
      circle( mat_frame, center, radius, Scalar(0,0,255), 3, 8, 0 );
    }


    if(!frameHoughElip)
      return(0);

    ///cvShowImage("Capture Example", frameHoughElip);

    char q = cvWaitKey(33);
    if( q == 'q' )
    {
        printf("got quit\n");
        return(0);
    }

    /*Get capture stop time*/

    clock_gettime(CLOCK_REALTIME, &timestamp);
    printf("\n\rTimestamp when HOUGH ELIPTICAL capture stopped Seconds:%ld and Nanoseconds:%ld\n",timestamp.tv_sec, timestamp.tv_nsec);
    stop_sec = timestamp.tv_sec;
    stop_nsec = timestamp.tv_nsec;
    delta_time_sec = stop_sec-start_sec;
    delta_time_nsec = stop_nsec-start_nsec;

    printf("circles.size = %d\n", circles.size());

    /*Calculate jitter*/
    val = (delta_time_sec*1000+delta_time_nsec/MSEC);
    jitter = hough_elip_deadline - val;
    printf("Hough Eclipse Jitter obtained is %ld ms\n\r", jitter);

    /*Release semaphore for next thread*/
    sem_post(&semCanny);
  }
  pthread_exit(NULL);
}

/* Print the current scheduling policy */
void print_scheduler(void)
{
   int schedType;

   schedType = sched_getscheduler(getpid());

   switch(schedType)
   {
     case SCHED_FIFO:
	   printf("Pthread Policy is SCHED_FIFO\n");
	   break;
     case SCHED_OTHER:
	   printf("Pthread Policy is SCHED_OTHER\n");
       break;
     case SCHED_RR:
	   printf("Pthread Policy is SCHED_OTHER\n");
	   break;
     default:
       printf("Pthread Policy is UNKNOWN\n");
   }
}

int main(int argc, char** argv)
{
	int dev=0;
	int rc, scope;

	/*Define gui window to open*/
	//cvNamedWindow( timg_window_name, CV_WINDOW_NORMAL);

	// Create a Trackbar for user to enter threshold
	//createTrackbar( "Min Threshold:", timg_window_name, &lowThreshold, max_lowThreshold, CannyThreshold );
    
	/* Select the camera device */
	capture = (CvCapture *)cvCreateCameraCapture(dev);
	
	/* Set the resolution */
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, HRES);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, VRES);

	printf("Before adjustments to scheduling policy:\n");
	print_scheduler();

	max_prio = sched_get_priority_max(SCHED_FIFO);
	min_prio = sched_get_priority_min(SCHED_FIFO);

	/*Initialise threads and attributes*/
	pthread_attr_init(&main_sched_attr);
	pthread_attr_setinheritsched(&main_sched_attr,PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&main_sched_attr,SCHED_FIFO);
	main_param.sched_priority=max_prio;

	pthread_attr_init(&canny_attr);
	pthread_attr_setinheritsched(&canny_attr,PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&canny_attr,SCHED_FIFO);
	canny_param.sched_priority=max_prio-1;

	pthread_attr_init(&hough_elip_attr);
	pthread_attr_setinheritsched(&hough_attr,PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&hough_attr,SCHED_FIFO);
	hough_param.sched_priority=max_prio-2;

	pthread_attr_init(&hough_attr);
	pthread_attr_setinheritsched(&hough_elip_attr,PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&hough_elip_attr,SCHED_FIFO);
	hough_elip_param.sched_priority=max_prio-3;

	/* Set scheduling policy */
	rc=sched_getparam(getpid(), &nrt_param);
	if (rc)
	{
		printf("ERROR; sched_setscheduler rc is %d\n", rc);
		perror(NULL);
		exit(-1);
	}

	rc=sched_setscheduler(getpid(),SCHED_FIFO,&main_param);
	if(rc)
	{
		printf("ERROR; main sched_setscheduler rc is %d\n",rc);
		perror(NULL);
		exit(-1);
	}

	printf("After adjustments to scheduling policy:\n");
	print_scheduler();

	pthread_attr_getscope(&canny_attr, &scope);

	if(scope == PTHREAD_SCOPE_SYSTEM)
	  printf("PTHREAD SCOPE SYSTEM\n");
	else if (scope == PTHREAD_SCOPE_PROCESS)
	  printf("PTHREAD SCOPE PROCESS\n");
	else
	  printf("PTHREAD SCOPE UNKNOWN\n");


	// initialize the signalling semaphores
	if (sem_init (&semCanny, 0, 1))
	{
	printf ("Failed to initialize sem_canny semaphore\n");
	exit (-1);
	}

	if (sem_init (&semHough, 0, 0))
	{
	printf ("Failed to initialize sem_hough semaphore\n");
	exit (-1);
	}

	if (sem_init (&semHoughElip, 0, 0))
	{
	printf ("Failed to initialize sem_hough_elip semaphore\n");
	exit (-1);
	}

	pthread_attr_setschedparam(&canny_attr, &canny_param);
	pthread_attr_setschedparam(&hough_attr, &hough_param);
	pthread_attr_setschedparam(&hough_elip_attr, &hough_elip_param);
	pthread_attr_setschedparam(&main_sched_attr, &main_param);

	printf("\n\rCreating threads\r\n");
	
	/* Create threads */
	if(pthread_create(&thread_canny, &canny_attr, canny_func, (void *)0)!=0){
		perror("ERROR; pthread_create:");
		exit(-1);
	}

	if(pthread_create(&thread_hough_elip, &hough_elip_attr, hough_elip_func, (void *)0)! =0){
		perror("ERROR; pthread_create:");
		exit(-1);
	}

	if(pthread_create(&thread_hough, &hough_attr, hough_func, (void *)0) !=0){
		perror("ERROR; pthread_create:");
		exit(-1);
	}

  printf("\n\rDone creating threads\r\n");
  printf("\n\n\n\rHorizontal resolution:%d and Vertical resolution:%d\n\r",HRES,VRES);

  /* Wait for threads to exit */
  pthread_join(thread_canny,NULL);
  pthread_join(thread_hough_elip,NULL);
  pthread_join(thread_hough,NULL);

  cvReleaseCapture(&capture);
  cvDestroyWindow("Capture Example");

  rc=sched_setscheduler(getpid(), SCHED_OTHER, &nrt_param);

  printf("All done\n");
}
