/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#ifndef __TRACKER__
#define __TRACKER__

#include <cv.h>
#include <highgui.h>

enum TrackingState{
  IDLE,
  TRACKING,
  LOST
};

class Tracker{
public:
  explicit Tracker(const IplImage* T, const cv::Rect& roi);
  virtual ~Tracker(void);

  enum TrackingState state;

  //Functions
  virtual CvRect track(const IplImage* frame) = 0;
protected:
  IplImage* T;
  cv::Rect roi;
};

#endif
