#include "EnCode.h"

int EnCode::Draw(Mat& image, Point Start, Point End, const char* filePath) {
	/// <summary>
	/// ���ļ��Ĵ���
	FILE* fp = fopen(filePath, "rb"); //���������ļ����ļ�·��
	if (fp == nullptr) return 1; //��ʧ�ܷ���1
	fseek(fp, 0, SEEK_END); //���ļ���ĩβ(0ƫ��)����
	int size = ftell(fp); //���ص�ǰ���λ��,�˴���Ϊ�ļ�ĩβ,���ڵó������ļ��Ĵ�С
	rewind(fp); //�����ļ�λ��Ϊ��fpָ���ļ��Ŀ�ͷ
	int* temp = (int*)malloc(sizeof(int) * size); //������̬����temp,��СΪ�ļ��Ĵ�Сsize��char�ͳ���
	if (temp == nullptr) return 1; //tempָ���򷵻�1
	fread(temp, 1, size, fp); //��fpָ����ļ��е����ݱ��浽temp������

	///
	int k = 0;
	//image ��ͼ����� Start-End ������ʼλ��
	//Scalar(0)---��ɫ  Scalar(255)---��ɫ
	int Column = 177, Row = 177;//��ά��Version �����ֵ�� 40���ʳߴ����ֵ��(40-1)*4+21 = 177���� 177 x 177 �ľ��� �ٴ�Ч����ܲ� �ߴ�����˺����ͨ��ѧ��ScaleToDisSize()�ķ���ʵ��
	for (int y = 0; y < Row; y++) {
		for (int x = 0; x < Column; x++) {
			//��ȡ����
			Vec3b color = image.at<Vec3b>(Point(x, y));

			//������ɫ
			if (temp[k] == BLACK) {
				color[0] = 0;
				color[1] = 0;
				color[2] = 0;//��ɫ
				image.at<Vec3b>(Point(x, y)) = color;
			}
			else {
				color[0] = 255;
				color[1] = 255;
				color[2] = 255;
				image.at<Vec3b>(Point(x, y)) = color;
			}//��ɫ
			 k++;
			//��������
			//image.at<Vec3b>(Point(x, y)) = color;
		}//for column
	}//for row
	//FILE* DesFile = fopen("D:\\���������\\���������\\��������� ��Ŀ��\\Project01\\test\\test.jpg","a");

	imwrite("D:\\���������\\���������\\��������� ��Ŀ��\\Project01\\test\\test.jpg", image);//����ͼƬ
	//imshow("PixelCode", image);
	//resizeWindow("PixelCode", 177, 177);
	//waitKey(10000);

	fclose(fp); //�ر��ļ�
	//fclose(DesFile);
	return 0;
}//Draw

Mat EnCode::ScaleToDisSize(const Mat& src) {//��src�Ŵ�ʮ��
	Mat dis;
	int FrameOutputSize = EnCode::FrameSize * EnCode::FrameOutputRate;
	//���������С�� ���� ֡ ��С�������
	dis = Mat(FrameOutputSize, FrameOutputSize, CV_8UC3);
	for (int i = 0; i < FrameOutputSize; ++i) {
		for (int j = 0; j < FrameOutputSize; ++j) {
			dis.at<Vec3b>(i, j) = src.at<Vec3b>(i / 10, j / 10);
		}//in-for
	}//out-for
	return dis;
}//ScaleToDisSize