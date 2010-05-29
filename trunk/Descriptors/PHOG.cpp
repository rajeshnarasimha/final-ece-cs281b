/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <PHOG.h>

#include <iostream>

#define PI 3.1416f
#define PI_HALF 1.5708f
#define RAD2DEG 180 / PI;

inline void initializeFloatArray(float* array, int size){
  for( register int i = size - 1; i >= 0; i--){
    array[i] = 0.0f;
  }
}

inline float dotProduct(float* descriptor, int size){
  float sum = 0.0f;
  for(int i = 0; i < size; i++){
    sum += descriptor[i]*descriptor[i];
  }
  return sum;
}

inline void scaleDescriptor(float* descriptor, int size, float scalar){
  for(int i = 0; i < size; i++){
    descriptor[i] /= scalar;
  }
}

PHOG::PHOG(const int _bins, const int _maxAngle, const int _l) 
  : bins(_bins), maxAngle(_maxAngle), l(_l){   
}

PHOG::~PHOG(void){
}

float* PHOG::computeDescriptor(const cv::Mat& _img, int& dim, 
                               const cv::Rect& roi, int norm){

  cv::Mat img(_img, roi);
  
  imgSize = cv::Size(img.cols, img.rows);
  
  cv::Mat dx, dy, bin, angle, magn;
  
  computeGradient(img, dx, dy);  
  
  //Compute Magnitude and Angle
  angle = cv::Mat::zeros(img.rows, img.cols, CV_32FC1);
  magn  = cv::Mat::zeros(img.rows, img.cols, CV_32FC1);
  bin   = cv::Mat::zeros(img.rows, img.cols, CV_32SC1);

  computePhasor(dx, dy, bin, angle, magn);

  float* descriptor = computePyramid(angle, magn, bin, dim);

  normalizeHistogram(descriptor, dim, norm);

  return descriptor;
}

void PHOG::computeDescriptor(const std::vector<cv::Mat>& imgs,
                             std::vector<float*>& descriptors, 
                             int& dim, int norm){
  float* descriptor = 0;
  int counter = 0;
  for( std::vector<cv::Mat>::const_iterator it = imgs.begin(); 
       it != imgs.end(); ++it, counter++ ){
    descriptor = computeDescriptor( (*it), dim, norm );
    if( !descriptor ){ 
      std::cerr << "Skipped Image " << counter << std::endl;
      continue; 
    }
    descriptors.push_back( descriptor );
  }
}


void PHOG::computeGradient( const cv::Mat& img,
                            cv::Mat& dx, cv::Mat& dy ){
  float gradOp[] = {-1.0f, 0.0f, 1.0f};
  cv::Mat dx_op(1, 3, CV_32F, gradOp);
  cv::Mat dy_op = dx_op.t();

  cv::filter2D(img, dx, CV_32F, dx_op);
  cv::filter2D(img, dy, CV_32F, dy_op);
}

void PHOG::computePhasor(const cv::Mat& dx, 
                         const cv::Mat& dy, 
                         cv::Mat& bin, 
                         cv::Mat& angle,
                         cv::Mat& magn){

  cv::Mat dot = dx.mul(dx) + dy.mul(dy);
  cv::Mat t = dy / dx; 

  int step = maxAngle / bins;

  for( int i = 0; i < magn.cols; i++){
    for( int j = 0; j < magn.rows; j++){
      magn.at<float>(j,i)  = sqrtf(dot.at<float>(j,i));
      //float a = atanf(t.at<float>(j,i))*180.0f/PI;
      //angle.at<float>(j,i) = a < 0 ? a + maxAngle : a;

      if( maxAngle == 180 ){
        float a = (atanf(t.at<float>(j,i) ) + PI_HALF)*RAD2DEG;
        angle.at<float>(j,i) = a;      
      }else if( maxAngle == 360 ){
        float a = (atan2f( dy.at<float>(j,i), dx.at<float>(j,i) ) + PI)*RAD2DEG;
        angle.at<float>(j,i) = a;      
      }
      bin.at<int>(j,i) = static_cast<int>( angle.at<float>(j,i) / step );

    }
  }

}

float* PHOG::computePyramid(const cv::Mat& angle, 
                            const cv::Mat& magn, 
                            const cv::Mat& bin,
                            int& dim){

  std::vector<float*> descriptor;

  float* block = computeHistogramPerBlock(angle, magn, bin, 
                                          cv::Rect(0,0,angle.cols,angle.rows));

  descriptor.push_back(block);

  //
  int blocks = 0;
  int dx = 0;
  int dy = 0;
  int x = 0;
  int y = 0;

  cv::Rect window;

  for( int i = 1; i < l; i++){
    blocks = pow(2, i);

    dx = imgSize.width  / blocks;
    dy = imgSize.height / blocks;
    
    x = 0;
    y = 0;

    while( x + dx <= imgSize.width ){
      while( y + dy <= imgSize.height ){
        window = cv::Rect(x,y, dx, dy);
        block = computeHistogramPerBlock(angle, magn, bin, window);
        y += dy;

        descriptor.push_back(block);
      }
      y = 0;
      x += dx;
    } 

  }

  dim = descriptor.size() * bins;
  float* desc = new float[dim];

  int idx = 0;
  for( std::vector<float*>::const_iterator it = descriptor.begin();
       it != descriptor.end(); ++it){
    for(register int d = 0; d < bins; d++){
      desc[idx++] = (*it)[d];
    }
  }

  return desc;
}


float* PHOG::computeHistogramPerBlock(const cv::Mat& angle, 
                                      const cv::Mat& magn,
                                      const cv::Mat& bin,
                                      const cv::Rect& roi){
  float* hist = new float[bins];
  //memset(hist, 0, bins*sizeof(float));
  initializeFloatArray(hist, bins);

  for(register int x = roi.x; x < roi.width; x++){
    for(register int y = roi.y; y < roi.height; y++){
      int b = bin.at<int>(y,x);
      hist[ b ] += magn.at<float>(y, x);
    }
  }

  return hist;
}

void PHOG::normalizeHistogram(float* histogram, const int size, int norm){

  switch( norm ){
  case PHOG::L1NORM:
    {
      float sum = 0.0f;
      for( int i = 0; i < size; i++){
        sum += histogram[i];
      }

      scaleDescriptor(histogram, size, sum);
    }
    break;
  case PHOG::L2NORM:
    {
      float norm = sqrtf( dotProduct(histogram, size) );
      scaleDescriptor(histogram, size, norm);
    }
    break;

  case PHOG::L2NORM_BLOCK:
    {
      
    }
    break;
    
  }
}


