/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#ifndef __INTERPOLATOR__
#define __INTERPOLATOR__

#include <MotionDataTypes.h>
#include <vector>

class Interpolator{
public:
  explicit Interpolator( const std::vector<motion_t>& _data ) : data(_data){ };
  virtual ~Interpolator(void){};

  virtual motion_t interpolate(double param) = 0;
protected:
  const std::vector<motion_t>& data;
};

#endif
