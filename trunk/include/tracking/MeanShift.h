/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */
#ifndef __MEAN_SHIFT__
#define __MEAN_SHIFT__

#include <Tracker.h>

enum ColorHistogram{
  RGB, //RGB Histogram
  HSV, //Hue-Saturation Histogram
  HUE //Hue Histogram
  //BLOB //Using just a Blob
};

class MeanShift : public Tracker{
public:
  MeanShift(const IplImage* T, 
            const unsigned int bins,
            const cv::Rect& roi,
            const cv::Size& gaussianWinSize,
            const cv::Scalar& hsmin, 
            const cv::Scalar& hsmax,
            const double& sigma,
            const double& thMin, const double& thMax,
            const ColorHistogram hist=HUE);

  virtual ~MeanShift(void);

  cv::RotatedRect getLocation( void ){ return location; }
  cv::RotatedRect getLocation( void ) const { return location; }

  CvRect track(const IplImage* frame);
  CvRect track(const cv::Mat& frame);

  //Setters/Getters
  void setThMin( const double& thmin ){
    thMin = thmin < 0 ? 0 : thmin > 255 ? 255 : thmin;
  }

  double getThMin(void){
    return thMin;
  }

  void setThMax( const double& thmax ){
    thMax = thmax < 0 ? 0 : thmax > 255 ? 255 : thmax;
  }

  double getThMax(void){
    return thMax;
  }

  void setBlurConfidenceMap( bool flag ){
    blurConfidenceMap = flag;
  }

private:
  //Attributes
  const unsigned int bins;
  const enum ColorHistogram histType;
  bool blurConfidenceMap;

  //Histogram
  cv::MatND histogram;
  cv::Mat confidenceMap;
  cv::Mat hsv;

  //Masking
  cv::Mat mask;
  cv::Mat maskROI;

  //Blurring Window
  const cv::Size winSize;
  const double sigma;

  //Threshold Values for ConfidenceMap
  double thMin;
  double thMax;

  //H-S Range
  cv::Scalar hsmin;
  cv::Scalar hsmax;

  //Mean
  cv::RotatedRect location;

  //~ Member functions
  void buildHistogram(const IplImage* patch);

  //~ Compute Mean
  void computeBackProjection(const IplImage* patch);
};

#endif
