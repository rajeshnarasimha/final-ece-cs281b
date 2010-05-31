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
#include <utils.h>
//#include <iostream>
//#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include "ml.h"
using namespace std;
using namespace cv;


#define PI 3.1416
void hog::write(float **trainingData,int size, int numTrain,string nameFile)
{
	ofstream myfile;
  	myfile.open (nameFile.c_str());
  	
  	
	for(int i=0;i<size;i++)
	{
		//cout<<"write "<<trainingData[numTrain][i];
		myfile <<trainingData[numTrain][i]<<endl;
		//trainingData[numTrain][size];	
	}
	myfile.close();
	
}


void hog::writeDescriptors(float **trainingData,int size, int numTrain,string nameFile)
{
	ofstream myfile;
  	myfile.open (nameFile.c_str());
  	
  	for( int j=0;j<numTrain;j++)
  	{
  		for(int i=0;i<size;i++)
		{
		
			//cout<<"J,I "<<j<<", "<<i<<endl;
			myfile <<trainingData[j][i]<<endl;
			//trainingData[numTrain][size];	
		}
		myfile <<endl;
	}
	myfile.close();
	
}




vector <Mat>  hog::normalizeBlock(vector <MatND>   block, double e)
{
	vector<MatND>  ::const_iterator celdita;
	double total=0;
	for(celdita=block.begin();celdita!=block.end();++celdita)
  	{
		double tempTotal=calculateL2Norm(*celdita);
		total=tempTotal+total;
		
	}
	e=e*e;
	total=total+e;
	total=sqrt(total);
	//total=total+e;
	total=1/total;
	vector <Mat>  normBlock;
	for(celdita=block.begin();celdita!=block.end();++celdita)
  	{
		Mat h =Mat(*celdita);
		/*for(int i=0;i<6;i++)
		{	
			for(int j=0;j<6;j++)
			{
				//cout<<"value before norm: "<<h.at<float>(i,j);
			}
		}*/
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

void hog::prepareDerImages(Mat img,Mat & dx, Mat & dy)
{
	
  	Mat gray;
        

       // img = imread(pictureName);

        if( !img.data ) {
          cerr << "Cannot open File" << endl;
          return;
        }

	imgutils::bgr2gray(img, gray);

	Sobel(gray, dx, CV_8U, 1, 0);
	Sobel(gray, dy, CV_8U, 0, 1);

	
}

void hog::prepareDerImages(string pictureName,Mat & dx, Mat & dy)
{
	
  	Mat gray;
    Mat img;
	img = imread(pictureName);
	if( !img.data ) {
          cerr << "Cannot open File" << endl;
          return;
        }

	imgutils::bgr2gray(img, gray);

	Sobel(gray, dx, CV_8U, 1, 0);
	Sobel(gray, dy, CV_8U, 0, 1);

	
}


float ** hog::trainEx(Mat originalImage,int trainEx,float **trainingData,string nameFileWrite,int & sizeInput,int windowSize,int blockSize)
{
	
	Mat dx,dy;
	vector< vector<MatND> >cells;
	vector< vector <Mat> > blocks;
	double e=0.0002;
	
	prepareDerImages(originalImage, dx,dy);
	Size windowCell(windowSize,windowSize);
	
	computeCells(dx,dy,windowCell,cells);
	
	vector< vector <Mat> > normalizedBlocks;
	computeBlocks(sizeInput, e,cells,windowSize,blockSize,normalizedBlocks);
	
	trainingData[trainEx] = new float[sizeInput];
	prepareTData(normalizedBlocks,trainingData,trainEx,sizeInput,windowSize);
	
	
	while(!blocks.empty())
	{
		vector <Mat>  & tempBlock=blocks.back();
		blocks.pop_back();
		while(!tempBlock.empty())
		{
			tempBlock.pop_back();
		}
		
	}
	
	char num [3];
  	sprintf (num, "%d",trainEx);
  	nameFileWrite.append(num);
  	nameFileWrite.append(".txt");
	
	 //write(trainingData,sizeInput,trainEx,nameFileWrite);
	return trainingData;
	
}


float ** hog::trainEx(string fileName,int trainEx,float **trainingData,string nameFileWrite,int & sizeInput,int windowSize,int blockSize)
{
	
	Mat dx,dy;
	vector< vector<MatND> >cells;
	vector< vector <Mat> > blocks;
	
	//float **trainingData=new float*[3];
	//int sizeInput=0;
	double e=0.0002;
	cout<<"llegue aqui con "<<fileName;
	prepareDerImages(fileName, dx,dy);
	Size windowCell(windowSize,windowSize);
	
	computeCells(dx,dy,windowCell,cells);
	//blocks=computeBlocks(sizeInput, e,cells,windowSize,2);
	vector< vector <Mat> > normalizedBlocks;
	computeBlocks(sizeInput, e,cells,windowSize,blockSize,normalizedBlocks);
	//cout<<"size out "<<sizeInput;
	//:computeBlocks(int & sizeInput,double e,vector< vector<MatND> > rowsOfCells,int windowSize,int blockSize)
	//blocks=computeBlocks(sizeInput, e,cells,windowSize);
	//cout<<"size out "<<sizeInput;
	//trainingData=
	trainingData[trainEx] = new float[sizeInput];
	prepareTData(normalizedBlocks,trainingData,trainEx,sizeInput,windowSize);
	
	
	while(!blocks.empty())
	{
		vector <Mat>  & tempBlock=blocks.back();
		blocks.pop_back();
		while(!tempBlock.empty())
		{
			tempBlock.pop_back();
		}
		
	}
	 
	//write(trainingData,sizeInput,trainEx,nameFileWrite);
	return trainingData;
	
}






void hog::computeCells(const cv::Mat& dx, const cv::Mat& dy, 
                       const cv::Size& window,
                       std::vector< std::vector<cv::MatND> >& cells,
                       const int& bins){
  
  //Compute Magnitude and Angle
  cv::Mat angle = cv::Mat::zeros(dx.rows, dx.cols, CV_32FC1);
  cv::Mat magn  = cv::Mat::zeros(dx.rows, dx.cols, CV_32FC1);

  cv::Mat dx32f;
  dx.convertTo(dx32f, CV_32FC1);

  cv::Mat dy32f;
  dy.convertTo(dy32f, CV_32FC1);

  magn  = dx32f.mul(dx32f) + dy32f.mul(dy32f);
  angle = dy32f / dx32f;

  for( int i = 0; i < magn.cols; i++){
    for( int j = 0; j < magn.rows; j++){
      magn.at<float>(j,i)  = sqrtf(magn.at<float>(j,i));
      angle.at<float>(j,i) = atanf(angle.at<float>(j,i))*180/PI;
      angle.at<float>(j,i) = angle.at<float>(j,i) < 0 ? 
        angle.at<float>(j,i) + 180 :
        angle.at<float>(j,i);
    }
  }

  //Create Cells
  int sizes[] = { bins };
  for( int y = 0; (y + window.height - 1) < angle.rows; y += window.height)
  {
    std::vector<cv::MatND> histRow;
    
    for( int x = 0; (x + window.width - 1) < angle.cols; x += window.width)
    {
      cv::MatND histogram(1, sizes, CV_32F, cv::Scalar(0.0) );
      buildCellHistogram(x,y,angle,magn,histogram,bins,window);
     /* for(int m=0;m<9;m++)
      {		//cout<<"magnitud histograma "<<fixed<<histogram.at<float>(m)<<endl;
      	
	  }*/
	  histRow.push_back( histogram );
    
    //cells.push_back(histRow);
  }
  cells.push_back(histRow);

}
	
}

void hog::obtainTestDescriptor(string fileName,string outputName, int & size,float **descriptor,int sizeCeldas,int sizeBlock)
{
	descriptor=new float*[1];
	trainEx(fileName,0,descriptor,outputName,size,sizeCeldas,sizeBlock);
	
}


float ** hog::prepareTrainData(int numTrainExample,string fileName, int & size,float **trainingData,int sizeCeldas,int sizeBlock,int & i)
{
	//int size;
	//trainingData=new float*[numTrainExample];
	
	
	
	 
	 while(i<numTrainExample)
	 {
	 
	
		
		string newFileName;
		string outputName="of";
		newFileName.append(fileName);
		if(i<10)
			newFileName.append("00");
		else if(i<100)
				newFileName.append("0");
		char num [3];
  		sprintf (num, "%d",i);
  		outputName.append(num);
  		outputName.append(".txt");
  		newFileName.append(num);
  		newFileName.append(".ppm");
  		//cout<<"Name of file "<<newFileName<<endl;
  		/*if(i==0)
  		newFileName="per00008.ppm";
  		else if(i==1)
  			newFileName="per00001.ppm";
  		else if(i==2)
  			newFileName="per00000.ppm";*/
  		
  		cout<<"this is the i of Training"<<i<<endl;	
  			
  		trainingData=trainEx(newFileName,i,trainingData,outputName,size,sizeCeldas,sizeBlock);
  		i++;
  		/*for(int j=0;j<size;j++)
	  {
		 // cout<<"i "<<i<<" j "<<j;
		  
		  cout<<"value: "<<trainingData[i][j]<<endl;
	  }*/
  		
  		//trainEx("per00980.ppm",i,trainingData,outputName,size,sizeCells,sizeBlocks);
  	}
  	
  	/*
  	for(int i=0;i<numTrainExample;i++)
  {
	  for(int j=0;j<size;j++)
	  {
		  cout<<"i "<<i<<" j "<<j<<endl;
		  cout<<endl<<"size"<<size<<endl;
		  cout<<trainingData[i][j]<<endl;
	  }
  }*/
  	return trainingData;
  }

float hog:: predictionHOG(Mat originalImage)
{
	float ** descriptor=new float*[1];
	int sizeDescriptor;
	int i=0;
	//descriptor=prepareTrainData(1,"bikeList.lst", sizeDescriptor,descriptor,8,4,i,"/home/saiph/281b/final-ece-cs281b/test/bike/");
	descriptor=trainEx(originalImage,0,descriptor,"descriptor",sizeDescriptor,8,4);
	Mat tset(1,sizeDescriptor, CV_32FC1,descriptor);
	CvSVM svm;
	svm.load(("bicis.xml"));
    float cls = svm.predict(tset.row(0));
    cout << endl<<"Class: " <<fixed << cls <<endl;
    return cls;	
  	
	
	
	

}

float ** hog::prepareTrainData(int numTrainExample,string fileName, int & size,float **trainingData,int sizeCeldas,int sizeBlock,int & i,string ruta)
{
	//trainingData=new float*[numTrainExample];
	 string line;
	 cout<<"file name "<<fileName;
  	ifstream myfile (fileName.c_str());
  	if (myfile.is_open())
  	{
		cout<<"llegue aqui";
    	while (! myfile.eof() && (i<numTrainExample))
    	{
			
			string name=ruta;
      		getline (myfile,line);
      		if(line.compare("")==0)
      		{
				cout<<endl<<"empty shit I'm out"<<endl;
				return trainingData;
			}
      		name.append(line);
      		
      		cout<<endl<<"final name: "<<name<<endl<<"value i "<<i;
      		trainingData=trainEx(name,i,trainingData,"trainingDescr",size,sizeCeldas,sizeBlock);
      		i++;
      		cout<<i;
      //	cout << line << endl;
    	}
    	myfile.close();
  	}
  	else
  	cout<<"no logre nada";

  	
  		
  			
  	//	trainingData=trainEx(newFileName,i,trainingData,outputName,size,sizeCeldas,sizeBlock);
  	
  	
  	return trainingData;
  }
  
  
 
  
void hog::computeBlocks(int & sizeInput,double e,vector< vector<MatND> > rowsOfCells,int windowSize,int blockSize,vector< vector <Mat> > & normalizedBlocks)
  {
	  int widthGroupOfCells=rowsOfCells.at(0).size();
	  int heigthGroupOfCells=rowsOfCells.size();
	  //cout<<endl<<"width "<<widthGroupOfCells;
	  //cout<<endl<<"height "<<heigthGroupOfCells;
	  int totalOfBlocks=widthGroupOfCells/blockSize;
	  
	  int tempValue=heigthGroupOfCells/blockSize;
	  totalOfBlocks=totalOfBlocks*tempValue;
	  //cout<<"total blocks "<<totalOfBlocks;
	  vector< vector<MatND> > block;
	  vector< vector<MatND> > rowOfBlocks;
	  //cout<<endl<<" block width: "<<widthGroupOfCells/blockSize<<" block height "<<(heigthGroupOfCells/blockSize);
	  vector<MatND> listOfBlocks[widthGroupOfCells/blockSize][(heigthGroupOfCells/blockSize)];
	  int numRow=0;
	  //cout<<"renlones "<<widthGroupOfCells/blockSize<<endl;
	  //cout<<"columnas"<<heigthGroupOfCells/blockSize<<endl;
	  for(int i=0;i<widthGroupOfCells/blockSize;i++)
	  {
		  for(int j=0;j<heigthGroupOfCells/blockSize;j++)
		  {
			  vector<MatND> tempVector;
			  listOfBlocks[i][j]=tempVector;
		  }
	  }
	  
	  while(numRow<rowsOfCells.size())
	  {
		  vector<MatND> rowCell=rowsOfCells.at(numRow);
	  	  int i=0;
	  	  
	  	  while(i<rowCell.size())
	  	  {
			 MatND cell=rowCell.at(i);
			  
			  int numRowBlock=i/blockSize;
			   int numColBlock=numRow/blockSize;
			// cout<<"columna "<<numColBlock<<"renglon: "<<numRowBlock<<endl;
			listOfBlocks[numRowBlock][numColBlock].push_back(cell);
			   i++;
		  }
		 
		  numRow++;
		 }
	
	
   vector <Mat> normValues;
   
   for(int k=0;k<(widthGroupOfCells/blockSize);k++)
   {
	   for(int j=0;j<(heigthGroupOfCells/blockSize);j++)
	   {
		   
		   normValues=normalizeBlock(listOfBlocks[k][j], e);
		   //vector <MatND>   blockX;
		   /*vector<Mat>  ::const_iterator celditaX;
		   for(celditaX=normValues.begin();celditaX<normValues.end();++celditaX)
  			{
		  
			   Mat tempCell=*celditaX;
			   for(int z=0;z<9;z++)
			   {
				   cout<<"cell value: "<<tempCell.at<float>(0,z)<<endl;
			   }
			  
		   }*/
		   
		   normalizedBlocks.push_back(normValues);
		   
	   }
   }
   //int numCeldas=normalizedBlocks.size()*normValues.size();
  // int numPixeles=numCeldas*windowSize*windowSize;
  // sizeInput=numPixeles;
  //asuminedo 9 bines
  sizeInput=normalizedBlocks.size()*normValues.size()*9;
  //sizeInput=normalizedBlocks.size()*windowSize*windowSize*9;
 // cout<<"windw size "<<windowSize<<endl;
  //cout<<endl<<"size norm Block "<<normalizedBlocks.size()<<"size input "<<sizeInput;
   //sizeInput=4099;
   //return normalizedBlocks;
		 
  		
  	}
vector< vector <Mat> > hog::computeBlocks(int & sizeInput,double e,vector< vector<MatND> > cells,int windowSize)
{
	
	vector< vector<MatND> >  blocks;
	vector<MatND> block;
  	vector < vector<MatND> > ::const_iterator punto;
	
	//int i=0;
	for(punto=cells.begin();punto+2<cells.end();punto=punto+3)
  	{
	  /*obtenemos 3 renglones de celdas consegutivos*/
	  
		const  vector<MatND> & primerRenglon=*punto;
	 	const vector<MatND> & segundoRenglon=*(punto+1);
	 	const vector<MatND> & tercerRenglon=*(punto+2);
	  	unsigned int iterador=0;
	  	/*Armaremos el bloque  tomando 3 elementos consecutivos de cada renglon, y
	   	* armando asi el bloque de 3X3, despues avanzaremos y armaremos otro 
	   	* bloque*/
	   	//&&iterador+2<segundoRenglon.size()&&iterador+2<tercerRenglon.size()
	   	while(iterador+2<primerRenglon.size() )
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
	
	
	//cout<<"norm size"<<normalizedBlocks.size()<<" block "<<normValues.size();
	int numCeldas=normalizedBlocks.size()*normValues.size();
	/*asumiendo que cada celda es de 6*6*/
	//int numPixeles=numCeldas*windowSize*windowSize;
	//cout<<"num pixeles "<<numPixeles;
 	
  sizeInput=3624;
  return normalizedBlocks;
  
 }

float ** hog::integrate( vector<Mat>block, float ** a, int & indice,int trainEx, int size, int windowSize)
{
	vector<Mat>::const_iterator matricita;
	//int i=0;
	
	for(matricita=block.begin();matricita!=block.end();++matricita)
  	{
		
		
		Mat temp=*matricita;
		Size sizeMatricita=temp.size();
		for(int j=0;j<sizeMatricita.width;j++)
		{
			for(int k=0;k<sizeMatricita.height;k++)
			{
				//if(indice>size)
					//return a;
				
				//temp.at<float>(k,j);
				//a[trainEx][indice]=matricita->at<float>(j,k);
				a[trainEx][indice]=matricita->at<float>(k,j);
			
				//cout<<"value"<<matricita->at<float>(j,k);
				
				//cout<<"height "<<tam.height<<" width "<<tam.width;
				//cout<<"size matriz ";
				//cout<<"valor indice: "<<indice<<endl;
				
				indice++;
		
			}
		}
   }
    return a;
}

float ** hog::prepareTData(vector < vector<Mat> >  normalizedBlocks,float **trainingData,int numTrain,int size,int windowSize)
{
	
	
	vector< vector<Mat> > ::const_iterator block;
	int indice=0;
	for(block=normalizedBlocks.begin();block!=normalizedBlocks.end();++block)
  	{
		 integrate(*block,trainingData,indice,numTrain,size,windowSize);
		 //cout<<"indice out: "<<indice<<" size: "<<size<<endl;
		 
	  	 // indice++;
	 	//if(indice>size)
	 		//return trainingData;
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
      int angleAt = static_cast<int>( ceilf(W.at<float>(j,i)) );
      bin = angleAt == 180 ? bins - 1 : angleAt % 180 / step;
      histogram.at<float>(bin) += M.at<float>(j,i); 
      //cout<<"BIN "<<bin<<endl;
     // cout<<"magnitud histograma "<<histogram.at<int>(bin); 
    }
  }
  

}
