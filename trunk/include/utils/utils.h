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
#include <iostream>

struct ImagesBundle{
  ImagesBundle(const cv::Mat& frame){
    cv::cvtColor(frame, frame8u, CV_8UC1);
  }

  ~ImagesBundle(void){
  }

  cv::Mat frame8u; //Frame GrayScale
  cv::Mat bg8u;    //Background GrayScale
  cv::Mat diff;
  cv::Mat mask;
};


struct HistogramParams{
  int* histSize;
  float** ranges;
  int* channels;
};

namespace imgutils{
  inline
  void bgr2gray(const IplImage* bgr, IplImage* gray){
    cvCvtColor(bgr, gray, CV_BGR2GRAY);
  }
  
  void maskFrameRGB(const cv::Mat& frame, 
                    const cv::Mat& src2, 
                    cv::Mat& maskedFrame);

  void maskFrameGray(const cv::Mat& frame, 
                     const cv::Mat& src2, 
                     cv::Mat& maskedFrame);

  void maskFrameGray(const cv::Mat& frame, 
                     const cv::Rect& roi, 
                     cv::Mat& maskedFrame);

  inline
  void bgr2gray(const cv::Mat& bgr, cv::Mat& gray){
    // IplImage* gray = cvCreateImage(cvSize(bgr->width, bgr->height),
    //                                IPL_DEPTH_8U, 1);
    // cvCvtColor(bgr, gray, CV_BGR2GRAY);
    cv::cvtColor(bgr, gray, CV_BGR2GRAY);
  }

  inline 
  void bg_subtract(const IplImage* frame, const IplImage* bg, 
                   IplImage* dst){
    cvAbsDiff(frame, bg, dst);
    cvThreshold(dst, dst, 65.0, 255.0, CV_THRESH_BINARY);
    cvSmooth(dst, dst, CV_MEDIAN, 3);
  }

  inline 
  void bg_subtract(const cv::Mat& frame, const cv::Mat& bg, 
                   cv::Mat& dst){
    dst = cv::abs( frame - bg );
    cv::threshold(dst, dst, 65.0, 255.0, CV_THRESH_BINARY);
    cv::medianBlur(dst, dst, 3);
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

  inline void dumpRectangle( const cv::Rect& r){
    std::cout << "X: " << r.x << std::endl;
    std::cout << "Y: " << r.y << std::endl;
    std::cout << "Width: " << r.width   << std::endl;
    std::cout << "Height: " << r.height << std::endl;
  }

  //Helpef Functions for Tracking
  void buildRGBHistogram(const IplImage* patch, 
                         cv::MatND& histogram,
                         const int& bins,
                         cv::Mat& mask);
  
  void buildHsvHistogram(const IplImage* patch, 
                         cv::MatND& histogram,
                         const int& bins,
                         cv::Mat& mask,
                         const cv::Scalar& hsmin, 
                         const cv::Scalar& hsmax,
                         const cv::Rect& roi);
  
  void buildHueHistogram(const IplImage* patch, 
                         cv::MatND& histogram,
                         const int& bins,
                         cv::Mat& mask,
                         const cv::Scalar& hsmin, 
                         const cv::Scalar& hsmax,
                         const cv::Rect& roi);

  void computeBackProjectionRGB(const cv::Mat& T,
                                const cv::MatND& histogram, 
                                cv::Mat& backProj);

  void computeBackProjectionHSV(const cv::Mat& T,
                                const cv::MatND& histogram, 
                                cv::Mat& backProj,
                                cv::Mat& mask,
                                const cv::Scalar& hsmin,
                                const cv::Scalar& hsmax);
  
  void computeBackProjectionHue(const cv::Mat& T,
                                const cv::MatND& histogram, 
                                cv::Mat& backProj,
                                cv::Mat& mask,
                                const cv::Scalar& hsmin,
                                const cv::Scalar& hsmax);

  void findSurroundingBox(const std::vector<cv::Point2f>& corners, 
                          const std::vector<unsigned char>& status,
                          cv::Rect& roi, const cv::Size& dim,
                          const cv::Point& compensation);

  void detectBlobs( cv::Mat& frame, std::vector<cv::Rect>& rois, 
                    const cv::Point& offset,
                    const int areaThreshold );

}

#endif
