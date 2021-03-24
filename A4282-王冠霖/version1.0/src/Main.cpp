#include "EnCoder.h"
#include "ConvertImgToVideo.h"
#include "ImageToVideo_src.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<vector>



using namespace std;
using namespace cv;


int main(){
	Mat Image(201,201, CV_8UC3, Scalar(255, 255, 255));//Scalar函数用于设置灰度值 画布背景为白色
	//CV_8UC3 参见: blog.csdn.net/weixin_35695879/article/details/85948011
	// Scalar 参见: blog.csdn.net/zqx951102/article/details/82797484

	//EnCode DrawPic;
	//DrawPic.Draw(Image,Point(12,12),Point(120,120),"D:\\计算机资料\\计算机网络\\计算机网络 项目库\\Project01\\test\\in.bin");
	//DrawPic.ScaleToDisSize(Image);
	//imshow("PixelCode",Image);
	//waitKey(10000);

		encoder Encoder;
		char filename[] = "./bin99.bin";
		char outfilename[] = "./Test_video.mp4";
		char video_len[] = "10000";
		Encoder.encode(filename, outfilename, video_len);


	//ConvertItoV ItoV;
	//ItoV.ImgsToVideo();

	//ImageToVideo_src itoV;
	//itoV.ItoV();
	return 0;
}



//int DealFile(const char* filePath) {
//	FILE* fp = fopen(filePath, "rb"); //读二进制文件打开文件路径
//	if (fp == nullptr) return 1; //打开失败返回1
//	fseek(fp, 0, SEEK_END); //从文件的末尾(0偏移)查找
//	int size = ftell(fp); //返回当前光标位置,此处即为文件末尾,利于得出整个文件的大小
//	rewind(fp); //设置文件位置为给fp指向文件的开头
//	char* temp = (char*)malloc(sizeof(char) * size); //建立动态数组temp,大小为文件的大小size×char型长度
//	if (temp == nullptr) return 1; //temp指空则返回1
//	fread(temp, 1, size, fp); //把fp指向的文件中的数据保存到temp数组中
//	fclose(fp); //关闭文件
//
//	return 0;
//}