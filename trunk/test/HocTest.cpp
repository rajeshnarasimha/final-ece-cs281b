
#include <hog.h>
#include <utils.h>
#include <iostream>

using namespace std;
using namespace cv;
using namespace hog;

int main(int argc, char** argv)
{
	float **trainingData=new float*[3];
	trainEx("./per00001.ppm",0,trainingData);
	trainEx("./per00002.ppm",1,trainingData);
	cout<<"made it here";
	
 
	return 0;
}
