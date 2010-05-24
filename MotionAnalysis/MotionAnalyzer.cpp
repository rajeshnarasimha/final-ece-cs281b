/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <MotionAnalyzer.h>

MotionAnalyzer::MotionAnalyzer( const std::string& file ){
  times.open( file.c_str() );
}

MotionAnalyzer::MotionAnalyzer( void ){
}

MotionAnalyzer::~MotionAnalyzer(void){
  if( times.is_open() ){
    times.close();
  }
}

void MotionAnalyzer::collectCentroidSample( const cv::Point2f& centroid,
                                            const double _time ){
  time.push_back( _time );
  points.push_back(centroid);
  computeVelocities(); //Update the curve
}

double MotionAnalyzer::getTimeFromFrame( const int& nFrame ){
  return 0.0;
}

void MotionAnalyzer::computeVelocities(void){
  //Call a curvefitting
  std::vector<cv::Point2f>::const_iterator p;
  std::vector<cv::Point2f>::const_iterator t;
  double dt(0.0);
  cv::Point2f dp;
  for( p = points.begin(), t = time.begin();
       (p + 1) != points.end(); ++p, ++t){
    dt = (*t) - *(t+1);
  }  
}

void MotionAnalyzer::drawTrajectory(cv::Mat& frame){
  if( points.size() < 2 ) return;

  for( std::vector<cv::Point2f>::const_iterator p = points.begin();
       (p + 1) != points.end(); ++p){
    cv::line( frame, *p, *(p+1), cv::Scalar(255.0, 0.0, 0.0) );
  }
}
