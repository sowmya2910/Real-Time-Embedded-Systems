/****************************************************************************************************
* Author - Virag Gada, Sowmya Ramakrishnan
* Reference examples from http://mercury.pr.erau.edu/~siewerts/cs415/code/computer_vision_cv3_tested/
*           - example-stereo-transform-improved
*           - simple-canny-interactive
*           - simple-hough-eliptical-interactive
****************************************************************************************************/

/*
 *
 *  Example by Sam Siewert  - modified for dual USB Camera capture to use to
 *                            experiment and learn Stereo vision concepts pairing with
 *                            "Learning OpenCV" by Gary Bradski and Adrian Kaehler
 *
 *  For more advanced stereo vision processing for camera calibration, disparity, and
 *  point-cloud generation from a left and right image, see samples directory where you
 *  downloaded and built Opencv latest: e.g. opencv-2.4.7/samples/cpp/stero_match.cpp
 *
 *  ADDED:
 *
 *  1) 4th argument of "t" for transform [Canny and Hough Linear] or "d" for disparity
 *
 *  2) 4th argument is "h" for Hough Linear transform and otherwise "c" for Canny
 *
 *  NOTE: Uncompressed YUV at 640x480 for 2 cameras is likely to exceed
 *        your USB 2.0 bandwidth available.  The calculation is:
 *        2 cameras x 640 x 480 x 2 bytes_per_pixel x 30 Hz = 36000 KBytes/sec
 *
 *        About 370 Mbps (assuming 8b/10b link encoding), and USB 2.0 is 480
 *        Mbps at line rate with no overhead.
 *
 *        So, for full performance with 2 cameras, drop resolution down to 320x240.
 *
 *        With a single camera I had no problem running 1280x720 (720p).
 *
 *        I tested with really old Logitech C200 and newer C270 webcams, both are well
 *        supported and tested with the Linux UVC driver - http://www.ideasonboard.org/uvc
 *
 *        Use with a native Linux installation and any UVC driver camera.  You can
 *        verify that UVC is loaded with "lsmod | grep uvc" after you plug in your
 *        camera (modprobe if you need to) and you can verify a USB camera's link wiht
 *        "lsusb" and "dmesg" after plugging it in.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/contrib/contrib.hpp"

using namespace cv;
using namespace std;

//#define HRES_COLS (640)
//#define VRES_ROWS (480)

// Should always work for uncompressed USB 2.0 dual cameras
//#define HRES_COLS (320)
//#define VRES_ROWS (240)

#define HRES_COLS (160)
#define VRES_ROWS (120)

#define ESC_KEY (27)

char snapshotname[80] = "snapshot_xxx.jpg";

/* Canny transform parameters */
int lowThreshold = 0;
int const max_lowThreshold = 100;
int kernel_size = 3;
int edgeThresh = 1;
int ratio = 3;
Mat canny_frame,cdst, timg_gray, timg_grad;

// Transform display window
char timg_window_name[] = "Edge Detector Transform";

IplImage *frame;

/* Function to perform Canny transform based on threshold */
void CannyThreshold(int, void*)
{
    //Mat mat_frame(frame);
    Mat mat_frame(cvarrToMat(frame));

    cvtColor(mat_frame, timg_gray, CV_RGB2GRAY);

    /// Reduce noise with a kernel 3x3
    blur( timg_gray, canny_frame, Size(3,3) );

    /// Canny detector
    Canny( canny_frame, canny_frame, lowThreshold, lowThreshold*ratio, kernel_size );

    /// Using Canny's output as a mask, we display our result
    timg_grad = Scalar::all(0);

    mat_frame.copyTo( timg_grad, canny_frame);

    imshow( timg_window_name, timg_grad );

}

