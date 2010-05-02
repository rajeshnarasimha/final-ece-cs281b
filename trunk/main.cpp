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

//Utils
#include <utils.h>

//Helper Functions
static inline void usage(void);
static void keyboardHandler(const int key);

//State variable of the program
static bool run = true;

//WindowName
static const char* display = "Video";
#ifdef VISUAL
static const char* diffWin  = "Difference";
#endif

//Entry Point!
int main(int argc, char** argv){
  if( argc != 3 ){
    usage();
    return 1;
  }

  /////////////////////////////////////////////////////
  //Loading Arguments
  IplImage*  bg_bgr  = cvLoadImage( argv[1] );
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

#ifdef VISUAL
  cvNamedWindow(diffWin);
#endif

  //Initializing Tracker
  imgutils::bgr2gray(bg_bgr, bundle.bg8u);
  
  //Main Loop
  while( frame && run ){
    cvShowImage(display, frame);
    keyboardHandler( cvWaitKey(rate) );

    //Background Subtraction
    imgutils::bgr2gray(frame, bundle.frame8u);
    imgutils::bg_subtract(bundle.frame8u, bundle.bg8u, bundle.diff);
    
#ifdef VISUAL
    //Visual Verbosing
    cvShowImage(diffWin, bundle.diff);
#endif

    frame = cvQueryFrame(capture);
  }//~End of While

  cvReleaseImage(&bg_bgr);
  cvReleaseCapture(&capture);

#ifdef VISUAL
  cvDestroyWindow(diffWin);
#endif

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
