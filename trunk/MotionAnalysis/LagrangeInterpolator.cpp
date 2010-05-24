/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <LagrangeInterpolator.h>

LagrangeInterpolator::LagrangeInterpolator( const std::vector<motion_t>& _data ) 
  : Interpolator(_data){
  
}

LagrangeInterpolator::~LagrangeInterpolator( void ){
  
}

//Implement the Barycentric implementation
motion_t LagrangeInterpolator::interpolate( double param ){
  return motion_t(0.0, cv::Point2f(0.,0.));
}
