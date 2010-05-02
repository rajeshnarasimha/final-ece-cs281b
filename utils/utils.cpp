/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <utils.h>

void imgutils::bgr2gray(const IplImage* bgr, IplImage* gray){
  cvCvtColor(bgr, gray, CV_BGR2GRAY);
}

void imgutils::bg_subtract(const IplImage* frame, const IplImage* bg, 
                           IplImage* dst){
  cvAbsDiff(frame, bg, dst);
  cvThreshold(dst, dst, 64.0, 255.0, CV_THRESH_BINARY);
  cvSmooth(dst, dst, CV_MEDIAN, 3);
}
