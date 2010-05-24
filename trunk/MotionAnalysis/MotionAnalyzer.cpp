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
  // motion_t tup( getTimeFromFrame(nFrame), centroid );
  // trajectory.push_back(tup);
  time.push_back( _time );
  points.push_back(centroid);
  computeCurve(); //Update the curve
}

double MotionAnalyzer::getTimeFromFrame( const int& nFrame ){
  return 0.0;
}

void MotionAnalyzer::computeCurve(void){

}
