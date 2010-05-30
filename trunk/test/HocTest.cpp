
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
	Mat img = imread("/home/saiph/281b/final-ece-cs281b/test/bike/train/pos/person_and_bike_143c.png");
    if( !img.data ) {
          cerr << "Cannot open File" << endl;
          return 1;
        }
        
    predictionHOG(img);
	return 0;
}
