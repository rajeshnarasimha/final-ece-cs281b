
#include <hog.h>
#include <utils.h>
#include <iostream>
#include "ml.h"

using namespace std;
using namespace cv;
using namespace hog;

int main(int argc, char** argv)
{
	float **trainingData=new float*[3];
	int size;
	trainEx("./per00004.ppm",0,trainingData,"ex11.txt",size);
	trainEx("./per00001.ppm",1,trainingData,"ex22.txt",size);
	trainEx("./per00000.ppm",2,trainingData,"ex00.txt",size);
	//cout<<"made it here "<<size<<" fuck";
	write(trainingData,size,0,"ex.txt");
	
	cv::Mat tset(3, size, CV_32FC1,trainingData);
  	float _labels[]={1.0,1.0,-1.0};
  	cv::Mat labels(3, 1, CV_32FC1, _labels);
  	CvSVM svm;
  	svm.train(tset, labels);
  	for( int i = 0; i < 3; i++){
    	float cls = svm.predict(tset.row(i));
    	std::cout << "Class: " << std::fixed << cls << std::endl;
  	}
	return 0;
}
