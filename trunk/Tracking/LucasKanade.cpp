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

  //Try this with FAST
  if( keyPointDetector != FAST ){
    cv::goodFeaturesToTrack(prev, prev_corners, maxCorners, eigenValThreshold, 
                            minDistance, mask, 3, 
                            keyPointDetector == HARRIS, k);
  }else{
    //fastDetector.fast(prev, );
  }

  std::vector<cv::Point2f>::iterator it;
  for( it = prev_corners.begin();
       it != prev_corners.end(); ++it){
    (*it).x -= roi.x;
    (*it).y -= roi.y;
  }

  cv::calcOpticalFlowPyrLK(prev(roi), current(roi), prev_corners, corners, 
                           status, err, cv::Size(5,5), 2);

  std::vector<cv::Point2f>::iterator it_next;
  for( it = prev_corners.begin(), it_next = corners.begin();
       it != prev_corners.end(); ++it, ++it_next){
    (*it).x      += roi.x;
    (*it).y      += roi.y;
    (*it_next).x += roi.x;
    (*it_next).y += roi.y;
  }

#ifdef DEBUG
  std::cout << "<<LK>> Corners Found: " << prev_corners.size() << std::endl;
#endif

  centroid.x = (roi.width  /2) + roi.x;
  centroid.y = (roi.height /2) + roi.y;

  //computeThOutliers();

  imgutils::findSurroundingBox(corners, status, roi, dim, compensation);

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

  cv::goodFeaturesToTrack(current(roi), corners, maxCorners, eigenValThreshold, 
                          minDistance, cv::Mat(), 3, 
                          keyPointDetector == HARRIS, k);

  //Applying a ROI analysis i.e. changing the coordinate system
  //This improves the computation
  std::vector<cv::Point2f>::iterator it;
  for( it = prev_corners.begin();
       it != prev_corners.end(); ++it){
    (*it).x -= roi.x;
    (*it).y -= roi.y;
  }

  cv::calcOpticalFlowPyrLK(prev(roi), current(roi), prev_corners, corners, 
                           status, err, cv::Size(5,5), 2,
                           cv::TermCriteria( cv::TermCriteria::COUNT + 
                                             cv::TermCriteria::EPS, 
                                             10, 1) );

  //Resetting coordinate system
  std::vector<cv::Point2f>::iterator it_next;
  for( it = prev_corners.begin(), it_next = corners.begin();
       it != prev_corners.end(); ++it, ++it_next){
    (*it).x      += roi.x;
    (*it).y      += roi.y;
    (*it_next).x += roi.x;
    (*it_next).y += roi.y;
  }

  centroid.x = (roi.width  /2) + roi.x;
  centroid.y = (roi.height /2) + roi.y;

  //Remove Outliers
  //removeOutliers();

#ifdef VISUAL
  current.copyTo(visual);
  std::vector<cv::Point2f>::const_iterator it_corners;
  std::vector<unsigned char>::const_iterator it_status;  

  cv::Point p;
  for( it_corners = corners.begin(), it_status = status.begin();
       it_corners != corners.end(); ++it_corners, ++it_status){

    if( (*it_status) ){
      p.x = static_cast<int>( (*it_corners).x );
      p.y = static_cast<int>( (*it_corners).y );
      cv::circle(visual, p, 3, cv::Scalar(255.0, 0,0, 0.0));
    }

  }

  cv::imshow("Visualize Corners", visual);
#endif

  imgutils::findSurroundingBox(corners, status, roi, dim, compensation);

  return roi;
}

void LucasKanade::computeThOutliers(void){
  //Compute the Variance
  cv::Point2f p;
  double dot = 0.0f;
  thOutliers = 0.0;
  for( std::vector<cv::Point2f>::const_iterator it = corners.begin();
       it != corners.end(); ++it){
    p   = (*it) - centroid;
    dot = static_cast<double>( p.dot(p) );
    thOutliers = std::max(dot, thOutliers);
  }

  std::cout << std::fixed << thOutliers <<std::endl;
}

void LucasKanade::removeOutliers(void){
  cv::Point2f p;
  double dot = 0.0f;

  std::vector<cv::Point2f> c;
  for( std::vector<cv::Point2f>::iterator it = corners.begin();
       it != corners.end(); ++it){
    p = (*it) - centroid;
    dot = static_cast<double>(p.dot(p));

    if( dot <= thOutliers){
      c.push_back( *it );
    }
    
  }

  corners.clear();
  corners = c;

}
