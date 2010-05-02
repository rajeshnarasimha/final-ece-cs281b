/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */
#ifndef __L1_MIN_TRACKER__
#define __L1_MIN_TRACKER__

#include <Tracker.h>

class L1MinTracker : public Tracker{
public:
  explicit L1MinTracker(const IplImage* T);
  ~L1MinTracker(void);
protected:
  IplImage* T;
};

#endif
