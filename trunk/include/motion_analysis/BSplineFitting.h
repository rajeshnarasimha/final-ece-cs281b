/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#ifndef __BSPLINE_INTERPOLATION__
#define __BSPLINE_INTERPOLATION__

#include <Interpolator.h>

class BSplineFitting : public Interpolator{
public:
  explicit BSplineFitting( const std::vector<motion_t>& _data );
  ~BSplineFitting(void);

  motion_t interpolate( double param );
};

#endif
