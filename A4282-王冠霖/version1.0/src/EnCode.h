#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<vector>

using namespace std;
using namespace cv;

enum PixelColor {
	WHITE, BLACK
};//0代表白色 1代表黑色

class EnCode{
public:
	int FrameSize = 108;//帧大小(不是实际分辨率)
	int FrameOutputRate = 10;//
	int Draw(Mat& image, Point Start, Point End, const char* filePath);
	Mat ScaleToDisSize(const Mat& src);
};

