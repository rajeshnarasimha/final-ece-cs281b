
#include <hog.h>
#include <utils.h>
#include <iostream>

using namespace std;
using namespace cv;
using namespace hog;

int main(int argc, char** argv)
{
	vector< vector <Mat> >  normalizedBlocks;
	vector< vector <Mat> >  normalizedBlocks2;
	vector< vector <Mat> > normalizedBlocks3;
	vector< vector<MatND> >cells;
	vector< vector <Mat> > blocks;
	int sizeInput=0;
	double e=0000.2;
	
	//vector< vector <Mat> > hog::computeBlocks(vector< vector<MatND> >& cells,vector< vector<MatND> >  & blocks,int sizeInput,double e)
	//computeBlocks(sizeInput,e);
	//cout<<"made it";
	Mat dx,dy;
	prepareDerImages("./per00001.ppm", dx,dy);
	Size windowCell(6,6);
	computeCells(dx,dy,windowCell,cells);
	
                     
	cout<<"made it here";
	/*int size=0,size2,size3;
	
	
	normalizedBlocks=obtainDescriptor("per00000.ppm",size);
	
	normalizedBlocks2=obtainDescriptor("per00001.ppm",size2);
	normalizedBlocks3=obtainDescriptor("per00002.ppm",size2);*/
 
	return 0;
}
