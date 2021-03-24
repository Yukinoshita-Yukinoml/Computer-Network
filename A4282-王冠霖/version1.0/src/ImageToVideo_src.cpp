#include "ImageToVideo_src.h"
#include<opencv2\opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

void ImageToVideo_src::ItoV(){//cv::String img_path){
	VideoWriter video("D:\\Pictures2\\test2.mp4", CAP_OPENCV_MJPEG, 1.0, Size(1080, 1080));
	//第一个参数表示读取的视频帧所存放的新的文件
	//第二个参数指的是视频存放的编码格式
	//fps表示每秒的帧数
	//size表示图像的长宽大小
	String img_path = "D:\\Pictures2\\*.jpg";
	vector<String> img;
	glob(img_path, img, false);//遍历文件夹下的所有图片
	size_t count = img.size();
	for (size_t i = 0; i < count; i++){
		Mat image = imread(img[i]);
		resize(image, image, Size(1080, 1080));//resize函数改变图像大小
		video << image;
	}
	//cout << "处理完毕！" << endl;
}

