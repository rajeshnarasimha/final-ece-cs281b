/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

//C++ Headers :: I/O
#include <iostream>

//OpenCV Headers
#include <cv.h>
#include <highgui.h>

//
#include <utils.h>

//Helper Functions
static inline void usage(void);
static void keyboardHandler(const int key);

//State variable of the program
static bool run = true;

static const char* display = "Video";

int main(int argc, char** argv){
  if( argc != 3 ){
    usage();
    return 1;
  }

  /////////////////////////////////////////////////////
  //Loading Arguments
  IplImage*  bg_rgb  = cvLoadImage( argv[1] );
  CvCapture* capture = cvCaptureFromFile( argv[2] );

  if( !capture ){
    std::cerr << "Unable to play Video" << std::endl;
    return 1;
  }

  //Extracting info from video
  int fps =  
    static_cast<int>( cvGetCaptureProperty(capture, CV_CAP_PROP_FPS) );
  fps = (fps == 1000 || fps == 500) ? 25 : fps;

  int rate = 
    static_cast<int>( ceil(1000.0 / ((fps == 1000 || fps == 500) ? 25 : fps)) );

  //Initializing Display
  IplImage* frame = cvQueryFrame(capture);
  ImagesBundle bundle(frame);
  cvNamedWindow(display);

  //Initializing Tracker
  
  //Main Loop
  while( frame && run ){
    cvShowImage(display, frame);
    keyboardHandler( cvWaitKey(rate) );

    frame = cvQueryFrame(capture);
  }//~End of While

  cvReleaseImage(&bg_rgb);
  cvReleaseCapture(&capture);

  cvDestroyWindow(display);

  return 0;
}

void usage(void){
  std::cout << "Usage: ./recog <BG.jpg> <VideoFile>" << std::endl;
}

void keyboardHandler(const int key){
  switch(key){
  case 'x': 
  case 'X': run = false;
  }
}
