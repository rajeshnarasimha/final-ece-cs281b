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
	
    Mat img = imread("/home/saiph/281b/final-ece-cs281b/test/bike/train/pos/person_and_bike_002a.png");
    if( !img.data ) {
          cerr << "Cannot open File" << endl;
          return 1;
        }
    cout<<"prediction ";
    predictionHOG(img,4,8,9);
    
    
    
	return 0;
}
