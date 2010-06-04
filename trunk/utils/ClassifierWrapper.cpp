/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <ClassifierWrapper.h>

ClassifierWrapper::ClassifierWrapper(const std::string& xml,
                                     const int _cellsPerBlock,
                                     const int _pixelsPerCells,
                                     const int _bins, 
                                     const int _maxAngle,
                                     const int _l, //Pyramid Levels only if PHOG
                                     enum DescriptorType _descriptorType,
                                     enum MLType mlType) : 
  cellsPerBlock( _cellsPerBlock), pixelsPerCells(_pixelsPerCells), 
  bins(_bins), maxAngle(_maxAngle), l(_l)
{
  svm.load( xml.c_str() );
  descriptorType = _descriptorType;
  
  switch( descriptorType ){
  case HOG:
    {
      //Init HOG
      phog = 0;
    }
    break;
  case PYR_HOG:
    {
      phog = new PHOG(bins, maxAngle, l); //Using dx = [-1 0 1]
    }
    break;
  default:
    return;
  }

}

ClassifierWrapper::~ClassifierWrapper( void ){ 
  if( phog ){
    delete phog;
  }
}

void ClassifierWrapper::trainKNN(const std::string& trainingSet, 
                                 const std::string& labels, int k){
  //knn.train();
}

float ClassifierWrapper::predict( const cv::Mat& img ){
  float prediction = 0.0;
  int dimension = 0;
  float* desc = 0;

  switch( descriptorType ){
  case HOG:
    prediction = hog::predictionHOG(img, cellsPerBlock, pixelsPerCells, bins);
    break;
  case PYR_HOG:
    {
      desc = phog->computeDescriptor( img, dimension, PHOG::L2NORM );
      cv::Mat descriptor = cv::Mat(1, dimension, CV_32F, desc);
      prediction = svm.predict(descriptor.row(0));
    }
    break;
  }

  return prediction;
}

