/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#ifndef __MOTION_ANALYSIS__
#define __MOTION_ANALYSIS__

#include <cv.h>
#include <string>
#include <fstream>

#include <MotionDataTypes.h>

class MotionAnalyzer{
public:
  enum {
    BICUBIC_BSPLINE,
    LAGRANGE
  };

  MotionAnalyzer( const std::string& file );
  MotionAnalyzer( void );
  ~MotionAnalyzer(void);

  void collectCentroidSample( const cv::Point2f& centroid,
                              const double time );

  cv::Point2f guessNextCentroid( void );

private:
  std::ifstream times;

  //std::vector<motion_t> trajectory;
  std::vector<double> time;
  std::vector<cv::Point2f> points;

  //Curve Generated by OpenCV
  cv::Mat curve;

  //Helper functions
  double getTimeFromFrame( const int& nFrame );

  //Compute Curve
  void computeCurve(void);
};

#endif
