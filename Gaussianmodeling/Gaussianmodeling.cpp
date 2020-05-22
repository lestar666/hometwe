#include<opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int calcGaussianBackground(std::vector<cv::Mat> srcMat, cv::Mat & meanMat, cv::Mat &varMat)
{

	int rows = srcMat[0].rows;
	int cols = srcMat[0].cols;


	for (int h = 0; h < rows; h++)
	{
		for (int w = 0; w < cols; w++)
		{

			int sum = 0;
			float var = 0;
			
			for (int i = 0; i < srcMat.size(); i++) {
				sum += srcMat[i].at<uchar>(h, w);//求均值
			}
			meanMat.at<uchar>(h, w) = sum / srcMat.size();
			
			for (int i = 0; i < srcMat.size(); i++) {
				var += pow((srcMat[i].at<uchar>(h, w) - meanMat.at<uchar>(h, w)), 2);//求方差
			}
			varMat.at<float>(h, w) = var / srcMat.size();
		}
	}

	return 0;
}

int gaussianThreshold(cv::Mat srcMat, cv::Mat meanMat, cv::Mat varMat, float weight, cv::Mat & dstMat)
{
	int srcI;
	int meanI;
	int dstI;
	int rows = srcMat.rows;
	int cols = srcMat.cols;

	for (int h = 0; h < rows; h++)
	{
		for (int w = 0; w < cols; w++)
		{
			srcI = srcMat.at<uchar>(h, w);
			meanI = meanMat.at<uchar>(h, w);
			int dif = abs(srcI - meanI);
			int th = weight * varMat.at<float>(h, w);

			if (dif > th) {

				dstMat.at<uchar>(h, w) = 255;
			}
			else {
				dstMat.at<uchar>(h, w) = 0;
			}
		}
	}

	return 0;
}

int main()
{
	VideoCapture capVideo(0);
	if (!capVideo.isOpened()) {
		std::cout << "fail to open video" << std::endl;
		return -1;
	}

	std::vector<cv::Mat> srcMat; //存放背景模型的图像

	int nBg = 200;
	float wVar = 2;		
    //方差权重
	int cnt = 0;
	cv::Mat frame, meanMat,varMat,dstMat;

	while(true)
	{
		capVideo >> frame;
		cvtColor(frame, frame, COLOR_BGR2GRAY);

		//计算背景
		if (cnt < nBg) {

			srcMat.push_back(frame);

			if (cnt == 0) {
				std::cout << "reading frame " << std::endl;
			}

		}
		else if (cnt == nBg) {
			meanMat.create(frame.size(), CV_8UC1);
			varMat.create(frame.size(), CV_32FC1);
			std::cout << "calculating background models" << std::endl;
			calcGaussianBackground(srcMat, meanMat, varMat);
		}
		else {
			dstMat.create(frame.size(), CV_8UC1);
			gaussianThreshold(frame, meanMat, varMat, wVar, dstMat);
			imshow("result", dstMat);
			imshow("frame", frame);
			waitKey(30);
		}
		cnt++;
	}
	waitKey(0);
	return 0;
}

