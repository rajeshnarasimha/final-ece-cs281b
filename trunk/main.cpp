/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

// //C++ Headers :: I/O
// #include <iostream>

// //OpenCV Headers
// #include <cv.h>
// #include <highgui.h>

// //Utils
// #include <utils.h>

// //
// #include <MeanShift.h>

// //Helper Functions
// static inline void usage(void);
// static void keyboardHandler(const int key);

// //State variable of the program
// static bool run = true;

// //WindowName
// static const char* display = "Video";
// #ifdef VISUAL
// static const char* diffWin  = "Difference";
// #endif

// //Entry Point!
// int main(int argc, char** argv){
//   if( argc != 3 ){
//     usage();
//     return 1;
//   }

//   /////////////////////////////////////////////////////
//   //Loading Arguments
//   IplImage*  bg_bgr  = cvLoadImage( argv[1] );
//   CvCapture* capture = cvCaptureFromFile( argv[2] );

//   if( !capture ){
//     std::cerr << "Unable to play Video" << std::endl;
//     return 1;
//   }

//   //Extracting info from video
//   int fps =  
//     static_cast<int>( cvGetCaptureProperty(capture, CV_CAP_PROP_FPS) );
//   fps = (fps == 1000 || fps == 500) ? 25 : fps;

//   int rate = 
//     static_cast<int>( ceil(1000.0 / ((fps == 1000 || fps == 500) ? 25 : fps)) );

// #ifdef DEBUG
//   std::cout << "<<DEBUG>> FPS: " << fps << std::endl;
// #endif

//   //Initializing Display
//   IplImage* frame = cvQueryFrame(capture);
//   ImagesBundle bundle(frame);
//   cvNamedWindow(display);

// #ifdef VISUAL
//   cvNamedWindow(diffWin);
// #endif

//   //Initializing Tracker
//   imgutils::bgr2gray(bg_bgr, bundle.bg8u);
//   MeanShift tracker(frame, 180, cvRect(0,0,frame->width, frame->height), 0.2, HSV);
  
//   //Main Loop
//   while( frame && run ){
//     cvShowImage(display, frame);
//     keyboardHandler( cvWaitKey(rate) );

//     //Background Subtraction
//     imgutils::bgr2gray(frame, bundle.frame8u);
//     imgutils::bg_subtract(bundle.frame8u, bundle.bg8u, bundle.diff);
    
// #ifdef VISUAL
//     //Visual Verbosing
//     cvShowImage(diffWin, bundle.diff);
// #endif

//     frame = cvQueryFrame(capture);
//   }//~End of While

//   cvReleaseImage(&bg_bgr);
//   cvReleaseCapture(&capture);

// #ifdef VISUAL
//   cvDestroyWindow(diffWin);
// #endif

//   cvDestroyWindow(display);

//   return 0;
// }

// void usage(void){
//   std::cout << "Usage: ./recog <BG.jpg> <VideoFile>" << std::endl;
// }

// void keyboardHandler(const int key){
//   switch(key){
//   case 'x': 
//   case 'X': run = false;
//   }
// }

#include <LucasKanade.h>
#include <MotionAnalyzer.h>
#include <utils.h>
#include <ClassifierWrapper.h>
#include <iostream>
#include <ml.h>

using namespace std;

static const char* display = "Display";
static void keyboardHandler(const int key);
static void mouseHandler(int event, int x, int y, int flags, void* param);

static bool run = true;
static bool roiSet = false;
static bool exitFlag = false;

static cv::Mat prev;
static cv::Mat next;

