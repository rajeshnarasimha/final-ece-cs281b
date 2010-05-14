/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <Tracker.h>

Tracker::Tracker(const cv::Mat& _T, const cv::Rect& _roi){
  IplImage t = _T;
  T = cvCloneImage(&t);
  roi   = _roi;
  state = IDLE;
}

Tracker::Tracker(const IplImage* _T, const cv::Rect& _roi){
  T   = cvCloneImage(_T);
  cv::Mat t = T;
  //cv::GaussianBlur(t, t, cv::Size(5,5), 0.0);
  roi = _roi;
  state = IDLE;
}

void Tracker::releaseTemplate(void){
  if( T ){
    cvReleaseImage(&T);
    T = 0;
  }
}

Tracker::~Tracker(void){
  releaseTemplate();
}

CvRect Tracker::track(const cv::Mat& frame){
  static CvRect c = cvRect(0,0,0,0);
  return c;
}

CvRect Tracker::track(const IplImage* patch){
  static CvRect c = cvRect(0,0,0,0);
  return c;
}
