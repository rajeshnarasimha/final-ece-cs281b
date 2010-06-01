/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#ifndef __CLASSIFIER_WRAPPER__
#define __CLASSIFIER_WRAPPER__

#include <PHOG.h>
#include <hog.h>
#include <ml.h>
#include <string>

class ClassifierWrapper{
public:
  enum DescriptorType{
    HOG, 
    PYR_HOG
  };
  
  explicit ClassifierWrapper(const std::string& xml,
                             const int cellsPerBlock=4,
                             const int pixelsPerCells=8,
                             const int bins=9, 
                             const int maxAngle=180,
                             const int l=3, //Pyramid Levels only if PHOG
                             enum DescriptorType descriptor_type=HOG);
  
  ~ClassifierWrapper(void);
  
  float predict(const cv::Mat& img);
  
private:
  //Attributes
  const int cellsPerBlock;
  const int pixelsPerCells;
  const int bins;
  const int maxAngle;
  const int l;

  CvSVM svm;
  int descriptorType;
  PHOG* phog;
  
  float hogPredict( const cv::Mat& img );
  float phogPredict( const cv::Mat& img );
};

#endif
