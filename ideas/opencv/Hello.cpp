#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <algorithm>


using namespace std;
using namespace cv;


bool sort_contours(const vector< Point> &c1, const vector< Point> &c2)
{
	auto r1 = boundingRect(Mat(c1));
	auto r2 = boundingRect(Mat(c2));

	return r1.x < r2.x;
}

Mat histeq(Mat in)                                                                                                                                                            
{
    Mat out(in.size(), in.type());
    if(in.channels()==3){
        Mat hsv;
        vector<Mat> hsvSplit;
        cvtColor(in, hsv, CV_BGR2HSV);
        split(hsv, hsvSplit);
        equalizeHist(hsvSplit[2], hsvSplit[2]);
        merge(hsvSplit, hsv);
        cvtColor(hsv, out, CV_HSV2BGR);
    }else if(in.channels()==1){
        equalizeHist(in, out);
    }

    return out;
}

Mat preprocessChar(Mat in){
	Mat grayResult;
	cvtColor(in, grayResult, CV_BGR2GRAY);
	blur(grayResult, grayResult, Size(3,3));
	grayResult=histeq(grayResult);

	Mat thresh_img;
	threshold(grayResult, thresh_img, 90, 255, CV_THRESH_BINARY_INV);
	
/*
	imshow("thresh_img", thresh_img);
	waitKey(0);
*/

    //Remap image
    int h=thresh_img.rows;
    int w=thresh_img.cols;
    Mat transformMat=Mat::eye(2,3,CV_32F);
    int m=max(w,h);
    transformMat.at<float>(0,2)=m/2 - w/2;
    transformMat.at<float>(1,2)=m/2 - h/2;

    Mat warpImage(m,m, thresh_img.type());
    warpAffine(thresh_img, warpImage, transformMat, warpImage.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(0) );

    Mat out;
    resize(warpImage, out, Size(20, 20) );
	return out;


}

bool verifyRR(RotatedRect r)
{
	auto w = r.size.width;
	auto h = r.size.height;

	return true;
	if (w*h < 300) return false;

	return true;
}

vector<Mat> segment(Mat in)
{
	vector<Mat> s;

	//灰度
	Mat img_gray;
	cvtColor(in, img_gray, CV_BGR2GRAY);
	blur(img_gray, img_gray, Size(5,5));
/*
	imshow("test", image);
	waitKey(0);
*/
	cout << "test2" << endl;
	//阀值
	Mat img_threshold;
	threshold(img_gray, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY_INV);
	cout << "test3" << endl;
	//轮廓
	vector< vector< Point> > contours;
	findContours(img_threshold, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	cout << "test4" << endl;
	//画轮廓
	Mat result;
	in.copyTo(result);
	drawContours(result, contours, -1, Scalar(255,0,0), 1);
	cout << "test5" << endl;

	imshow("test", result);
	waitKey(0);

	sort(contours.begin(), contours.end(), sort_contours);

	auto itc= contours.begin();
	while (itc!=contours.end()) {
		//旋转
		auto mr= minAreaRect(Mat(*itc));
		if (verifyRR(mr)) {
			auto rotmat= getRotationMatrix2D(mr.center, mr.angle, 1);
			Mat img_rotated;
			warpAffine(in, img_rotated, rotmat, in.size(), CV_INTER_CUBIC);
/*
			imshow("img_rotated", img_rotated);
			waitKey(0);
*/
			//裁剪
			auto rect_size=mr.size;
			Mat img_crop;
			getRectSubPix(img_rotated, rect_size, mr.center, img_crop);

/*
			imshow("img_crop", img_crop);
			waitKey(0);
*/


			auto normal_crop = preprocessChar(img_crop);
			s.push_back(normal_crop);
/*
			cout << "before show" << endl;
			imshow("normal_crop", normal_crop);
			waitKey(0);
			cout << "after show" << endl;
*/

		}

		itc++;
	}


	return s;
}

void predict(CvSVM& svm)
{
	auto image = imread("/home/vagrant/opensource/ngx_openresty-1.7.10.2/test/ideas/opencv/data/input.png");
	auto s = segment(image);

	for (auto &ch : s) {
		auto m = ch.reshape(1, 1);
		m.convertTo(m, CV_32FC1);
		auto response = (int)svm.predict(m);

		cout << response << endl;
	}
}


void train()
{
	Mat SVM_TrainingData;
	Mat SVM_Classes;
	for (int i = 0; i <= 9; i++) {
		stringstream ss;
		ss << "/home/vagrant/opensource/ngx_openresty-1.7.10.2/test/ideas/opencv/data/";
		ss << i;
		ss << ".png";
		auto img_path = ss.str();
		auto image = imread( img_path, CV_LOAD_IMAGE_COLOR);
		cout << img_path << endl;
		cout << "test1" << endl;

		auto s = segment(image);
		for (auto &ch : s) {
			auto m = ch.reshape(1, 1);
			m.convertTo(m, CV_32FC1);
			SVM_TrainingData.push_back(m);
			SVM_Classes.push_back(i);
		}
	}

	//Set SVM params
    CvSVMParams SVM_params;
    SVM_params.svm_type = CvSVM::C_SVC;
    SVM_params.kernel_type = CvSVM::LINEAR; //CvSVM::LINEAR;
    SVM_params.degree = 0;
    SVM_params.gamma = 1;
    SVM_params.coef0 = 0;
    SVM_params.C = 1;
    SVM_params.nu = 0;
    SVM_params.p = 0;
    SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.01);
    //Train SVM
    CvSVM svmClassifier(SVM_TrainingData, SVM_Classes, Mat(), Mat(), SVM_params);

	predict(svmClassifier);
}

int main()
{
	train();
}
