#define _CRT_SECURE_NO_WARNINGS
#include "Encoder.h"

encoder::encoder(){
	//每一张图片可用于存放信息的blocks 
	data_blocks_per_frame = IMG_HEIGHT * IMG_WIDTH / BLOCK_WIDTH / BLOCK_WIDTH - LOCATION_POINT_WIDTH ^ 2 / BLOCK_WIDTH / BLOCK_WIDTH * 4;//每一帧中真正用于存放数据的块的大小(数据区非识别区)
	CRC_TABLE = new unsigned char[TABLE_LEN];// 分配给CRC_TABLE[] 2^8 个存储单元

	blocks_1_x = (IMG_WIDTH - LOCATION_POINT_WIDTH * 2) / BLOCK_WIDTH;//划分出数据区1的位置(图片宽度减去两个定位块的宽度)后除以一个数据对应的色块的大小以确定for循环时的步长, 下同
	blocks_2_x = (IMG_WIDTH) / BLOCK_WIDTH;
	blocks_3_x = (IMG_WIDTH - LOCATION_POINT_WIDTH) / BLOCK_WIDTH;
	blocks_1_y = (LOCATION_POINT_WIDTH) / BLOCK_WIDTH;
	blocks_2_y = (IMG_HEIGHT-LOCATION_POINT_WIDTH*2) / BLOCK_WIDTH;
	blocks_3_y = (LOCATION_POINT_WIDTH) / BLOCK_WIDTH;

	get_CRC_Tables();
}

encoder::~encoder()
{
}


//获取bin文件大小
int encoder::get_binfile_size() {
	FILE* bin_file = fopen(input_filename, "rb");
	fseek(bin_file, 0, SEEK_END);
	unsigned int len = ftell(bin_file);
	fclose(bin_file);
	bin_size = len;
	return len;
}



//将二进制文件串输入存储数组中
void encoder::get_bin_array() {
	FILE* bin_src = fopen(input_filename, "rb");
	unsigned char* buffer = new unsigned char[bin_size];
	bitset<16> temp;//16位长度的二进制序列

	int read_len=fread(buffer, 1, bin_size, bin_src);

	if (read_len!=bin_size)
		cout << "未完整读取文件" << endl;
	
	long bit_len = bin_size * 16;//确定所需存储空间的长度

	bin_array = new bool[bit_len];//给bin_array分配好所需的存储空间

	temp = return_with_CRC(buffer[0]);//temp用于接收从文件读取数据后编制的CRC码

	for (size_t i = 0,j=1; i < bit_len&&Trans_bin<=bin_size; i++,j++){
		//将二进制序列存入bin_array,可在这里进行校验码，纠正码编码,这里主要使用CRC检错码，采用CRC8-MAXIM标准，生成多项式为X^8+X^5+X^4+1，即0x31
		bin_array[i] = temp[j-1];
		if (j%16==0){
			temp = return_with_CRC(buffer[++Trans_bin]);
			j = 1;//每16位更新一次temp, 因为每16位为一组
		}
	}
}




