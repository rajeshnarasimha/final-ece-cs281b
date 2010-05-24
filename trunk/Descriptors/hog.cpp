/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <hog.h>
#include <cmath>
using namespace std;
using namespace cv;


#define PI 3.1416

/*
vector <double> obtainVectorFromBlock(vector<MatND> block)
{
	vector<MatND>  ::const_iterator celdita;
	vector <double> valuesHistograms;
	
	for(celdita=block.begin();celdita!=block.end();++celdita)
  	{
		MatND tempCelda =*celdita;
		//CvHistogram * tempHist=tempCelda.hist;
		int i=0;
		while(i<8)
		{
			double  tempMagnitud= cvQueryHistValue_1D(tempHist,i);
			valuesHistograms.push_back(tempMagnitud);
			i++;
		}
	}
	return valuesHistograms;
}*/




vector <Mat>  hog::normalizeBlock(vector <MatND>   block, double e)
{
	vector<MatND>  ::const_iterator celdita;
	double total=0;
	for(celdita=block.begin();celdita!=block.end();++celdita)
  	{
		double tempTotal=calculateL2Norm(*celdita);
		total=tempTotal+total;
		
	}
	total=total+e;
	total=sqrt(total);
	total=total+e;
	vector <Mat>  normBlock;
	for(celdita=block.begin();celdita!=block.end();++celdita)
  	{
		Mat h =Mat(*celdita);
		
		h=h*total;
		normBlock.push_back(h);
		
		
	}
	
	return normBlock;
	
}
double hog:: calculateL2Norm(MatND matricita)
{
	Mat h =Mat(matricita);
	double magTotal=h.dot(h);
	return magTotal;
	
}


void buildCellHistogram(const int& x, const int& y,
                        const cv::Mat& angle, const cv::Mat& magn,
                        cv::MatND& histogram, const int& bins,
                        const cv::Size& window);

//dx, dy type CV_32FC1

void hog::computeCells(const cv::Mat& dx, const cv::Mat& dy, 
                       const int& row, const cv::Size& window,
                       std::vector< std::vector<cv::MatND> >& cells,
                       const int& bins){
  
  //Compute Magnitude and Angle
  cv::Mat angle = cv::Mat::zeros(dx.rows, dx.cols, CV_32FC1);
  cv::Mat magn  = cv::Mat::zeros(dx.rows, dx.cols, CV_32FC1);

  magn  = dx.mul(dx) + dy.mul(dy);
  angle = dy / dx;

  for( int i = 0; i < magn.cols; i++){
    for( int j = 0; j < magn.rows; j++){
      magn.at<float>(j,i)  = sqrt(magn.at<float>(j,i));
      angle.at<float>(j,i) = atan(angle.at<float>(j,i))*180/PI;
      angle.at<float>(j,i) = angle.at<float>(j,i) < 0 ? 
        angle.at<float>(j,i) + 180 :
        angle.at<float>(j,i);
    }
  }

  //Create Cells
  int sizes[] = { bins };
  for( int y = 0; y < angle.rows; y += window.height){
    std::vector<cv::MatND> histRow;
    
    for( int x = 0; x < angle.cols; x += window.width){
      cv::MatND histogram(1, sizes, CV_32F, cv::Scalar(0.0) );
      buildCellHistogram(x,y,angle,magn,histogram,bins,window);
      histRow.push_back( histogram );
    }
    
    cells.push_back(histRow);
  }
  


	
}


