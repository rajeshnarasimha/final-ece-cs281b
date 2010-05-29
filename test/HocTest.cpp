
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
	float **trainingData;
	//float **test=new float*[1];
	int size;
	int i=0;
	
	
	trainingData=new float*[1122];
   
    prepareTrainData(924,"per00", size,trainingData,8,4,i);
     prepareTrainData(1121,"example.txt", size,trainingData,8,4,i,"/home/saiph/281b/final-ece-cs281b/test/");
   
   
   //prepareTrainData(831,"pos.txt", size,trainingData,8,4,i,"/home/saiph/281b/final-ece-cs281b/test/");
   //prepareTrainData(924,"per00", size,trainingData,8,4,i);
   //prepareTrainData(999,"trainPos.lst", size,trainingData,8,4,i,"/home/saiph/281b/final-ece-cs281b/test/");
  // prepareTrainData(924,"per00", size,trainingData,8,4,i);
   //prepareTrainData(1639,"trainNeg.lst", size,trainingData,8,4,i,"/home/saiph/281b/final-ece-cs281b/test/");
   cout<<"valor i"<<i;
  cv::Mat tset(1122, size, CV_32FC1,trainingData);
   float tLables[1122];
	
	for(int i=0;i<924;i++)
	{
		tLables[i]=1.0;
	}
	for(int i=924;i<1122;i++)
	{
		tLables[i]=-1.0;
	}
	cv::Mat labels(1122, 1, CV_32FC1,tLables);
	
	CvSVM svm;
	
  	svm.train(tset, labels);
  	
  	ofstream myfile;
  	myfile.open ("output.txt");
  	for( int i = 0; i <1122; i++){
    	float cls = svm.predict(tset.row(i));
    	cout<<"i value: "<<i;
    	cout << endl<<"Class: " <<fixed << cls <<endl;
    	myfile <<"i value: "<<i;
    	myfile<<endl<<"Class: " <<fixed << cls <<endl;
  	}
  	
  	
  	myfile.close();
  	svm.save("svmPersons.xml");
    
	
	
  	//svm.save("svmPersons.xml");*/
    
	return 0;
}
