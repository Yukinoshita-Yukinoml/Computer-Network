#include "EnCode.h"

int EnCode::Draw(Mat& image, Point Start, Point End, const char* filePath) {
	/// <summary>
	/// 对文件的处理
	FILE* fp = fopen(filePath, "rb"); //读二进制文件打开文件路径
	if (fp == nullptr) return 1; //打开失败返回1
	fseek(fp, 0, SEEK_END); //从文件的末尾(0偏移)查找
	int size = ftell(fp); //返回当前光标位置,此处即为文件末尾,利于得出整个文件的大小
	rewind(fp); //设置文件位置为给fp指向文件的开头
	int* temp = (int*)malloc(sizeof(int) * size); //建立动态数组temp,大小为文件的大小size×char型长度
	if (temp == nullptr) return 1; //temp指空则返回1
	fread(temp, 1, size, fp); //把fp指向的文件中的数据保存到temp数组中

	///
	int k = 0;
	//image 是图像对象 Start-End 代表起始位置
	//Scalar(0)---黑色  Scalar(255)---白色
	int Column = 177, Row = 177;//二维码Version 的最大值是 40，故尺寸最大值是(40-1)*4+21 = 177，即 177 x 177 的矩阵 再大效果会很差 尺寸问题此后可以通过学长ScaleToDisSize()的方法实现
	for (int y = 0; y < Row; y++) {
		for (int x = 0; x < Column; x++) {
			//获取像素
			Vec3b color = image.at<Vec3b>(Point(x, y));

			//处理颜色
			if (temp[k] == BLACK) {
				color[0] = 0;
				color[1] = 0;
				color[2] = 0;//黑色
				image.at<Vec3b>(Point(x, y)) = color;
			}
			else {
				color[0] = 255;
				color[1] = 255;
				color[2] = 255;
				image.at<Vec3b>(Point(x, y)) = color;
			}//白色
			 k++;
			//设置像素
			//image.at<Vec3b>(Point(x, y)) = color;
		}//for column
	}//for row
	//FILE* DesFile = fopen("D:\\计算机资料\\计算机网络\\计算机网络 项目库\\Project01\\test\\test.jpg","a");

	imwrite("D:\\计算机资料\\计算机网络\\计算机网络 项目库\\Project01\\test\\test.jpg", image);//保存图片
	//imshow("PixelCode", image);
	//resizeWindow("PixelCode", 177, 177);
	//waitKey(10000);

	fclose(fp); //关闭文件
	//fclose(DesFile);
	return 0;
}//Draw

Mat EnCode::ScaleToDisSize(const Mat& src) {//把src放大十倍
	Mat dis;
	int FrameOutputSize = EnCode::FrameSize * EnCode::FrameOutputRate;
	//定义输出大小， 等于 帧 大小×输出率
	dis = Mat(FrameOutputSize, FrameOutputSize, CV_8UC3);
	for (int i = 0; i < FrameOutputSize; ++i) {
		for (int j = 0; j < FrameOutputSize; ++j) {
			dis.at<Vec3b>(i, j) = src.at<Vec3b>(i / 10, j / 10);
		}//in-for
	}//out-for
	return dis;
}//ScaleToDisSize