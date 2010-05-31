/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#ifndef __HOG_CLASS__
#define __HOG_CLASS__

#include <cv.h>
#include <highgui.h>
#include <vector>

class PHOG{
public:
  enum{ L1NORM, L2NORM, L2NORM_BLOCK};

  PHOG(const int bins,
       const int maxAngle,
       const int l);

  ~PHOG(void);

  float* computeDescriptor(const cv::Mat& img, 
                           int& dimension, 
                           int norm=L1NORM){
    return computeDescriptor(img, dimension, 
                             cv::Rect(0,0,img.cols, img.rows));
  }
  
  float* computeDescriptor(const cv::Mat& img, 
                           int& dimension, 
                           const cv::Rect& roi,
                           int norm=L1NORM);
  
  void computeDescriptor(const std::vector<cv::Mat>& imgs,
                         std::vector<float*>& descriptors,
                         int& dimension, int norm=L1NORM);
  
  
private:
  const int bins;
  const int maxAngle;
  const int l;
  cv::Size imgSize;
  //int dimension;
  
  void computeGradient( const cv::Mat& img, 
                        cv::Mat& dx, cv::Mat& dy );
  
  void computePhasor(const cv::Mat& dx, const cv::Mat& dy, 
                     cv::Mat& bin, 
                     cv::Mat& angle, 
                     cv::Mat& magn);
  
  float* computePyramid(const cv::Mat& angle, 
                        const cv::Mat& magn, 
                        const cv::Mat& bin,
                        int& dim);
  
  float* computeHistogramPerBlock(const cv::Mat& angle, 
                                  const cv::Mat& magn,
                                  const cv::Mat& bin,
                                  const cv::Rect& roi);
  
  void normalizeHistogram(float* histogram, const int size, int norm);
};

#endif
