
#include <PHOG.h>
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
                         bool filter=true );

static void train(const std::string& tset, 
                  const std::string& xml_output);

static void testSVM(const std::string& xml,
                    const std::string& test_bed,
                    const std::string& report);

static inline void usage(void){
  std::cout << "./phogTest --<train|test> <xml> <dir>" << std::endl;
}


bool stringCompare( const std::string &left, const std::string &right ){
   for( std::string::const_iterator lit = left.begin(), rit = right.begin(); 
        lit != left.end() && rit != right.end(); ++lit, ++rit )
      if( tolower( *lit ) < tolower( *rit ) )
         return true;
      else if( tolower( *lit ) > tolower( *rit ) )
         return false;
   if( left.size() < right.size() )
      return true;
   return false;
}

static std::string trainStr = "--train";
static std::string testStr  = "--test";

static int L = 5;
static CvSVM classifier;
static PHOG phog(9, 180, L);

int main(int argc, char** argv){

  if( argc != 4 ){
    usage();
    return 1;
  }

  if( !trainStr.compare(argv[1]) ){
    train(argv[3], argv[2]);
  }else if( !testStr.compare(argv[1]) ){
    testSVM(argv[2], argv[3], "Statistics.dat");
  }


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
      
      //std::ofstream imgs("images.txt");
      //std::ofstream imgs("labels.txt");
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
            
            //std::cout << "Appending ... " << fullPath << std::endl; 
            //imgs << std::fixed << labels.back() << std::endl;
            images.push_back( fullPath );
          }
        }
      //imgs.close();
  }else{
    std::cerr << "Directory does not exists " << std::endl;
  }
  
  //Sort the files
  //sort(images.begin(), images.end(), stringCompare);
}

void train(const std::string& _tset, 
           const std::string& xml_output){
  std::vector<std::string> files;
  std::vector<cv::Mat> images;
  std::vector<float> labels;
  std::vector<float*> descriptors;

  readDataSet( _tset, ".ppm", "person_and_bike_", files, labels, false);

  std::cout << "NumFiles: " << files.size() << std::endl;

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

  //@@@ Find a good way to pass the allocated array to OpenCV
  cv::Mat cvTset(descriptors.size(), dimension, CV_32FC1, tset);
  //cv::Mat cvLset(labels.size(), 1, CV_32FC1, tlabels);
  cv::Mat cvLset(labels);

  //Testing Matrix
  std::cout << "cvTset: " << cvTset.rows << "x" << cvTset.cols << std::endl;
  std::cout << "Step: " << cvTset.step << std::endl;
  std::ofstream cvdescriptorsFile("cvdescriptors.dat");  

  for( int cv_y = 0; cv_y < cvTset.rows; cv_y++){
    for( int cv_x = 0; cv_x < cvTset.cols; cv_x++){
      cvdescriptorsFile << std::fixed << cvTset.at<float>(cv_y,cv_x) << " ";
    }
    cvdescriptorsFile << std::endl;
  }
  cvdescriptorsFile.close();

  ///////////////////////////////////////////////////////////////////
  //SVM Training
  //////////////////////////////////////////////////////////////////
  std::cout << "Training SVM..." << std::endl;

  CvSVMParams params;
  //params.svm_type = CvSVM::C_SVC;
  //params.kernel_type = CvSVM::LINEAR;
  //params.kernel_type = CvSVM::RBF;
  //params.gamma = 0.95;
  //params.C = 0.99;
  //params.svm_type = CvSVM::ONE_CLASS;
  //params.nu = 0.5;
  
  // //CvBoostParams params;
  // //params.boost_type =CvBoost::GENTLE;

  //CvSVM classifier;
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
  //if( !classifier.train(cvTset, cvLset, cv::Mat(), cv::Mat(), params) ){
    std::cerr << "Cannot Train" << std::endl;
  }

  params = classifier.get_params();

  std::cout << "C: "    << std::fixed << params.C << std::endl;
  std::cout << "Gamma: "<< std::fixed << params.gamma << std::endl;

  classifier.save(xml_output.c_str());

  //Test
  idx = 0;
  int errors = 0;
  std::ofstream outfile("train_errors.dat");
  std::vector<std::string>::iterator file_it = files.begin();
  for( std::vector<float*>::iterator descriptor = descriptors.begin();
       descriptor != descriptors.end(); ++descriptor, idx++, ++file_it){
    float guess = classifier.predict(cvTset.row(idx));
    if( abs(static_cast<float>(labels[idx]) - guess) > 0.0001f  ){
      std::cerr << idx << " Expected: " << std::fixed <<  labels[idx] 
                << " Got: " << std::fixed << guess << std::endl;
      errors++;
    }
    outfile << (*file_it) << " " << std::fixed << guess << std::endl;
  }
  outfile.close();

  std::cout << "Errors: " << errors << std::endl;
}

void testSVM(const std::string& xml,
             const std::string& test_bed,
             const std::string& report){
  //Loading 
  classifier.load(xml.c_str());

  CvSVMParams params = classifier.get_params();

  std::cout << "C: "    << std::fixed << params.C << std::endl;
  std::cout << "Gamma: "<< std::fixed << params.gamma << std::endl;

  std::vector<std::string> files;
  std::vector<cv::Mat> images;
  std::vector<float> labels;
  std::vector<float*> descriptors;

  readDataSet(test_bed, ".ppm", "person_and_bike_", files, labels, false);

  std::cout << "NumFiles: " << files.size() << std::endl;

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

  
  int dimension = 0;
  phog.computeDescriptor( images, descriptors, dimension, PHOG::L2NORM );

  //Allocate Matrix
  float** tset   = new float*[descriptors.size()];
  float* tlabels = new float[descriptors.size()];

  idx = 0;
  for( std::vector<float*>::iterator descriptor = descriptors.begin();
       descriptor != descriptors.end(); ++descriptor, idx++){

    tset[idx]    = (*descriptor);
    tlabels[idx] = static_cast<float>( labels[idx] );

  }

  float* lintset = imgutils::mat2linarray((const float**)tset, 
                                          dimension, descriptors.size());

  
  
  cv::Mat cvTset(descriptors.size(), dimension, CV_32F, lintset);
  cv::Mat cvLset(labels.size(), 1, CV_32FC1, tlabels);

  //Test
  idx = 0;
  int errors = 0;
  std::ofstream outfile("test_errors.dat");
  std::vector<std::string>::iterator file_it = files.begin();
  for( std::vector<float*>::iterator descriptor = descriptors.begin();
       descriptor != descriptors.end(); ++descriptor, idx++, ++file_it){
    float guess = classifier.predict(cvTset.row(idx));
    if( abs(static_cast<float>(labels[idx]) - guess) > 0.0001f  ){
      std::cerr << idx << " Expected: " << std::fixed <<  labels[idx] 
                << " Got: " << std::fixed << guess << std::endl;
      errors++;
    }
    outfile << (*file_it) << " " << std::fixed << guess << std::endl;
  }
  outfile.close();

  std::cout << "Errors: " << errors << std::endl;
}
