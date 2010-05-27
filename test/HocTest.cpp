
#include <hog.h>
#include <utils.h>
#include <iostream>
#include "ml.h"

#include <fstream>

using namespace std;
using namespace cv;
using namespace hog;

int main(int argc, char** argv)
{
	float **trainingData=new float*[3];
	float **test=new float*[1];
	int size;
	
	//prepareTrainData(1,"per00", size,trainingData,8,4);
	
	
   prepareTrainData(985,"per00", size,trainingData,8,4);
   
	cv::Mat tset(985, size, CV_32FC1,trainingData);
	//cv::Mat tTest(1, size, CV_32FC1,test);
	float tLables[985];
	for(int i=0;i<924;i++)
	{
		tLables[i]=1.0;
	}
	for(int i=924;i<985;i++)
	{
		tLables[i]=-1.0;
	}
  	//float _labels[]={2.0,2.0,3.0};
  	cv::Mat labels(985, 1, CV_32FC1,tLables);
  	
  	CvSVM svm;
  	svm.train(tset, labels);
  	ofstream myfile;
  	myfile.open ("output.txt");
  	for( int i = 0; i < 985; i++){
    	float cls = svm.predict(tset.row(i));
    	cout<<"i value: "<<i;
    	cout << endl<<"Class: " <<fixed << cls <<endl;
    	myfile <<"i value: "<<i;
    	myfile<<endl<<"Class: " <<fixed << cls <<endl;
  	}
  	
  	
  	myfile.close();
  	svm.save("svmPersons.xml");
  	//myfile.close();
  	
  	//float cls = svm.predict(tTest.row(0));
    //cout << endl<<"Class: " <<fixed << cls <<endl;
    
	return 0;
}
