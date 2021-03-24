#pragma once

#include<opencv2/opencv.hpp>
#include<fstream>
#include<bitset>
#define TABLE_LEN pow(2,8)//��

using namespace std;
using namespace cv;

class encoder
{
public:
	const int IMG_WIDTH = 800;
	const int IMG_HEIGHT = 800;//��ά���СΪ 800��800
	const int BLOCK_WIDTH = 20;//����20,����������
	const int LOCATION_POINT_WIDTH = 200;//���ڶ�λ��
	int data_blocks_per_frame;

	unsigned char* CRC_TABLE;//crc��������

	int bin_size;//�����ļ���С
	
	char* input_filename;

	char* output_filename;
	
	bool* bin_array;//Boolean�Ͷ������ļ�����
	
	const int FPS = 15;
	
	int count_blocks = 0;
	/// <summary>
	/// �ĸ���λ�㽫��ά��ֳ�3������, ��������ʶ������ڵ��Ǽ��м�Ϊblock1, �м�û��ʶ���Ĵ���Ϊblock2, ����������ʶ������ڵļ��м�Ϊblock3
	/// </summary>
	int blocks_1_x, blocks_1_y;
	int blocks_2_x, blocks_2_y;
	int blocks_3_x, blocks_3_y;

	int Trans_bin = 0;

	Scalar WHITE = Scalar(255, 255, 255);
	Scalar BLACK = Scalar(0);//��ͨ����ֵ��ɫ
	

public:
	encoder();
	~encoder();

	int get_binfile_size();

	void get_CRC_Tables();

	int get_video_len(char* v_len);

	unsigned char cal_CRC_CODE(unsigned char src);

	bitset<16> return_with_CRC(unsigned char src);

	void get_bin_array();

	void creat_bin_img(Mat & image);

	Mat Draw_Location_points();

	void encode(char* inputfilename, char* output_filename, char* set_video_length);
};

