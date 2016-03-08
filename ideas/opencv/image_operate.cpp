#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <unistd.h>


using namespace cv;
using namespace std;

int thresh = 55;
RNG rng(12345);


void gray_thresh(char* imageName, Mat& gray_image)
{
	Mat image;
	image = imread( imageName, CV_LOAD_IMAGE_COLOR);

	if( !image.data )
	{
		printf( " No image data \n " );
		exit(-1);
	}

	cvtColor( image, gray_image, CV_BGR2GRAY );

	threshold( gray_image, gray_image, thresh, 255, THRESH_BINARY );

	imwrite( "/vagrant/cv/Gray.jpg", gray_image );
}

void canny(Mat& gray_image)
{
	Mat canny_output;

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// 用Canny算子检测边缘
  	Canny( gray_image, canny_output, thresh, thresh*2, 3 );
  	/// 寻找轮廓
  	findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	/// 绘出轮廓
  	Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
  	for(unsigned int i = 0; i< contours.size(); i++ )
    {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }
	imwrite( "/vagrant/cv/Canny.jpg", drawing );
}

void bound(Mat& gray_image)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// 找到轮廓
	findContours( gray_image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	/// 多边形逼近轮廓 + 获取矩形和圆形边界框
	vector<vector<Point> > contours_poly( contours.size() );
	vector<Rect> boundRect( contours.size() );
	vector<Point2f>center( contours.size() );
	vector<float>radius( contours.size() );

	for(unsigned int i = 0; i < contours.size(); i++ )
	{
		approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
		boundRect[i] = boundingRect( Mat(contours_poly[i]) );
		minEnclosingCircle( contours_poly[i], center[i], radius[i] );
	}


	/// 画多边形轮廓 + 包围的矩形框 + 圆形框
	Mat drawing = Mat::zeros( gray_image.size(), CV_8UC3 );
	for(unsigned int i = 0; i< contours.size(); i++ )
	{
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
		rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
		circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
	}

	imwrite( "/vagrant/cv/Bound.jpg", drawing );

}

int main( int argc, char** argv )
{
	if(argc != 2)
	{
		printf("useage: %s <imagefile>\n ", argv[0]);
		return -1;
	}

	Mat gray_image;
	gray_thresh(argv[1], gray_image);

	canny(gray_image);

	bound(gray_image);


	return 0;
}
