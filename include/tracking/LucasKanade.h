/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#ifndef __LK_OFLOW__
#define __LK_OFLOW__

#include <Tracker.h>
#include <vector>

#include <FASTCVWrapper.h>

enum KeyPointDetector{
  SHITOMASI,
  HARRIS,
  FAST
};

class LucasKanade : public Tracker{
public:

  //@@@ TODO: Check if I can add another KeyPoint detector
  LucasKanade(const cv::Mat& frame1, 
              const cv::Mat& frame2,
              const cv::Rect& roi, 
              const unsigned int maxCorners,
              const cv::Point compensation,
              const double eigenValThreshold,
              const double k=0.04,
              const double minDistance=1.0,
              enum KeyPointDetector keyPointDetector=SHITOMASI);
              
  ~LucasKanade(void);

  CvRect track( const IplImage* frame );
  CvRect track( const cv::Mat& frame  );

  cv::Rect getLocation(void){
    return roi;
  }

private:
  cv::Rect location;

  //Frames
  cv::Mat prev, current;
  cv::Mat mask;


  //Corners
  std::vector<cv::Point2f> prev_corners, corners;

  std::vector<unsigned char> status;
  std::vector<float> err;

  const unsigned int maxCorners;   //MaxCorners
  const cv::Point compensation;    //Compensation
  const double eigenValThreshold;  //Strength of the eigenValues
  const double k; //Value for Harris (eigenvalues)
  const double minDistance; //Distance between corners


  const enum KeyPointDetector keyPointDetector;

  cv::Size dim;

  FASTCVWrapper fastDetector;

#ifdef VISUAL
  cv::Mat visual;
#endif

};

#endif
