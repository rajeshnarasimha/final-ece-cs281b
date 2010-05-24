/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <BSplineFitting.h>

#include <math.h>
#include <gsl/gsl_bspline.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_statistics.h>

BSplineFitting::BSplineFitting( const std::vector<motion_t>& _data ) 
  : Interpolator(_data) {
  
}

BSplineFitting::~BSplineFitting( void ){
  
}

motion_t BSplineFitting::interpolate( double param ){
  return motion_t(0.0, cv::Point2f(0.,0.));
}