//绘制定位点
Mat encoder::Draw_Location_points() {
	Mat image = Mat(IMG_HEIGHT, IMG_WIDTH, CV_8UC3, WHITE);
	//左上角定位点
	rectangle(image, Point(0, 0), Point(LOCATION_POINT_WIDTH, LOCATION_POINT_WIDTH), WHITE, FILLED);
	rectangle(image, Point(0, 0), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH, LOCATION_POINT_WIDTH - BLOCK_WIDTH), BLACK, FILLED);
	rectangle(image, Point(BLOCK_WIDTH, BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH * 2, LOCATION_POINT_WIDTH - BLOCK_WIDTH * 2), WHITE, FILLED);
	rectangle(image, Point(BLOCK_WIDTH * 2, BLOCK_WIDTH * 2), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH * 3, LOCATION_POINT_WIDTH - BLOCK_WIDTH * 3), BLACK, FILLED);


	//右上角定位点
	rectangle(image, Point(IMG_WIDTH, 0), Point(IMG_WIDTH - LOCATION_POINT_WIDTH, LOCATION_POINT_WIDTH), WHITE, FILLED);
	rectangle(image, Point(IMG_WIDTH, 0), Point(IMG_WIDTH - LOCATION_POINT_WIDTH + BLOCK_WIDTH, LOCATION_POINT_WIDTH - BLOCK_WIDTH), BLACK, FILLED);
	rectangle(image, Point(IMG_WIDTH - BLOCK_WIDTH, BLOCK_WIDTH), Point(IMG_WIDTH - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 2, LOCATION_POINT_WIDTH - BLOCK_WIDTH * 2), WHITE, FILLED);
	rectangle(image, Point(IMG_WIDTH - BLOCK_WIDTH * 2, BLOCK_WIDTH * 2), Point(IMG_WIDTH - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 3, LOCATION_POINT_WIDTH - BLOCK_WIDTH * 3), BLACK, FILLED);


	//左下角定位点
	rectangle(image, Point(0, IMG_HEIGHT), Point(LOCATION_POINT_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH), WHITE, FILLED);
	rectangle(image, Point(0, IMG_HEIGHT), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH + BLOCK_WIDTH), BLACK, FILLED);
	rectangle(image, Point(BLOCK_WIDTH, IMG_HEIGHT - BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH * 2, IMG_HEIGHT - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 2), WHITE, FILLED);
	rectangle(image, Point(BLOCK_WIDTH * 2, IMG_HEIGHT - BLOCK_WIDTH * 2), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH * 3, IMG_HEIGHT - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 3), BLACK, FILLED);


	////右下角定位点
	//rectangle(image, Point(IMG_WIDTH, IMG_HEIGHT), Point(IMG_WIDTH - LOCATION_POINT_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH), WHITE, FILLED);
	//rectangle(image, Point(IMG_WIDTH, IMG_HEIGHT), Point(IMG_WIDTH - LOCATION_POINT_WIDTH + BLOCK_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH + BLOCK_WIDTH), BLACK, FILLED);
	//rectangle(image, Point(IMG_WIDTH - BLOCK_WIDTH, IMG_HEIGHT - BLOCK_WIDTH), Point(IMG_WIDTH - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 2, IMG_HEIGHT - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 2), WHITE, FILLED);
	//rectangle(image, Point(IMG_WIDTH - BLOCK_WIDTH * 2, IMG_HEIGHT - BLOCK_WIDTH * 2), Point(IMG_WIDTH - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 3, IMG_HEIGHT - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 3), BLACK, FILLED);

	return image;
}


//填充每一帧图案
void encoder::creat_bin_img(Mat& image) {
	//填充二进制数据到二维码中

	//数据区1
	for (size_t y = 0; y < blocks_1_y; y++){
		for (size_t x = 0; x < blocks_1_x ; x++){
			if (bin_array[++count_blocks])//判断读入二进制文件是0还是1, 如果是1则在对应的地方画黑色正方形, 否则画白色正方形
				rectangle(image, Point(LOCATION_POINT_WIDTH + x * BLOCK_WIDTH, y * BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH + (x+1) * BLOCK_WIDTH , (y+1) * BLOCK_WIDTH), BLACK, FILLED);
			else
				rectangle(image, Point(LOCATION_POINT_WIDTH + x * BLOCK_WIDTH, y * BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH + (x + 1) * BLOCK_WIDTH, (y + 1) * BLOCK_WIDTH), WHITE, FILLED);
		}//行
	}//列
//以下同理

	//数据区2
	for (size_t y = 0; y < blocks_2_y; y++){
		for (size_t x = 0; x < blocks_2_x; x++){
			if (bin_array[++count_blocks])
				rectangle(image, Point(x * BLOCK_WIDTH, LOCATION_POINT_WIDTH + y * BLOCK_WIDTH), Point((x + 1) * BLOCK_WIDTH, LOCATION_POINT_WIDTH + (y + 1) * BLOCK_WIDTH), BLACK, FILLED);
			else
				rectangle(image, Point(x * BLOCK_WIDTH, LOCATION_POINT_WIDTH + y * BLOCK_WIDTH), Point((x + 1) * BLOCK_WIDTH, LOCATION_POINT_WIDTH + (y + 1) * BLOCK_WIDTH), WHITE, FILLED);
		}
	}


	//数据区3
	for (size_t y = 0; y < blocks_3_y; y++){
		for (size_t x = 0; x < blocks_3_x ; x++){
			if (bin_array[++count_blocks])
				rectangle(image, Point(LOCATION_POINT_WIDTH + x * BLOCK_WIDTH, IMG_HEIGHT-LOCATION_POINT_WIDTH + y * BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH + (x + 1) * BLOCK_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH + (y + 1) * BLOCK_WIDTH), BLACK, FILLED);
			else
				rectangle(image, Point(LOCATION_POINT_WIDTH + x * BLOCK_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH + y * BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH + (x + 1) * BLOCK_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH + (y + 1) * BLOCK_WIDTH), WHITE, FILLED);
		}
	}
}


