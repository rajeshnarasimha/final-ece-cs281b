/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#ifndef __LAGRANGE_INTERPOLATION__
#define __LAGRANGE_INTERPOLATION__

#include <Interpolator.h>

class LagrangeInterpolator : public Interpolator{
public:
  explicit LagrangeInterpolator( const std::vector<motion_t>& _data );
  ~LagrangeInterpolator(void);

  motion_t interpolate( double param );
};

#endif
