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

class Tracker{
public:
  explicit Tracker(const IplImage* T);
  virtual ~Tracker(void);
protected:
  IplImage* T;
};

#endif
