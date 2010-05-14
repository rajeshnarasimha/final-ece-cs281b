/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <utils.h>

using namespace cv;

void imgutils::buildRGBHistogram(const IplImage* patch, 
                                 MatND& histogram, 
                                 const int& bins,
                                 Mat& mask){
  int  histSize[] = {bins, bins, bins};
  float   range[] = {0.0f, 255.0f};
  const float* ranges[] = { range, 
                            range,
                            range
  };
  
  const int  channels[] = {0,1,2};
  
  calcHist(reinterpret_cast<const Mat*>(patch), 
           1, 
           reinterpret_cast<const int*>(channels), 
           mask,
           histogram, 3, 
           reinterpret_cast<const int*>(histSize), 
           reinterpret_cast<const float**>(ranges) );
  
}

void imgutils::buildHsvHistogram(const IplImage* patch, 
                                 MatND& histogram,
                                 const int& bins,
                                 Mat& mask, 
                                 const Scalar& hsmin, 
                                 const Scalar& hsmax,
                                 const Rect& roi){
  Mat hsv;
  cvtColor(patch, hsv, CV_BGR2HSV);

  Mat hsv_roi = Mat(hsv, roi);
  Mat maskROI = Mat(mask, roi);

  inRange(hsv_roi, hsmin, hsmax, maskROI);

  int  histSize[] = {bins, bins};
  float h_range[] = {0.0f, 180.0f};
  float s_range[] = {0.0f, 256.0f};
  const float* ranges[] = { h_range, s_range };
  const int  channels[] = {0,1};
  
  calcHist(&hsv, 
           1, 
           reinterpret_cast<const int*>(channels), 
           mask,
           histogram, 2, 
           reinterpret_cast<const int*>(histSize), 
           reinterpret_cast<const float**>(ranges) );

  //Normalizing the Histogram
  double maxVal;
  minMaxLoc(histogram, 0, &maxVal,0, 0);
  histogram.convertTo(histogram, histogram.type(), 
                      maxVal ? 255 / maxVal : 0.0 , 0);

#ifdef VISUAL
  int scale = 1;
  Mat histImg = Mat::zeros(bins*scale, bins*scale, CV_8UC3);

  for( int i = 0; i < bins; i++){
    for(int j = 0; j < bins; j++){
      float binVal = histogram.at<float>(i, j);
      int intensity= cvRound(binVal*255/maxVal);
      IplImage histImgIpl = histImg;
      cvRectangle( &histImgIpl, Point(i*scale, j*scale),
                         Point( (i+1)*scale - 1, (j+1)*scale - 1),
                         Scalar::all(intensity),
                         CV_FILLED );
    }
  }

  imshow( "H-S Histogram", histImg );
  waitKey();
#endif
}

void imgutils::buildHueHistogram(const IplImage* patch, 
                                 MatND& histogram,
                                 const int& bins,
                                 Mat& mask,
                                 const Scalar& hsmin, 
                                 const Scalar& hsmax,
                                 const Rect& roi){
  Mat hsv;
  cvtColor(patch, hsv, CV_BGR2HSV);

  Mat maskROI = Mat(mask, roi);
  Mat hsv_roi = Mat(hsv, roi);

  inRange(hsv_roi, hsmin, hsmax, maskROI);
  
  int  histSize[] = {bins};
  float h_range[] = {0.0f, 180.0f};
  const float* ranges[] = { h_range };
  const int  channels[] = {0};
  
  calcHist(&hsv, 
           1, 
           reinterpret_cast<const int*>(channels), 
           mask,
           histogram, 1, 
           reinterpret_cast<const int*>(histSize), 
           reinterpret_cast<const float**>(ranges) );

  //Normalizing the Histogram
  double maxVal;
  minMaxLoc(histogram, 0, &maxVal,0, 0);
  histogram.convertTo(histogram, histogram.type(), 
                      maxVal ? 255 / maxVal : 0.0 , 0);
  
#ifdef VISUAL
  int scale = 1;
  Mat histImg = Mat::zeros(int(maxVal*scale), bins*scale, CV_8UC3);

  for( int i = 0; i < bins; i++){
      float binVal = histogram.at<float>(i);
      int intensity= cvRound(binVal*255/maxVal);
      IplImage histImgIpl = histImg;
      cvRectangle( &histImgIpl, Point(i*scale, bins*scale),
                   Point( (i+1)*scale - 1,( bins - binVal )*scale),
                   Scalar::all(intensity),
                   CV_FILLED );
  }

  imshow( "Hue Histogram", histImg );
  waitKey();
#endif

}

