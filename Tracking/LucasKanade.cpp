/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <LucasKanade.h>
#include <utils.h>

#include <vector>
#include <algorithm>

#ifdef DEBUG
#include <iostream>
#include <cstdio>
#include <ctime>
#endif

LucasKanade::LucasKanade( const cv::Mat& frame1, 
                          const cv::Mat& frame2,
                          const cv::Rect& _roi,
                          const unsigned int _maxCorners,
                          const cv::Point _compensation, 
                          const double _eigenValThreshold,
                          const double _k,
                          const double _minDistance,
                          enum KeyPointDetector _keyPointDetector ) : 
  Tracker(frame1, _roi), 
  maxCorners( _maxCorners ), 
  compensation(_compensation),
  eigenValThreshold(_eigenValThreshold), 
  k(_k), 
  minDistance(_minDistance),
  keyPointDetector(_keyPointDetector)
{
  
  prev = T;
  frame2.copyTo(current);

  dim = cv::Size(frame1.cols, frame1.rows);

  //mask      = cv::Mat::zeros(dim, CV_8U);
  //mask(roi) = cv::Scalar(255);

  //Try this with FAST
  if( keyPointDetector != FAST ){
    cv::goodFeaturesToTrack(prev, prev_corners, maxCorners, eigenValThreshold, 
                            minDistance, mask, 3, 
                            keyPointDetector == HARRIS, k);
  }else{
    //fastDetector.fast(prev, );
  }

  cv::calcOpticalFlowPyrLK(prev(roi), current(roi), prev_corners, corners, 
                           status, err, cv::Size(5,5), 1);

#ifdef DEBUG
  std::cout << "<<LK>> Corners Found: " << prev_corners.size() << std::endl;
  clock_t start, end;
  start = clock();
#endif

  imgutils::findSurroundingBox(corners, status, roi, dim, compensation);

#ifdef DEBUG
  end = clock();
  std::cout << "FindBox: "<< std::fixed 
            << double(end - start)/CLOCKS_PER_SEC << " [seg]" << std::endl;
#endif

  state = TRACKING;
}

LucasKanade::~LucasKanade(void){
  
}

CvRect LucasKanade::track( const IplImage* frame ){
  return track( cv::Mat(frame) );
}

CvRect LucasKanade::track( const cv::Mat& frame ){
  prev_corners.clear();
  prev_corners = corners;

  corners.clear();

  current.copyTo(prev);
  frame.copyTo(current);

  //mask      = cv::Scalar(0);
  //mask(roi) = cv::Scalar(255);

#ifdef DEBUG
  clock_t start, end;
  start = clock();
#endif

  // cv::goodFeaturesToTrack(current, corners, maxCorners, eigenValThreshold, 
  //                         minDistance, mask, 3, 
  //                         keyPointDetector == HARRIS, k);

  cv::goodFeaturesToTrack(current(roi), corners, maxCorners, eigenValThreshold, 
                          minDistance, cv::Mat(), 3, 
                          keyPointDetector == HARRIS, k);

#ifdef DEBUG
  end = clock();
  std::cout << "Extract Features: "<< std::fixed 
            << double(end-start)/CLOCKS_PER_SEC << " [seg]" << std::endl;
  start = clock();
#endif

  //Appy a ROI analysis
  std::vector<cv::Point2f>::iterator it;
  for( it = prev_corners.begin();
       it != prev_corners.end(); ++it){
    (*it).x -= roi.x;
    (*it).y -= roi.y;
  }

  cv::calcOpticalFlowPyrLK(prev(roi), current(roi), prev_corners, corners, 
                           status, err, cv::Size(5,5), 1,
                           cv::TermCriteria( cv::TermCriteria::COUNT + 
                                             cv::TermCriteria::EPS, 
                                             10, 1) );
  std::vector<cv::Point2f>::iterator it_next;
  for( it = prev_corners.begin(), it_next = corners.begin();
       it != prev_corners.end(); ++it, ++it_next){
    (*it).x      += roi.x;
    (*it).y      += roi.y;
    (*it_next).x += roi.x;
    (*it_next).y += roi.y;
  }

#ifdef DEBUG
  end = clock();
  std::cout << "OpticalFlow: "<< std::fixed 
            << double(end - start)/CLOCKS_PER_SEC << " [seg]" << std::endl;
#endif


#ifdef VISUAL
  current.copyTo(visual);
  std::vector<cv::Point2f>::const_iterator it;
  std::vector<unsigned char>::const_iterator it_status;  

  cv::Point p;
  for( it = corners.begin(), it_status = status.begin();
       it != corners.end(); ++it, ++it_status){

    if( (*it_status) ){
      p.x = static_cast<int>( (*it).x );
      p.y = static_cast<int>( (*it).y );
      cv::circle(visual, p, 3, cv::Scalar(255.0, 0,0, 0.0));
    }

  }

  cv::imshow("Visualize Corners", visual);
#endif

#ifdef DEBUG
  start = clock();
#endif

  imgutils::findSurroundingBox(corners, status, roi, dim, compensation);

#ifdef DEBUG
  end = clock();
  std::cout << "FindBox: "<< std::fixed 
            << double(end - start)/CLOCKS_PER_SEC << " [seg]" << std::endl;

#endif
  
  return roi;
}