vector< vector <Mat> > hog::computeBlocks(vector< vector<MatND> >& cells,vector< vector<MatND> >  & blocks,int sizeInput,double e)
{
	vector<MatND> block;
  	vector < vector<MatND> > ::const_iterator punto;
	
	//int i=0;
	for(punto=cells.begin();punto!=cells.end();punto=punto+3)
  	{
	  /*obtenemos 3 renglones de celdas consegutivos*/
	const  vector<MatND> & primerRenglon=*punto;
	 const vector<MatND> & segundoRenglon=*(punto+1);
	 const vector<MatND> & tercerRenglon=*(punto+2);
	  unsigned int iterador=0;
	  /*Armaremos el bloque  tomando 3 elementos consecutivos de cada renglon, y
	   * armando asi el bloque de 3X3, despues avanzaremos y armaremos otro 
	   * bloque*/
	   
	   while(iterador+2<primerRenglon.size()||iterador+2<segundoRenglon.size()||iterador+2<tercerRenglon.size())
	  {
		  block.push_back(primerRenglon.at(iterador));
		  block.push_back(segundoRenglon.at(iterador));
		  block.push_back(tercerRenglon.at(iterador));
		  
		  block.push_back(primerRenglon.at(iterador+1));
		  block.push_back(segundoRenglon.at(iterador+1));
		  block.push_back(tercerRenglon.at(iterador+1));
		  
		  block.push_back(primerRenglon.at(iterador+2));
		  block.push_back(segundoRenglon.at(iterador+2));
		  block.push_back(tercerRenglon.at(iterador+2));
		  
		  //cout<<"size block inside "<<block.size()<<endl;
		  blocks.push_back(block);
		  //cout<<"one block"<<block.size();
		  block=vector<MatND>();
		  iterador+=3;
		  
	  }
	   
	 // i++;
	  
  }
   vector< vector <Mat> >  normalizedBlocks;
   vector <Mat> normValues;
   for(punto=blocks.begin();punto!=blocks.end();++punto)
  {
	 normValues=normalizeBlock(*punto, e);
	 normalizedBlocks.push_back(normValues);
	//sizeMax++;
  	// normValues= normalizeBlock(*punto, .002);
  	 //normalizedBlocks.push_back(normValues);
  }
	
	int totalSize=normalizedBlocks.size()*normValues.size();
 
  sizeInput=totalSize+192;
  return normalizedBlocks;
  
 }

float ** hog::integrate( vector<Mat>block, float ** a, int & indice,int trainEx)
{
	vector<Mat>::const_iterator matricita;
	int i=0;
	for(matricita=block.begin();matricita!=block.end();++matricita)
  	{
		
		/*assumiendo que la matriz esta hecha de 6*6*/
		for(int j=0;j<6;j++)
		{
			for(int k=0;k<6;k++)
			{
				//Mat temp=*matricita;
				//temp.at<float>(k,j);
				a[trainEx][indice]=matricita->at<float>(j,k);
				indice++;
		
			}
		}
   }
    return a;
}

float ** hog::prepareTData(vector < vector<Mat> >  normalizedBlocks,float **trainingData,int numTrain,int size)
{
	trainingData[numTrain] = new float[size];
	vector< vector<Mat> > ::const_iterator parteEjemplo;
	int indice=0;
	for(parteEjemplo=normalizedBlocks.begin();parteEjemplo!=normalizedBlocks.end();++parteEjemplo)
  	{
		  integrate (*parteEjemplo,trainingData,indice,0);
	  	  indice++;
	 
  	}
  	return trainingData;
}

void buildCellHistogram(const int& x, const int& y,
                        const cv::Mat& angle, const cv::Mat& magn,
                        cv::MatND& histogram, const int& bins,
                        const cv::Size& window){
  cv::Mat W;
  cv::Mat M;
  cv::Rect roi = cv::Rect(x,y, window.width, window.height);
  angle(roi).copyTo(W);
  magn(roi).copyTo(M);

  int bin;
  int step = 180 / bins;

  for( int i = 0; i < W.cols; i++){
    for( int j = 0; j < W.rows; j++){
      int angleAt = static_cast<int>( W.at<float>(j,i) );
      bin = angleAt == 180 ? bins - 1 : angleAt % 180 / step;
      histogram.at<int>(bin) += M.at<float>(j,i); 
    }
  }

}
