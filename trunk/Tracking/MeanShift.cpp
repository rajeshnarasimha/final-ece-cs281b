/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <MeanShift.h>
#include <utils.h>

using namespace cv;

#include <iostream>

MeanShift::MeanShift(const IplImage* _T, 
                     const unsigned int _bins,
                     const Rect& _roi,
                     const Size& gaussianWinSize,
                     const cv::Scalar& _hsmin, 
                     const cv::Scalar& _hsmax,
                     const double& _sigma,
                     const double& thmin, const double& thmax,
                     const ColorHistogram hist) : 
  Tracker(_T, _roi), bins(_bins), histType(hist),
  winSize(gaussianWinSize), sigma(_sigma) {

  confidenceMap = Mat::zeros(T->height, T->width, CV_8UC1);
  blurConfidenceMap = false;

  //Setting Thresholds
  setThMin(thmin);
  setThMax(thmax);

  //Range intervals for HSV space
  hsmin = _hsmin;
  hsmax = _hsmax;
  
  //Settingup Masks
  maskROI   = Mat::zeros(T->height, T->width, CV_8UC1);
  
  buildHistogram(T);
  computeBackProjection(T);

  state = TRACKING;

  //Blurring ConfidenceMap
  if( blurConfidenceMap ){
    GaussianBlur(confidenceMap, confidenceMap, winSize, sigma);
  }

  //Computing CamShift
  location = CamShift(confidenceMap, roi, 
                      cv::TermCriteria( cv::TermCriteria::EPS | 
                                        cv::TermCriteria::COUNT, 
                                        15, 1 ));

  //Is location containing a good area?
  Rect b = location.boundingRect();
  if( b.width * b.height < 9 ){
    state = LOST;
  }

  roi = location.boundingRect();
}

MeanShift::~MeanShift(void){
  
}



CvRect MeanShift::track(const Mat& frame){
  IplImage t = frame;
  return track(&t);
}

CvRect MeanShift::track(const IplImage* frame){
  computeBackProjection(frame);

  //Blurring the ConfidenceMap
  if( blurConfidenceMap ){
    GaussianBlur(confidenceMap, confidenceMap, winSize, sigma);
  }

  //Tracking with CamShift
  //imshow("Map", confidenceMap);
  location = CamShift(confidenceMap, roi, 
                      cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 
                                      10, 1 ));

  //Is location containing a good area?
  Rect b = location.boundingRect();
  if( b.width * b.height < 9 ){
    state = LOST;
  }

  //Update ROI
  roi = location.boundingRect();

  //ROI always less than the frame size
  roi.x = roi.x > 0 ? std::min(frame->width - 1, roi.x) : 0;
  roi.y = roi.y > 0 ? std::min(frame->height - 1, roi.y) : 0;

  roi.width  = roi.x + roi.width  < frame->width  ? 
    roi.width  : frame->width - roi.x - 1;
  roi.height  = roi.y + roi.height < frame->height ? 
    roi.height : frame->height - roi.y - 1;

  return roi;
}


void MeanShift::buildHistogram(const IplImage* patch){
  mask = Mat::zeros(patch->height, patch->width, CV_8UC1);

  switch( histType ){
  case RGB:
    imgutils::buildRGBHistogram(patch, histogram, bins, mask);
    break;
  case HSV:
    imgutils::buildHsvHistogram(patch, histogram, bins, mask, hsmin, hsmax, roi);
    break;
  case HUE:
    imgutils::buildHueHistogram(patch, histogram, bins, mask, hsmin, hsmax, roi);
    break;
  };
}

void MeanShift::computeBackProjection(const IplImage* T){
  switch( histType ){
  case RGB:
    imgutils::computeBackProjectionRGB(T, histogram, confidenceMap);
    break;
  case HSV:
    imgutils::computeBackProjectionHSV(T, histogram, confidenceMap,
                                       mask, hsmin, hsmax);
    break;
  case HUE:
    imgutils::computeBackProjectionHue(T, histogram, confidenceMap,
                                       mask, hsmin, hsmax);
    break;
  };  

  //Masking considering Hue-[Saturation]
  bitwise_and(confidenceMap, mask, confidenceMap);

  //Mask the backprojection
  maskROI = Scalar(0,0,0); 

  // int w = roi.width >> 1;
  // int h = roi.height>> 1;

  // Rect r = state == TRACKING ? 
  //   Rect( std::max( int(location.center.x) - w,  0), 
  //         std::max( int(location.center.y) - h,  0), 
  //         w << 1, h << 1 ) : roi;

  Mat mRoi(maskROI, roi);
  mRoi = Scalar(0xff, 0xff, 0xff);

  bitwise_and(confidenceMap, maskROI, confidenceMap);

  threshold(confidenceMap, confidenceMap, thMin, 255.0, THRESH_TOZERO);
  threshold(confidenceMap, confidenceMap, thMax, 255.0, THRESH_TOZERO_INV);
}