void imgutils::computeBackProjectionRGB(const Mat& T,
                                        const MatND& histogram, 
                                        Mat& backProj){
  float   range[] = {0.0f, 255.0f};
  const float* ranges[] = { range, 
                            range,
                            range
  };
  
  const int  channels[] = {0,1,2};

  calcBackProject(&T, 3, 
                  reinterpret_cast<const int*>(channels), 
                  histogram, backProj, 
                  reinterpret_cast<const float**>(ranges));
  
}

void imgutils::computeBackProjectionHSV(const Mat& T,
                                        const MatND& histogram, 
                                        Mat& backProj,
                                        Mat& mask,
                                        const Scalar& hsmin,
                                        const Scalar& hsmax){

  Mat hsv;
  cvtColor(T, hsv, CV_BGR2HSV);
  
  inRange(hsv, hsmin, hsmax, mask);

  //Compute Confidence Map i.e. prob distribution over ROI
  const int  channels[] = {0, 1};  
  float h_range[] = {0.0f, 180.0f};
  float s_range[] = {0.0f, 180.0f};
  const float* ranges[] = { h_range, s_range };

  calcBackProject(&hsv, 2, 
                  reinterpret_cast<const int*>(channels), 
                  histogram, backProj, 
                  reinterpret_cast<const float**>(ranges));
  //1/norm);

#ifdef VISUAL
  IplImage temp = backProj;
  IplImage* scaled = cvCreateImage(cvGetSize(&temp), temp.depth, temp.nChannels);
  cvCvtScale(&temp, scaled, 1.0);
  imshow("BackProj",scaled);
  cvWaitKey();
  cvReleaseImage(&scaled);
#endif
  
}

void imgutils::computeBackProjectionHue(const Mat& T, //Hue image plane
                                        const MatND& histogram, 
                                        Mat& backProj,
                                        Mat& mask,
                                        const Scalar& hsmin,
                                        const Scalar& hsmax){
  Mat hsv;
  cvtColor(T, hsv, CV_BGR2HSV);

  inRange(hsv, hsmin, hsmax, mask);
  
  //Compute Confidence Map i.e. prob distribution over ROI
  const int  channels[] = {0};  
  float h_range[] = {0.0f, 180.0f};
  const float* ranges[] = { h_range };
  calcBackProject(&hsv, 1, 
                  reinterpret_cast<const int*>(channels), 
                  histogram, backProj, 
                  reinterpret_cast<const float**>(ranges));
  //1/norm);

#ifdef VISUAL
  IplImage temp = backProj;
  IplImage* scaled = cvCreateImage(cvGetSize(&temp), temp.depth, temp.nChannels);
  cvCvtScale(&temp, scaled, 1.0);
  imshow("BackProj",scaled);
  cvWaitKey();
  cvReleaseImage(&scaled);
#endif

}

void imgutils::maskFrameGray(const Mat& frame, const Mat& src2, Mat& maskedFrame){
  Mat gray;
  cvtColor(frame, gray, CV_BGR2GRAY);
  bitwise_and(gray, src2, maskedFrame);
}

void imgutils::maskFrameGray(const Mat& frame, const Rect& roi, Mat& maskedFrame){
  Mat gray;
  cvtColor(frame, gray, CV_BGR2GRAY);
  maskedFrame = Scalar(0);
  maskedFrame += Mat(gray, roi);
}


void imgutils::maskFrameRGB(const Mat& frame, const Mat& src2, Mat& maskedFrame){
  std::vector<cv::Mat> planes;
  std::vector<cv::Mat> masked_planes;    

  masked_planes.push_back(Mat::zeros(frame.rows, frame.cols, CV_8U));
  masked_planes.push_back(Mat::zeros(frame.rows, frame.cols, CV_8U));
  masked_planes.push_back(Mat::zeros(frame.rows, frame.cols, CV_8U));

  cv::split(frame, planes);
  cv::bitwise_and(planes[0], src2, masked_planes[0]);
  cv::bitwise_and(planes[1], src2, masked_planes[1]);
  cv::bitwise_and(planes[2], src2, masked_planes[2]);

  cv::merge(masked_planes, maskedFrame);
}


