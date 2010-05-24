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
  if( points.size() < 2 ) return;

  std::vector<cv::Point2f>::const_iterator p;
  std::vector<double>::const_iterator t;

  p = points.end() - 1;
  t = time.end() - 1;

  double dt      = *t - *(t - 1);
  cv::Point2f dp = *p - *(p - 1);

  dp.x /= static_cast<float>(dt);
  dp.y /= static_cast<float>(dt);

  veloc.push_back( dp );

  if( veloc.size() == 1 ){
    velocity = veloc.back();
    return;
  }

  //Compute the average on-line
  //Reusing dp for computing the average velocity
  velocity.x *= static_cast<float>(veloc.size() - 1);
  velocity.y *= static_cast<float>(veloc.size() - 1);

  velocity  += veloc.back();

  velocity.x /= static_cast<float>(veloc.size());
  velocity.y /= static_cast<float>(veloc.size());
}

void MotionAnalyzer::drawTrajectory(cv::Mat& frame){
  if( points.size() < 2 ) return;
 
  //Draw lines per frame
  for( std::vector<cv::Point2f>::const_iterator p = points.begin();
       (p + 1) != points.end(); ++p){
    cv::line( frame, *p, *(p+1), cv::Scalar(255.0, 0.0, 0.0) );
  }
}