int main(int argc, char** argv){
  if( argc != 3 ){
    cout << argv[0] << " <videoFile> <bg>" << endl;
    return 1;
  }

  /////////////////////////////////////////////////////
  //Loading Arguments
  cv::VideoCapture capture( argv[1] );
  cv::Mat bg = cv::imread( argv[2] );

  if( !capture.isOpened() ){
    std::cerr << "Unable to play Video" << std::endl;
    return 1;
  }

  int fps =  
    static_cast<int>( capture.get( CV_CAP_PROP_FPS) );
  fps = (fps == 1000 || fps == 500) ? 25 : fps;

  int rate = 
    static_cast<int>( ceil(1000.0 / ((fps == 1000 || fps == 500) ? 25 : fps)) );

  int numFrames =
    static_cast<int>( capture.get( CV_CAP_PROP_FRAME_COUNT ) );

#ifdef DEBUG
  std::cout << "<<DEBUG>> FPS: "  << fps  << std::endl;
  std::cout << "<<DEBUG>> Rate: " << rate << std::endl;
  std::cout << "<<DEBUG>> NumFrames: " << numFrames << std::endl;
#endif

  cv::Mat frame;
  cv::Mat visualFrame;

  capture >> frame;
  frame.copyTo(visualFrame);

  cv::Rect roi( 0,0,0,0 );

  cv::namedWindow(display);
  cvSetMouseCallback(display,mouseHandler,&roi);

  std::cout << "Width:  " << frame.cols << std::endl;
  std::cout << "Height: " << frame.rows << std::endl;

  //Initializing Tracker
  LucasKanade* tracker = 0;

  //Using BG-Subtraction
  IplImage temp = frame;
  ImagesBundle bundle(&temp);
  cv::Mat frame_track = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC3);

  temp = bg;
  imgutils::bgr2gray(&temp, bundle.bg8u);

  cv::Mat t;

  ////////////////////////////////////////////////////////////
  //Pedestrian Detector
  // CvSVM svm;
  // svm.load("test.xml");
  // std::cout << "SVM Ready ... " << std::endl;
   ClassifierWrapper classifier("test.xml", 0, 0, 9, 180, 
                                5, ClassifierWrapper::PYR_HOG);
  //ClassifierWrapper classifier("SVMBicis.xml");
  ///////////////////////////////////////////////////////////
  //PHOG Descriptor
  //////////////////////////////////////////////////////////
  // PHOG phog(9, 180, 2);
  // cv::Mat descriptor;
  // int dimension = 0;

  //Main Loop
  cv::Mat maskedFrame;
  MotionAnalyzer manalyzer;

  cv::Mat resized = cv::Mat::zeros(128,64,CV_8UC1);

  while( true ){
    frame.copyTo(visualFrame);

    //Background Subtraction
    cv::Mat frameMat = frame;
    imgutils::bgr2gray(frameMat, bundle.frame8u);
    imgutils::bg_subtract(bundle.frame8u, bundle.bg8u, bundle.diff);

    //Extract Moving Object
    imgutils::maskFrameGray(frameMat, bundle.diff, maskedFrame);

    //Extract ROI of Blobs detected
    if( exitFlag ) break;

    //Set the Region to Track
    if( roiSet ){
      //Call Tracker

      roiSet = false;

      //Upscale?
      tracker = new LucasKanade(maskedFrame, maskedFrame, roi, 400, 
                                cv::Point(7,7), 0.0999, 
                                0.04, 1.0, HARRIS);
      
      cv::Rect rect = tracker->getLocation();
      cv::Mat f = visualFrame;
      cv::rectangle(f, cv::Point(rect.x, rect.y), 
                    cv::Point(rect.x + rect.width, rect.y + rect.height),
                    cv::Scalar(255.0, 0.0, 0.0));
      cv::Rect location = tracker->getLocation();
      manalyzer.collectCentroidSample( cv::Point2f(location.x, location.y),
                                       capture.get(CV_CAP_PROP_POS_MSEC) );
    }else{
      std::vector<cv::Rect> rois;
      imgutils::detectBlobs(maskedFrame, rois, cv::Point(1,5), 100, 12000);
      //std::cout << "Blobs: " << rois.size() << std::endl;
      cv::Mat f = visualFrame;

      cv::Mat blob_rgb;
      for( std::vector<cv::Rect>::const_iterator blob = rois.begin(); 
           blob != rois.end(); blob++){
        
        cv::Mat(f, (*blob)).copyTo(blob_rgb);

        imgutils::bgr2gray(blob_rgb, resized);
        imgutils::resize64x128(resized, resized, cv::Rect(0,0,64,128));

        //Call the Detector
        // float* desc = phog.computeDescriptor(resized, dimension, PHOG::L2NORM);
        // descriptor = cv::Mat(1, dimension, CV_32F, desc);
        // float guess = svm.predict(descriptor.row(0));
        float guess = classifier.predict(resized);

        if( guess == 1.0f ){
          cv::rectangle(f, cv::Point((*blob).x, (*blob).y),
                        cv::Point((*blob).x + (*blob).width, 
                                  (*blob).y + (*blob).height), 
                        cv::Scalar(255.0, 0.0, 0.0) );
        }else if( guess == -1.0f ){
          cv::rectangle(f, cv::Point((*blob).x, (*blob).y),
                        cv::Point((*blob).x + (*blob).width, 
                                  (*blob).y + (*blob).height), 
                        cv::Scalar(0.0, 0.0, 255.0) );
        }
        
        
      }

    }


    //If tracking
    if( tracker && tracker->state == TRACKING ){
      cv::Mat f = visualFrame;
      cv::Rect location = tracker->track(maskedFrame);
      cv::rectangle(f, 
                    cv::Point(location.x, location.y), 
                    cv::Point(location.x + location.width, 
                              location.y + location.height),
                    cv::Scalar(255.0, 0.0, 0.0));
      
      rate = 2;

      if( run ){
        manalyzer.collectCentroidSample( cv::Point2f(location.x, location.y),
                                         capture.get(CV_CAP_PROP_POS_MSEC) );
        manalyzer.drawTrajectory(visualFrame);
        cv::Point2f v = manalyzer.getAverageVelocity();
        std::cout << "Vel: " << std::fixed << v.x << " , " 
                  << std::fixed << v.y << std::endl;
      }
    }

    //cvShowImage(display, visualFrame);
    cv::imshow(display, visualFrame);
    keyboardHandler( cv::waitKey(rate) );

#ifdef DEBUG
    // float posMsec   = capture.get(CV_CAP_PROP_POS_MSEC);
    // int posFrames   = static_cast<int>( capture.get(CV_CAP_PROP_POS_FRAMES) );
    // std::cout << "PosMSec: "   <<  std::fixed << posMsec<< std::endl;
    // std::cout << "PosFrames: " <<  posFrames  << std::endl;
#endif

    if( run ){ 
      capture >> frame;
    }

  }//~End of While

  delete tracker;

  return 0;
}

void keyboardHandler(const int key){
  switch( (char) key ){
  case 'p':
    run = !run;
    break;
  case 'q':
    exitFlag = true;
    break;
  }
}

static void mouseHandler(int event, int x, int y, int flags, void* param){
  cv::Rect* roi = (cv::Rect*)param;

  switch( event ){
  case CV_EVENT_LBUTTONDOWN:
    roi->x = x;
    roi->y = y;
    break;
  case CV_EVENT_LBUTTONUP:
    roi->width = x - roi->x; //Assuming positive selection
    roi->height= y - roi->y; //Assuming positive selection
    roiSet = true;
    std::cout << "ROI Set" << std::endl;
    break;
  }
}
