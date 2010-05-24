#include <LucasKanade.h>
#include <utils.h>
#include <iostream>

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
  //CvCapture* capture = cvCaptureFromFile( argv[1] );
  //IplImage* bg = cvLoadImage(argv[2]);

  //  if( !capture ){
  if( !capture.isOpened() ){
    std::cerr << "Unable to play Video" << std::endl;
    return 1;
  }

  //Extracting info from video
  // int fps =  
  //   static_cast<int>( cvGetCaptureProperty(capture, CV_CAP_PROP_FPS) );
  // fps = (fps == 1000 || fps == 500) ? 25 : fps;

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
  //Main Loop
  cv::Mat maskedFrame;
  while( true ){
    frame.copyTo(visualFrame);

    //Background Subtraction
    cv::Mat frameMat = frame;
    imgutils::bgr2gray(frameMat, bundle.frame8u);
    imgutils::bg_subtract(bundle.frame8u, bundle.bg8u, bundle.diff);
    
    //Extract Moving Object
    imgutils::maskFrameGray(frameMat, bundle.diff, maskedFrame);

    if( exitFlag ) break;

    //Set the Region to Track
    if( roiSet ){
      //Call Tracker

      roiSet = false;

      //Upscale?
      // cv::Mat(maskedFrame).copyTo(prev);
      //prev.copyTo(next);
      
      tracker = new LucasKanade(maskedFrame, maskedFrame, roi, 400, 
                                cv::Point(7,7), 0.0999, 
                                0.04, 1.0, HARRIS);

      cv::Rect rect = tracker->getLocation();
      cv::Mat f = visualFrame;
      cv::rectangle(f, cv::Point(rect.x, rect.y), 
                    cv::Point(rect.x + rect.width, rect.y + rect.height),
                    cv::Scalar(255.0, 0.0, 0.0));

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
    }

    //cvShowImage(display, visualFrame);
    cv::imshow(display, visualFrame);
    keyboardHandler( cv::waitKey(rate) );

#ifdef DEBUG
    float posMsec   = capture.get(CV_CAP_PROP_POS_MSEC);
    int posFrames   = static_cast<int>( capture.get(CV_CAP_PROP_POS_FRAMES) );
    std::cout << "PosMSec: "   <<  std::fixed << posMsec<< std::endl;
    std::cout << "PosFrames: " <<  posFrames  << std::endl;
#endif

    if( run ){ 
      //frame = cvQueryFrame(capture); 
      capture >> frame;
    }

  }//~End of While

  //cvReleaseCapture(&capture);
  //cvReleaseImage(&visualFrame);
  
  //cvDestroyWindow(display);
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
