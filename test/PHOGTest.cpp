
#include <PHOG.h>
#include <utils.h>
#include <iostream>
#include <ml.h>
#include <fstream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace boost::filesystem;

void readDataSet( const path& directory, 
                  std::vector<std::string>& images,
                  std::vector<float>& labels);

int main(int argc, char** argv){
  std::vector<std::string> files;
  std::vector<cv::Mat> images;
  std::vector<float> labels;
  std::vector<float*> descriptors;

  readDataSet( argv[1], files, labels );

  std::cout << "NumFiles: " << files.size() << std::endl;

  int idx = 0;
  for( std::vector<std::string>::const_iterator it = files.begin();
       it != files.end(); ++it, idx++){
    cv::Mat img = cv::imread((*it));
    cv::Mat img_gray;
    
    imgutils::bgr2gray(img, img_gray);
    images.push_back( img_gray );
  }

  PHOG hog(9, 180, 4);
  
  int dimension = 0;
  hog.computeDescriptor( images, descriptors, dimension, PHOG::L2NORM );

  //Allocate Matrix
  float** tset   = new float*[descriptors.size()];
  float* tlabels = new float[descriptors.size()];

  //std::ofstream file("desc.dat");

  idx = 0;
  for( std::vector<float*>::iterator descriptor = descriptors.begin();
       descriptor != descriptors.end(); ++descriptor, idx++){
    tset[idx]    = (*descriptor);
    tlabels[idx] = static_cast<float>( labels[idx] );

    // for( int i = 0; i < dimension; i++){
    //   file << std::fixed << tset[idx][i] << " ";
    // }
    // file << std::endl;

  }

  // file.close();
  
  cv::Mat cvTset(descriptors.size(), dimension, CV_32F, tset);
  cv::Mat cvLset(labels.size(), 1, CV_32FC1, tlabels);

  CvSVMParams params;
  params.kernel_type = CvSVM::RBF;
  params.gamma = 0.5;

  // //CvBoostParams params;
  // //params.boost_type =CvBoost::GENTLE;

  CvSVM classifier;
  // //CvBoost boost;
  // // CvANN_MLP nn;
  // //CvNormalBayesClassifier classifier;

  // // cv::Mat w;
  // // cv::Mat o;

  // if( !classifier.train(cvTset, cvLset, cv::Mat(), cv::Mat(), params) ){
  // // if( !boost.train(cvTset, CV_ROW_SAMPLE, cvLset, 
  // //                  cv::Mat(), cv::Mat(), cv::Mat(), cv::Mat(),
  // //                  params) ){
  // //if( !nn.train(cvTset, cvLset, w) ){
  if( !classifier.train(cvTset, cvLset) ){
    std::cerr << "Cannot Train" << std::endl;
  }

  // //boost.save("svmPersons.xml");

  // //Test
  idx = 0;
  int errors = 0;
  for( std::vector<float*>::iterator descriptor = descriptors.begin();
       descriptor != descriptors.end(); ++descriptor, idx++){
    //float guess = nn.predict(cvTset.row(idx), o);
    float guess = classifier.predict(cvTset.row(idx));
    if( abs(static_cast<float>(labels[idx]) - guess) > 0.0001f  ){
      std::cerr << idx << " Expected: " << std::fixed <<  labels[idx] 
                << " Got: " << std::fixed << guess << std::endl;
      errors++;
    }
  }

  std::cout << "Errors: " << errors << std::endl;

  return 0;
}

void readDataSet( const path& directory, 
                  std::vector<std::string>& images,
                  std::vector<float>& labels){
  if( exists( directory ) ){   
      directory_iterator end;
      std::string file;
      unsigned int pos = 0;
      std::string fullPath;
      
      for( directory_iterator iter(directory) ; iter != end ; ++iter )        
        if ( !is_directory( *iter ) ){
          file = iter->path().leaf();
          pos = file.find(".ppm");
          
          if( pos != std::string::npos){
            fullPath = directory.string() + file;

            pos = file.find("obj");

            if( pos != std::string::npos ){
              labels.push_back(-1.0f);
            }else{
              labels.push_back(1.0f);              
            }
            
            //std::cout << "Appending ... " << fullPath << std::endl; 
            images.push_back( fullPath );
          }
        }
  }else{
    std::cerr << "Directory does not exists " << std::endl;
  }
  
}
