/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */
#ifndef __UTILS__
#define __UTILS__

#include <cv.h>
#include <highgui.h>

struct ImagesBundle{
  ImagesBundle(IplImage* frame){
    frame8u = cvCreateImage(cvSize(frame->width, frame->height), 
                            IPL_DEPTH_8U, 1);

    bg8u = cvCreateImage(cvSize(frame->width, frame->height), 
                         IPL_DEPTH_8U, 1);

    diff = cvCreateImage(cvSize(frame->width, frame->height), 
                         IPL_DEPTH_8U, 1);
  }

  ~ImagesBundle(void){
    cvReleaseImage(&frame8u);
    cvReleaseImage(&bg8u);
    cvReleaseImage(&diff);
  }

  IplImage* frame8u; //Frame GrayScale
  IplImage* bg8u;    //Background GrayScale
  IplImage* diff;
};

namespace imgutils{
  //inline 
  void bgr2gray(const IplImage* bgr, IplImage* gray);
  
  //inline 
  void bg_subtract(const IplImage* frame, const IplImage* bg, IplImage* dst);

}

#endif