void encoder::encode(char* inputfilename, char* output_filename, char* set_video_length) {//编码 绘制二维码
	input_filename = inputfilename;//获取输入路径
	double video_length = get_video_len(set_video_length) / 1000;
	get_binfile_size();//得到二进制文件大小
	get_bin_array();//把二进制文件存到数组
	VideoWriter writer(output_filename, VideoWriter::fourcc('M', 'J', 'P', 'G'), FPS, Size(900, 900),false);//将图片保存到指定的输出路径
	char img_path[100];//图片所在路径
	for (size_t i = 0; i < video_length*FPS; i++){//一共需要绘制 视频长度 × 帧率 个二维码
		Mat frame = Draw_Location_points();//绘制定位点
		creat_bin_img(frame);//填充每一帧图案
	//运行时注释掉的代码:
		imshow("i",frame);
		waitKey(0);
		//cout<<i<<endl;
		//sprintf(img_path, "./img/%d.png", i+1);
		imwrite(img_path, frame);
	//
		writer.write(frame);
	}
	writer.release();
}

unsigned char encoder::cal_CRC_CODE(unsigned char src) {
	//对一个字符进行crc编码
	unsigned char crc = src;
	for (int i = 0; i < 8; i++) {
		if (crc & 0x80)//crc 与 10000000 按位与, 即判断crc第0位(从左到右算起)是不是0
			crc = (crc << 1) ^ 0x31;//不是0--crc左移1位后, 整体与 00110001 按位异或, 即将第 2/3/7 位取反
		else  
			crc = crc << 1;//是0--crc左移一位
	}

	return crc;
}

void encoder::get_CRC_Tables() {
	unsigned char tmp = 0x00;//初始化tmp 为 16进制0
	unsigned char res;
	for (int i = 1; i <= pow(2, 8); i++) {//循环 2^(8) = 256 次 将所有的字符编码并保存
		res = cal_CRC_CODE(tmp);//res保存编码结果
		CRC_TABLE[i - 1] = res;//赋到CRC_TABLE[]中
		tmp += 1;
	}
}

bitset<16> encoder::return_with_CRC(unsigned char src) {//bitset用于存储固定长度的布尔值序列，返回值为16位二进制
	unsigned char crc = CRC_TABLE[src];//crc保存src在CRC_TABLE中对应的字符
	string res;
	res += src;
	res += crc;
	bitset<16> code(src);// 用src 初始化 16位二进制序列 code
	bitset<8>  crc_code(crc);//用crc初始化 8位二进制序列 crc_code
	for (int i = 0; i < 8; i++)
		code[8 + i] = crc_code[i];//用crc_code赋值给code的8到15位
	return code;
}

int encoder::get_video_len(char* v_len) {//获取视频长度
	return stod(v_len);
}