/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <Tracker.h>

Tracker::Tracker(const IplImage* _T){
  T = cvCloneImage(_T);
}

Tracker::~Tracker(void){
  if( T ){
    cvReleaseImage(&T);
  }
}
