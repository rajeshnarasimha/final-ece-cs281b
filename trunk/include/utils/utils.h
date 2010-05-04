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
  inline
  void bgr2gray(const IplImage* bgr, IplImage* gray){
    cvCvtColor(bgr, gray, CV_BGR2GRAY);
  }
  
  inline
  void bg_subtract(const IplImage* frame, const IplImage* bg, 
                             IplImage* dst){
    cvAbsDiff(frame, bg, dst);
    cvThreshold(dst, dst, 64.0, 255.0, CV_THRESH_BINARY);
    cvSmooth(dst, dst, CV_MEDIAN, 3);
  }

  inline
  IplImage* bgr2gray(const IplImage* bgr){
    IplImage* gray = cvCreateImage(cvSize(bgr->width, bgr->height),
                                   IPL_DEPTH_8U, 1);
    cvCvtColor(bgr, gray, CV_BGR2GRAY);
    return gray;
  }


  inline 
  double dotProduct(const cv::Scalar& a, const cv::Scalar& b){
    return (a.val[0] * b.val[0] + 
            a.val[1] * b.val[1] +
            a.val[2] * b.val[2] +
            a.val[3] * b.val[3] );
  }

  inline
  double l2norm(const cv::Scalar& a){
    return sqrt(dotProduct(a,a));
  }

  inline
  cv::Scalar difference(const cv::Scalar& a, const cv::Scalar& b){
    cv::Scalar c;
    c.val[0] = a.val[0] - b.val[0]; 
    c.val[1] = a.val[1] - b.val[1]; 
    c.val[2] = a.val[2] - b.val[2]; 
    c.val[3] = a.val[0] - b.val[3];     
    return c;
  }

  inline 
  double euclideanDistance(const cv::Scalar& a, const cv::Scalar& b){
    cv::Scalar c = difference(a,b);
    return dotProduct(c,c);
  }
}

#endif
