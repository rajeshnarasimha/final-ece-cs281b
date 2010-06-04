/**
 * University of California, Santa Barbara
 * HumanActivity Recognition {Walking,Biking}
 *
 * Authors: 
 * Norma S. Savage   <saiph@cs.ucsb.edu>
 * Victor M. Fragoso <vfragoso@cs.ucsb.edu>
 */

#include <utils.h>
#include <algorithm>

using namespace cv;

bool xDiff(cv::Point i, cv::Point j);
bool yDiff(cv::Point i, cv::Point j);

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

void imgutils::findSurroundingBox(const std::vector<cv::Point2f>& corners, 
                                  const std::vector<unsigned char>& status,
                                  cv::Rect& roi, const cv::Size& dim,
                                  const cv::Point& compensation){
  std::vector<cv::Point2f>::const_iterator it;
  std::vector<unsigned char>::const_iterator it_status;  
  
  std::vector<unsigned int> x_components;
  std::vector<unsigned int> y_components;
  
  for( it = corners.begin(), it_status = status.begin();
       it != corners.end(); ++it, ++it_status){
    if( (*it_status) ){//Push onto the vectors
      x_components.push_back( static_cast<unsigned int>( (*it).x ) );
      y_components.push_back( static_cast<unsigned int>( (*it).y ) );
    }
  }

  //Sort the components
  if( !x_components.empty() && !y_components.empty() ){
    std::sort(x_components.begin(), x_components.end());
    std::sort(y_components.begin(), y_components.end());

    //Beware of the boundaries
    roi.x = x_components[0] - compensation.x;
    roi.x = roi.x < 0 ? 0 : roi.x;

    roi.y = y_components[0] - compensation.y;
    roi.y = roi.y < 0 ? 0 : roi.y;

    roi.width  = x_components.back() - roi.x + 2*compensation.x;
    roi.width  = (roi.x + roi.width < dim.width) ? 
      roi.width : dim.width - roi.x - 1;

    roi.height = y_components.back() - roi.y + 2*compensation.y;
    roi.height = (roi.y + roi.height < dim.height) ? 
      roi.height : dim.height - roi.y - 1 ;

  }
}

bool xDiff(cv::Point i, cv::Point j) { 
  return (i.x < j.x); 
}

bool yDiff(cv::Point i, cv::Point j) { 
  return (i.y < j.y); 
}


void imgutils::detectBlobs( cv::Mat& frame,
                            std::vector<cv::Rect>& rois,
                            const cv::Point& offset,
                            const unsigned int minAreaThreshold,
                            const unsigned int maxAreaThreshold,
                            const cv::Size& size,
                            const double minAspectRatio,
                            const double maxAspectRatio ){
  std::vector< std::vector<cv::Point> > contours;
  cv::findContours(frame, contours, CV_RETR_EXTERNAL, 
                   CV_CHAIN_APPROX_SIMPLE, offset);

  
  std::vector< std::vector<cv::Point> >::iterator row_it;
  std::vector<cv::Point>::iterator contour;

  for( row_it = contours.begin(); row_it != contours.end(); ++row_it){
    std::sort( (*row_it).begin(), (*row_it).end(), xDiff);
    int x  = std::max( std::min( (*row_it)[0].x - offset.x, size.width - 1 ), 
                       0);
    int dx = std::min( (*row_it).back().x - x + offset.x + 1, size.width - x - 1);

    //Validating that does not goes out of bound

    std::sort( (*row_it).begin(), (*row_it).end(), yDiff);
    int y  = std::max( std::min( (*row_it)[0].y - offset.y, size.height - 1), 
                       0);
    int dy = std::min( (*row_it).back().y - y + offset.y + 1, size.height - y - 1);

    //@@@ TODO: See if the blobs are close to each other
    //Merge them if necessary
    //This would be using a Proximity algorithm i.e. Voronoi ...

    unsigned int area = dx*dy;
    if( area < minAreaThreshold ){
      continue;
    }

    if( area > maxAreaThreshold ){
      continue;
    }

    if( dx > 0 && dy > 0 ){
      double ar = double(dy) / double(dx);
      if( ar < minAspectRatio || ar > maxAspectRatio )
        continue;
    }

    cv::Rect roi(x,y,dx,dy);
    rois.push_back(roi);
  }
}


