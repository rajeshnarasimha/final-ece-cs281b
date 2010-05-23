/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#ifndef __HOG__
#define __HOG__

#include <vector>
#include <cv.h>
#include <highgui.h>

namespace hog{

  void computeCells(const cv::Mat& dx, const cv::Mat& dy, 
                    const int& row, const cv::Size& window,
                    std::vector< std::vector<cv::MatND> >& cells, 
                    const int& bins=9);

  void computeBlocks(std::vector< std::vector<cv::MatND> >& cells,
                     
}


#endif