int main( int argc, char** argv )
{
    /* Timing parameters */
    double prev_frame_time;
    double curr_frame_time;
    struct timespec frame_time;

    /* Common parameters */
    CvCapture *capture;

    Mat disp,gray;
    int dev=0;

    /* For Hough Interactive */
    vector<Vec4i> lines;

    /* For Hough Eleptical Interactive */
    vector<Vec3f> circles;

    /* Switch Control */
    int applyCannyTransform = 0;
    int applyHoughTransform = 0;
    int applyHoughElliptical = 0;

    // used to compute running averages for single camera frame rates
    double ave_framedt=0.0, ave_frame_rate=0.0, fc=0.0, framedt=0.0;
    unsigned int frame_count=0;

    if(argc == 1)
    {
      printf("Will open DEFAULT video device video0\n");
      capture = cvCreateCameraCapture(0);
      cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, HRES_COLS);
      cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, VRES_ROWS);
    }

    else if(argc == 2)
    {
      printf("argv[1]=%s\n", argv[1]);
      printf("Will open DEFAULT video device video0\n");
      capture = cvCreateCameraCapture(0);
      cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, HRES_COLS);
      cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, VRES_ROWS);

	    /* Enable Transform based on user input */
      if(strncmp(argv[1],"c",1) == 0)
      {
      	applyCannyTransform = 1;
      }
      else if(strncmp(argv[1],"h",1) == 0)
      {
        applyHoughTransform= 1 ;
      }
      else if(strncmp(argv[1],"e",1) == 0)
      {
        applyHoughElliptical = 1;
      }
	  else
	  {
	    printf("Enter arguments: c- Canny h- Hough, e-Hough Elliptical\n\r");
  	  }

	  cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);

	  while(1)
	  {
		//if(cvGrabFrame(capture)) frame=cvRetrieveFrame(capture);
		frame=cvQueryFrame(capture); // short for grab and retrieve

		if(!frame) break;

		clock_gettime(CLOCK_REALTIME, &frame_time);
		curr_frame_time=((double)frame_time.tv_sec * 1000.0) +
						 ((double)((double)frame_time.tv_nsec / 1000000.0));
        
		/* Apply Canny Transform */
        if(applyCannyTransform)
        {
          CannyThreshold(0, 0);
        }
		/* Apply Hough Transform */
  	    else if(applyHoughTransform)
		{
		  //Mat mat_frame_l(frame_l);
		  Mat mat_frame(cvarrToMat(frame));
		  Canny(mat_frame, canny_frame, 50, 200, 3);
		  //Mat mat_frame_r(frame_r);

		  HoughLinesP(canny_frame, lines, 1, CV_PI/180, 50, 50, 10);

		  for( size_t i = 0; i < lines.size(); i++ )
		  {
			Vec4i l = lines[i];
			line(mat_frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
		  }
		}
		/* Apply Hough Elliptical transform */
		else if(applyHoughElliptical)
		{
			frame=cvQueryFrame(capture);
			if(!frame)
            break;

			Mat mat_frame(cvarrToMat(frame));

			cvtColor(mat_frame, gray, COLOR_BGR2GRAY);

			GaussianBlur(gray, gray, Size(9,9), 2, 2);

			HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, gray.rows/8, 100, 50, 0, 0);

			printf("Circles.size = %d\n", circles.size());

			for( size_t i = 0; i < circles.size(); i++ )
			{
			  Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			  int radius = cvRound(circles[i][2]);
			  // circle center
			  circle( mat_frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
			  // circle outline
			  circle( mat_frame, center, radius, Scalar(0,0,255), 3, 8, 0 );
			}

			imshow("Capture Example", mat_frame);
		}

		clock_gettime(CLOCK_REALTIME, &frame_time);
		prev_frame_time=((double)frame_time.tv_sec * 1000.0) +
						((double)((double)frame_time.tv_nsec / 1000000.0));

		frame_count++;

		if(frame_count > 2)
		{
			fc=(double)frame_count;
			ave_framedt=((fc-1.0)*ave_framedt + framedt)/fc;
			ave_frame_rate=1.0/(ave_framedt/1000.0);
		}

		cvShowImage("Capture Example", frame);
		printf("Frame @ %u sec, %lu nsec, dt=%5.2lf msec, avedt=%5.2lf msec, rate=%5.2lf fps\n",
				  (unsigned)frame_time.tv_sec,
				  (unsigned long)frame_time.tv_nsec,
				  framedt, ave_framedt, ave_frame_rate);

	    framedt = prev_frame_time - curr_frame_time;
	    prev_frame_time=curr_frame_time;

  	    // Set to pace frame capture and display rate
		char c = cvWaitKey(33);
		if(c == ESC_KEY)
		{
			sprintf(&snapshotname[9], "%8.4lf.jpg", curr_frame_time);
			cvSaveImage(snapshotname, frame);
		}
		else if(c == 'q')
		{
			printf("Got Quit\n");
			cvReleaseCapture(&capture);
			exit(0);
		}

	  }
	  cvReleaseCapture(&capture);
      cvDestroyWindow("Capture");
   	}
    else
    {
      cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);

      while(1)
      {
        frame = cvQueryFrame(capture); // short for grab and retrieve

        if(!frame)
          break;
        else
            {
                clock_gettime(CLOCK_REALTIME, &frame_time);
                curr_frame_time=((double)frame_time.tv_sec * 1000.0) +
                                ((double)((double)frame_time.tv_nsec / 1000000.0));
                frame_count++;

                if(frame_count > 2)
                {
                    fc=(double)frame_count;
                    ave_framedt=((fc-1.0)*ave_framedt + framedt)/fc;
                    ave_frame_rate=1.0/(ave_framedt/1000.0);
                }
            }

            cvShowImage("Capture Example", frame);
            printf("Frame @ %u sec, %lu nsec, dt=%5.2lf msec, avedt=%5.2lf msec, rate=%5.2lf fps\n",
                   (unsigned)frame_time.tv_sec,
                   (unsigned long)frame_time.tv_nsec,
                   framedt, ave_framedt, ave_frame_rate);

            // Set to pace frame capture and display rate
            char c = cvWaitKey(10);
            if(c == ESC_KEY)
            {
                sprintf(&snapshotname[9], "%8.4lf.jpg", curr_frame_time);
                cvSaveImage(snapshotname, frame);
            }
            else if(c == 'q')
            {
                printf("Got Quit\n");
                cvReleaseCapture(&capture);
                exit(0);
            }
            framedt=curr_frame_time - prev_frame_time;
            prev_frame_time=curr_frame_time;
        }

        cvReleaseCapture(&capture);
        cvDestroyWindow("Capture Example");
    }

}
