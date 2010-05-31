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
	
	
	trainingData=new float*[1007];
	
   
   	//prepareTrainData(1,"per00", size,trainingData,8,4,i);
    trainingData=prepareTrainData(924,"per00", size,trainingData,8,4,i);
    trainingData=prepareTrainData(1007,"bikeList.lst", size,trainingData,8,4,i,"/home/saiph/281b/final-ece-cs281b/test/bike/");
    //writeDescriptors(trainingData,size,1007,"fluffy2.dat");  
    cv::Mat tset(1007, size, CV_32FC1,trainingData);
    
    ofstream myfilex;
  	myfilex.open ("fluffy33.dat");
	
    for( int j=0;j<1007;j++)
  	{
  		for(int i=0;i<size;i++)
		{
		
			//cout<<"J,I "<<j<<", "<<i<<endl;
			myfilex <<trainingData[j][i]<<endl;
			//trainingData[numTrain][size];	
		}
		myfilex<<endl;
	}
	myfilex.close();
   
   //cout<<"valor i"<<i;
  //cv::Mat tset(1007, size, CV_32FC1,trainingData);
  
   
   float tLables[1007];
	
	for(int i=0;i<924;i++)
	{
		tLables[i]=1.0;
	}
	for(int i=924;i<1007;i++)
	{
		tLables[i]=-1.0;
	}
	cv::Mat labels(1007,1, CV_32FC1,tLables);
	
	
	CvSVM svm;
	
  	svm.train(tset, labels);
  	//svm.load("bicis.xml");
  	
  	ofstream myfile;
  	myfile.open ("resultsTrain.txt");
  	for( int i = 0; i <1007; i++){
    	float cls = svm.predict(tset.row(i));
    	cout<<"i value: "<<i;
    	cout << endl<<"Class: " <<fixed << cls <<endl;
    	myfile <<"i value: "<<i;
    	myfile<<endl<<"Class: " <<fixed << cls <<endl;
  	}
  	
  	
  	myfile.close();
  	svm.save("bicis.xml");
  //	float cls = svm.predict(tset.row(1006));
  //	cout << endl<<"Class: " <<fixed << cls <<endl;
  	
    Mat img = imread("/home/saiph/281b/final-ece-cs281b/test/bike/train/pos/person_and_bike_002a.png");
    if( !img.data ) {
          cerr << "Cannot open File" << endl;
          return 1;
        }
    cout<<"prediction ";
    predictionHOG(img);
    
     img = imread("/home/saiph/281b/final-ece-cs281b/test/bike/train/pos/person_and_bike_002b.png");
    if( !img.data ) {
          cerr << "Cannot open File" << endl;
          return 1;
        }
    cout<<"prediction ";
    predictionHOG(img);
    
     img = imread("/home/saiph/281b/final-ece-cs281b/test/bike/train/pos/person_and_bike_005a.png");
    if( !img.data ) {
          cerr << "Cannot open File" << endl;
          return 1;
        }
    cout<<"prediction ";
    predictionHOG(img);
    
     img = imread("/home/saiph/281b/final-ece-cs281b/test/bike/train/pos/person_and_bike_005b.png");
    if( !img.data ) {
          cerr << "Cannot open File" << endl;
          return 1;
        }
    cout<<"prediction ";
    predictionHOG(img);
    



  	
  	
    
	
	
  	//svm.save("svmPersons.xml");*/
    
	return 0;
}
