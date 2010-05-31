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
 std::vector< std::vector <cv::Mat> > computeBlocks(int & sizeInput,double e,std::vector< std::vector<cv::MatND> > cells,int windowSize);
 
 double calculateL2Norm(cv::MatND matricita);
 
 void obtainTestDescriptor(std::string fileName, std::string outputName,int & size,float **descriptor,int sizeCeldas,int sizeBlock);
 
 float ** trainEx(std::string fileName,int trainEx,float **trainingData,std::string nameFileWrite,int & size,int windowSize,int blockSize);
  
 void prepareDerImages(std::string pictureName,cv::Mat & dx, cv::Mat & dy);
 float ** trainEx(cv::Mat originalImage,int trainEx,float **trainingData,std::string nameFileWrite,int & sizeInput,int windowSize,int blockSize);
 void prepareDerImages(cv::Mat img,cv::Mat & dx, cv::Mat & dy); 
 
 std::vector <cv::Mat> normalizeBlock(std::vector <cv::MatND>   block, double e);
 float predictionHOG(cv::Mat);
 float ** integrate( std::vector<cv::Mat>block, float  ** a, int & indice,int trainEx,int size,int windowSize);
 float ** prepareTrainData(int numTrainExample,std::string fileName, int & size,float **trainingData,int sizeCeldas,int sizeBlock,int & i,std::string ruta );
 
 float ** prepareTrainData(int numTrainExample,std::string fileName, int & size,float **trainingData,int sizeCeldas,int sizeBlock,int & i);

void computeBlocks(int & sizeInput,double e,std::vector< std::vector<cv::MatND> > rowsOfCells,int windowSize,int blockSize,std::vector< std::vector <cv::Mat> > & normalizedBlocks);
 
 float ** prepareTData(std::vector < std::vector<cv::Mat> >  normalizedBlocks,float **trainingData,int numTrain,int size,int windowSize);
  void writeDescriptors(float **trainingData,int size, int numTrain,std::string nameFile);         
}


#endif
