#pragma once

#include<opencv2/opencv.hpp>
#include<fstream>
#include<bitset>
#define TABLE_LEN pow(2,8)//表长

using namespace std;
using namespace cv;

class encoder
{
public:
	const int IMG_WIDTH = 800;
	const int IMG_HEIGHT = 800;//二维码大小为 800×800
	const int BLOCK_WIDTH = 20;//块宽度20,用于数据区
	const int LOCATION_POINT_WIDTH = 200;//用于定位区
	int data_blocks_per_frame;

	unsigned char* CRC_TABLE;//crc编码数组

	int bin_size;//输入文件大小
	
	char* input_filename;

	char* output_filename;
	
	bool* bin_array;//Boolean型二进制文件数组
	
	const int FPS = 15;
	
	int count_blocks = 0;
	/// <summary>
	/// 四个定位点将二维码分成3个区域, 上面两个识别点所在的那几行记为block1, 中间没有识别点的大块记为block2, 最下面两个识别点所在的几行即为block3
	/// </summary>
	int blocks_1_x, blocks_1_y;
	int blocks_2_x, blocks_2_y;
	int blocks_3_x, blocks_3_y;

	int Trans_bin = 0;

	Scalar WHITE = Scalar(255, 255, 255);
	Scalar BLACK = Scalar(0);//三通道赋值颜色
	

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

