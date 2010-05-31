#include <hog.h>
#include <utils.h>
#include <iostream>
#include "ml.h"

#include <fstream>

using namespace std;
using namespace cv;
using namespace hog;

int countMalas(int finClase1)
{
	string line;
	int i=0;
	int counterWrong=0;
  ifstream myfile ("trainResults.txt");
  if (myfile.is_open())
  {
    while (! myfile.eof() )
    {
      getline (myfile,line);
      if(i>finClase1)
      {
		  if(line.compare("1.000000")==0)
		  {
			  counterWrong++;
		  }
	  }
	  
	  else
	  {
		  if(line.compare("-1.000000")==0)
		  {
			  counterWrong++;
		  }
	  }
	  i++;
      cout << line << endl;
    }
    myfile.close();
    return counterWrong;
  }

  else cout << "Unable to open file"; 

  return -1;


}
int main(int argc, char** argv)
{
	
	float **trainingData;
	//float **test=new float*[1];
	int size;
	int i=0;
	
	
	trainingData=new float*[484];
	//trainingData=new float*[1007];
	trainingData=prepareTrainData(301,"per00", size,trainingData,8,4,i);
    trainingData=prepareTrainData(401,"bikeTrainingSet.txt", size,trainingData,8,4,i,"/home/saiph/281b/final-ece-cs281b/");
	trainingData=prepareTrainData(484,"bikeList.lst", size,trainingData,8,4,i,"/home/saiph/281b/final-ece-cs281b/test/bike/");   
   	//prepareTrainData(1,"per00", size,trainingData,8,4,i);
   	
   // trainingData=prepareTrainData(924,"per00", size,trainingData,8,4,i);
    //trainingData=prepareTrainData(1007,"bikeList.lst",size,trainingData,8,4,i,"/home/saiph/281b/final-ece-cs281b/test/bike/");
    //writeDescriptors(trainingData,size,1007,"fluffy2.dat");  
    cv::Mat tset(484, size, CV_32FC1,trainingData);
   //cv::Mat tset(1007, size, CV_32FC1,trainingData);
    
    /*ofstream myfilex;
    
  	myfilex.open ("descriptorImages2.dat");
	
    for( int j=0;j<1024;j++)
  	{
  		for(int i=0;i<size;i++)
		{
		
			//cout<<"J,I "<<j<<", "<<i<<endl;
			myfilex <<trainingData[j][i]<<" ";
			//trainingData[numTrain][size];	
		}
		myfilex<<endl;
	}
	myfilex.close();
   */
   //cout<<"valor i"<<i;
  //cv::Mat tset(1007, size, CV_32FC1,trainingData);
  
   
   float tLables[484];
	
	for(int i=0;i<301;i++)
	{
		tLables[i]=1.0;
	}
	for(int i=301;i<484;i++)
	{
		tLables[i]=-1.0;
	}
	cv::Mat labels(484,1, CV_32FC1,tLables);
	
	CvSVMParams params;
  params.svm_type = CvSVM::C_SVC;
  params.kernel_type = CvSVM::RBF;
  //params.kernel_type = CvSVM::LINEAR;
  params.gamma = .3;
  params.C = .95;
	
	CvSVM svm;
	//classifier.train(cvTset, cvLset, cv::Mat(), cv::Mat(), params)
  	svm.train(tset, labels,cv::Mat(), cv::Mat(), params);
  	//svm.train(tset, labels);
  	//svm.load("SVMbicis.xml");
  	
  	ofstream myfile;
  	//myfile.open ("trainResults.txt");
  	myfile.open ("trainResults.txt");
  	for( int a = 0; a <484; a++){
    	float cls = svm.predict(tset.row(a));
    	//cout<<"i value: "<<a;
    	cout <<"i value: "<<a<<endl<<"Class: " <<fixed << cls <<endl;
    	//myfile <<"i value: "<<a;
    	myfile<<fixed << cls <<endl;
  	}
  	
  	int counter=countMalas(301);
  	cout<<"size "<<counter;
  	myfile.close();
  	svm.save("SVMbicis.xml");
  //	float cls = svm.predict(tset.row(1006));
  //	cout << endl<<"Class: " <<fixed << cls <<endl;
  	/*
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
    */



  	
  	
    
	
	
  	//svm.save("svmPersons.xml");*/
    
	return 0;
}
