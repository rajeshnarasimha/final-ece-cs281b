
#include <hog.h>
#include <utils.h>
#include <iostream>

using namespace std;
using namespace cv;
using namespace hog;

int main(int argc, char** argv)
{
	float **trainingData=new float*[3];
	trainEx("./per00002.ppm",0,trainingData,"ex1.txt");
	trainEx("./per00001.ppm",1,trainingData,"ex2.txt");
	trainEx("./per00000.ppm",2,trainingData,"ex0.txt");
	cout<<"made it here";
	return 0;
}
