#include <PHOG.h>
#include <hog.h>
#include <utils.h>
#include <iostream>
#include <ml.h>
#include <fstream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace boost::filesystem;

static void readDataSet( const path& directory, 
                         const std::string& prefix,
                         const std::string& posprefix,
                         std::vector<std::string>& images,
                         std::vector<float>& labels,
                         bool filter=true);

static void testKNN(const std::string& tset,
                    const std::string& test_bed,
                    const std::string& repot);

static inline void usage(void){
  std::cout << "./knnTest <tset> <testset>" << std::endl;
}

static int L = 6;
static PHOG phog(9, 180, L);

int main(int argc, char** argv){

  if( argc != 3 ){
    usage();
    return 1;
  }

  testKNN(argv[1], argv[2], "Statistics.dat");

  return 0;
}

void readDataSet( const path& directory, 
                  const std::string& prefix,
                  const std::string& posprefix,
                  std::vector<std::string>& images,
                  std::vector<float>& labels,
                  bool filter){
  if( exists( directory ) ){   
      directory_iterator end;
      std::string file;
      unsigned int pos = 0;
      std::string fullPath;
      
      for( directory_iterator iter(directory) ; iter != end ; ++iter )        
        if ( !is_directory( *iter ) ){
          file = iter->path().leaf();

          pos = filter ? file.find(prefix) : 1;
          
          if( pos != std::string::npos){
            fullPath = directory.string() + file;

            pos = file.find(posprefix);

            if( pos != std::string::npos ){
              std::cout << file << " => " << -1 << std::endl;
              labels.push_back(-1.0f);
            }else{
              std::cout << file << " => " << 1 << std::endl;
              labels.push_back(1.0f);              
            }
            
            //imgs << fullPath << std::endl;
            images.push_back( fullPath );
          }
        }
  }else{
    std::cerr << "Directory does not exists " << std::endl;
  }
  
  //Sort the files
  //sort(images.begin(), images.end(), stringCompare);
}

