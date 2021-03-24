#include "ImageToVideo_src.h"
#include<opencv2\opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

void ImageToVideo_src::ItoV(){//cv::String img_path){
	VideoWriter video("D:\\Pictures2\\test2.mp4", CAP_OPENCV_MJPEG, 1.0, Size(1080, 1080));
	//��һ��������ʾ��ȡ����Ƶ֡����ŵ��µ��ļ�
	//�ڶ�������ָ������Ƶ��ŵı����ʽ
	//fps��ʾÿ���֡��
	//size��ʾͼ��ĳ����С
	String img_path = "D:\\Pictures2\\*.jpg";
	vector<String> img;
	glob(img_path, img, false);//�����ļ����µ�����ͼƬ
	size_t count = img.size();
	for (size_t i = 0; i < count; i++){
		Mat image = imread(img[i]);
		resize(image, image, Size(1080, 1080));//resize�����ı�ͼ���С
		video << image;
	}
	//cout << "������ϣ�" << endl;
}

