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

 std::vector< std::vector <cv::Mat> > computeBlocks(std::vector< std::vector<cv::MatND> >& cells,std::vector< std::vector<cv::MatND> >  & blocks,int sizeInput,double e);
 double calculateL2Norm(cv::MatND matricita);  
 std::vector <cv::Mat> normalizeBlock(std::vector <cv::MatND>   block, double e);
 float ** integrate( std::vector<cv::Mat>block, float ** a, int & indice,int trainEx);
 float ** prepareTData(std::vector < std::vector<cv::Mat> >  normalizedBlocks,float **trainingData,int numTrain,int size);
           
}


#endif
