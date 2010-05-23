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

#define PI 3.1416

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