void testKNN( const std::string& _tset, 
              const std::string& _testset,
              const std::string& report ){

  std::vector<std::string> files;
  std::vector<cv::Mat> images;
  std::vector<float> labels;
  std::vector<float*> descriptors;

  readDataSet( _tset, ".ppm", "person_and_bike_", files, labels, false);

  std::cout << "NumFiles: " << files.size() << std::endl;

  //Reading the Images
  int idx = 0;
  for( std::vector<std::string>::const_iterator it = files.begin();
       it != files.end(); ++it, idx++){
    cv::Mat img = cv::imread((*it));

    if( img.empty() ){
      std::cerr << (*it) << " not loaded" << std::endl;
    }

    cv::Mat img_gray;
    
    imgutils::bgr2gray(img, img_gray);
    images.push_back( img_gray );
  }


  //////////////////////////////////////////////////////////////////
  //Computing Descriptors
  std::cout << "Computing Descriptors..." << std::endl;

  int dimension = 0;
  phog.computeDescriptor( images, descriptors, dimension, PHOG::L2NORM );

  //Allocate Matrix
  //float** tset   = new float*[descriptors.size()];
  float* tset    = new float[descriptors.size()*dimension];
  float* tlabels = new float[descriptors.size()];

  idx = 0;
  int d_it = 0;
  std::ofstream descriptorsFile("descriptors.dat");
  for( std::vector<float*>::iterator descriptor = descriptors.begin();
       descriptor != descriptors.end(); ++descriptor, idx++){

    tlabels[idx] = static_cast<float>( labels[idx] );

    for( int i = 0; i < dimension; i++){
      tset[d_it++]    = (*descriptor)[i];
      descriptorsFile << std::fixed << (*descriptor)[i] << " ";
    }
    descriptorsFile << std::endl;

  }
  descriptorsFile.close();

  cv::Mat cvTset(descriptors.size(), dimension, CV_32FC1, tset);
  cv::Mat cvLset(labels);

  ///////////////////////////////////////////////////////////////////
  //KNN Training
  //////////////////////////////////////////////////////////////////
  std::cout << "Training K-NN..." << std::endl;

  CvKNearest classifier;

  if( !classifier.train(cvTset, cvLset) ){
    std::cerr << "Cannot Train" << std::endl;
  }

  ////////////////////////
  //Test TrainSet
  idx = 0;
  int errors = 0;
  std::vector<std::string>::iterator file_it = files.begin();
  for( std::vector<float*>::iterator descriptor = descriptors.begin();
       descriptor != descriptors.end(); ++descriptor, idx++, ++file_it){
    float guess = classifier.find_nearest(cvTset.row(idx), 1);
    if( abs(static_cast<float>(labels[idx]) - guess) > 0.0001f  ){
      std::cerr << idx << " Expected: " << std::fixed <<  labels[idx] 
                << " Got: " << std::fixed << guess << std::endl;
      errors++;
    }
  }

  files.clear();
  images.clear();
  labels.clear();

  std::vector<float*>::iterator it_desc;
  for( it_desc = descriptors.begin(); it_desc != descriptors.end(); ++it_desc){
    delete [] (*it_desc);
  }

  descriptors.clear();
  delete [] tset;
  delete [] tlabels;

  ////////////////////////
  //Test
  ////////////////////////
  readDataSet( _testset, ".ppm", "person_and_bike_", files, labels, false);

  std::cout << "NumFiles: " << files.size() << std::endl;

  //Reading the Images
  idx = 0;
  for( std::vector<std::string>::const_iterator it = files.begin();
       it != files.end(); ++it, idx++){
    cv::Mat img = cv::imread((*it));

    if( img.empty() ){
      std::cerr << (*it) << " not loaded" << std::endl;
    }

    cv::Mat img_gray;
    
    imgutils::bgr2gray(img, img_gray);
    images.push_back( img_gray );
  }


  //////////////////////////////////////////////////////////////////
  //Computing Descriptors
  std::cout << "Computing Descriptors..." << std::endl;

  dimension = 0;
  phog.computeDescriptor( images, descriptors, dimension, PHOG::L2NORM );

  //Allocate Matrix
  tset    = new float[descriptors.size()*dimension];
  tlabels = new float[descriptors.size()];

  idx = 0;
  d_it = 0;
  for( std::vector<float*>::iterator descriptor = descriptors.begin();
       descriptor != descriptors.end(); ++descriptor, idx++){

    tlabels[idx] = static_cast<float>( labels[idx] );

    for( int i = 0; i < dimension; i++){
      tset[d_it++]    = (*descriptor)[i];
    }
  }

  cvTset = cv::Mat(descriptors.size(), dimension, CV_32FC1, tset);
  cvLset = cv::Mat(labels);


  //Testing
  //
  std::cout << "Testing TestSet" << std::endl;

  // 0 => 1
  // 1 => -1
  int class_errors[] = {0,0};

  idx = 0;
  errors = 0;
  file_it = files.begin();
  for( std::vector<float*>::iterator descriptor = descriptors.begin();
       descriptor != descriptors.end(); ++descriptor, idx++, ++file_it){
    float guess = classifier.find_nearest(cvTset.row(idx), 1);
    if( abs(static_cast<float>(labels[idx]) - guess) > 0.0001f  ){
      std::cerr << idx << " Expected: " << std::fixed <<  labels[idx] 
                << " Got: " << std::fixed << guess << std::endl;
      errors++;
      class_errors[ labels[idx] == 1 ? 0 : 1 ] += 1;
    }
  }

  std::ofstream reportFile(report.c_str());
  std::cout << "Errors: " << errors << std::endl;
  std::cout << "Class Bikers  " << class_errors[0] << std::endl;
  std::cout << "Class Persons " << class_errors[1] << std::endl;

  reportFile << "L: "    << L << std::endl;
  reportFile << "Size: " << descriptors.size() << std::endl;
  reportFile << "Errors: " << errors << std::endl;
  reportFile << "Class Bikers  " << class_errors[0] << std::endl;
  reportFile << "Class Persons " << class_errors[1] << std::endl;

  reportFile.close();

  //
  files.clear();
  images.clear();
  labels.clear();

  for( it_desc = descriptors.begin(); it_desc != descriptors.end(); ++it_desc){
    delete [] (*it_desc);
  }

  descriptors.clear();
  delete [] tset;
  delete [] tlabels;
}

