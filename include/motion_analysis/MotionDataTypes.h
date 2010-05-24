/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <cv.h>

#ifndef __MOTION_DTYPES__
#define __MOTION_DTYPES__

//Structure to estimate the velocity according to the data
//provided in the video
struct motion_t{
  motion_t(double _t, const cv::Point2f& _pos){
    t = _t;
    pos.x = _pos.x;
    pos.y = _pos.y;
  }
  
  double t;         //Time?
  cv::Point2f pos;  //Position
};

#endif
