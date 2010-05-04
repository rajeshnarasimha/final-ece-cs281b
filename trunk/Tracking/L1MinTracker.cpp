#include <L1MinTracker.h>

L1MinTracker::L1MinTracker(const IplImage* T, const cv::Rect& roi) : 
  Tracker(T, roi){
  
}

L1MinTracker::~L1MinTracker(void){
  
}

CvRect L1MinTracker::track(const IplImage* frame){
  return cvRect(0,0,0,0);
}

