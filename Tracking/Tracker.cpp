/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <Tracker.h>

Tracker::Tracker(const IplImage* _T, const cv::Rect& _roi){
  T   = cvCloneImage(_T);
  roi = _roi;
  state = IDLE;
}

Tracker::~Tracker(void){
  if( T ){
    cvReleaseImage(&T);
  }
}

CvRect Tracker::track(const IplImage* patch){
  static CvRect c = cvRect(0,0,0,0);
  return c;
}
