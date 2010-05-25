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
                     const cv::Size& window,
                    std::vector< std::vector<cv::MatND> >& cells, 
                    const int& bins=9);
                    
void write(float **trainingData,int size, int numTrain,std::string nameFile);
 std::vector< std::vector <cv::Mat> > computeBlocks(int & sizeInput,double e,std::vector< std::vector<cv::MatND> > cells);
 
 double calculateL2Norm(cv::MatND matricita);
  
 void prepareDerImages(std::string pictureName,cv::Mat & dx, cv::Mat & dy); 
 
 std::vector <cv::Mat> normalizeBlock(std::vector <cv::MatND>   block, double e);
 
 float ** integrate( std::vector<cv::Mat>block, float  ** a, int & indice,int trainEx,int size);
 
 float ** prepareTData(std::vector < std::vector<cv::Mat> >  normalizedBlocks,float **trainingData,int numTrain,int size);
           
}


#endif
