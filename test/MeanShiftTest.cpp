#include <MeanShift.h>
#include <utils.h>
#include <iostream>

using namespace std;

static const char* display = "Display";
static void keyboardHandler(const int key);
static void mouseHandler(int event, int x, int y, int flags, void* param);

static bool run = true;
static bool roiSet = false;
static bool exitFlag = false;

int main(int argc, char** argv){
  if( argc != 3 ){
    cout << argv[0] << " <videoFile> <bg>" << endl;
    return 1;
  }

  /////////////////////////////////////////////////////
  //Loading Arguments
  CvCapture* capture = cvCaptureFromFile( argv[1] );
  IplImage* bg = cvLoadImage(argv[2]);

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

#ifdef DEBUG
  std::cout << "<<DEBUG>> FPS: " << fps << std::endl;
#endif

  //Initializing Display
  IplImage* frame       = cvQueryFrame(capture);
  IplImage* visualFrame = cvCloneImage(frame);
  CvRect roi = cvRect( 0,0,0,0 );
  cvNamedWindow(display);
  cvSetMouseCallback(display,mouseHandler,&roi);

  std::cout << "Width:  " << frame->width << std::endl;
  std::cout << "Height: " << frame->height<< std::endl;

  //Initializing Tracker
  MeanShift* tracker = 0;

  //Using BG-Subtraction
  ImagesBundle bundle(frame);
  cv::Mat frame_track = cv::Mat::zeros(frame->height, frame->width, CV_8UC3);

  imgutils::bgr2gray(bg, bundle.bg8u);

  //Main Loop
  cv::Mat maskedFrame;
  while( frame ){
    cvCopy(frame, visualFrame);      

    //Background Subtraction
    imgutils::bgr2gray(frame, bundle.frame8u);
    imgutils::bg_subtract(bundle.frame8u, bundle.bg8u, bundle.diff);
    
    //Extract Moving Object
    imgutils::maskFrameRGB(frame, bundle.diff, maskedFrame);

    if( exitFlag ) break;

    //Set the Region to Track
    if( roiSet ){
      //Call Tracker
      tracker = new MeanShift(frame, 16, roi, cv::Size(7,7),
                              cv::Scalar(  0.0,  5.0,   5.0), 
                              cv::Scalar(180.0, 250.0, 250.0), 3.5, 
                              10.0, 250.0, HUE);
      roiSet = false;
      
      cv::RotatedRect loc = tracker->getLocation();
      cv::Rect rect = loc.boundingRect();
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

    cvShowImage(display, visualFrame);
    keyboardHandler( cvWaitKey(rate) );

    if( run ){ 
      frame = cvQueryFrame(capture); 
    }

  }//~End of While

  cvReleaseCapture(&capture);
  cvReleaseImage(&visualFrame);
  
  cvDestroyWindow(display);
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
  CvRect* roi = (CvRect*)param;

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
