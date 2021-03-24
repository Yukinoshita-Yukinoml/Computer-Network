#define _CRT_SECURE_NO_WARNINGS
#include "Encoder.h"

encoder::encoder(){
	//ÿһ��ͼƬ�����ڴ����Ϣ��blocks 
	data_blocks_per_frame = IMG_HEIGHT * IMG_WIDTH / BLOCK_WIDTH / BLOCK_WIDTH - LOCATION_POINT_WIDTH ^ 2 / BLOCK_WIDTH / BLOCK_WIDTH * 4;//ÿһ֡���������ڴ�����ݵĿ�Ĵ�С(��������ʶ����)
	CRC_TABLE = new unsigned char[TABLE_LEN];// �����CRC_TABLE[] 2^8 ���洢��Ԫ

	blocks_1_x = (IMG_WIDTH - LOCATION_POINT_WIDTH * 2) / BLOCK_WIDTH;//���ֳ�������1��λ��(ͼƬ��ȼ�ȥ������λ��Ŀ��)�����һ�����ݶ�Ӧ��ɫ��Ĵ�С��ȷ��forѭ��ʱ�Ĳ���, ��ͬ
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


//��ȡbin�ļ���С
int encoder::get_binfile_size() {
	FILE* bin_file = fopen(input_filename, "rb");
	fseek(bin_file, 0, SEEK_END);
	unsigned int len = ftell(bin_file);
	fclose(bin_file);
	bin_size = len;
	return len;
}



//���������ļ�������洢������
void encoder::get_bin_array() {
	FILE* bin_src = fopen(input_filename, "rb");
	unsigned char* buffer = new unsigned char[bin_size];
	bitset<16> temp;//16λ���ȵĶ���������

	int read_len=fread(buffer, 1, bin_size, bin_src);

	if (read_len!=bin_size)
		cout << "δ������ȡ�ļ�" << endl;
	
	long bit_len = bin_size * 16;//ȷ������洢�ռ�ĳ���

	bin_array = new bool[bit_len];//��bin_array���������Ĵ洢�ռ�

	temp = return_with_CRC(buffer[0]);//temp���ڽ��մ��ļ���ȡ���ݺ���Ƶ�CRC��

	for (size_t i = 0,j=1; i < bit_len&&Trans_bin<=bin_size; i++,j++){
		//�����������д���bin_array,�����������У���룬���������,������Ҫʹ��CRC����룬����CRC8-MAXIM��׼�����ɶ���ʽΪX^8+X^5+X^4+1����0x31
		bin_array[i] = temp[j-1];
		if (j%16==0){
			temp = return_with_CRC(buffer[++Trans_bin]);
			j = 1;//ÿ16λ����һ��temp, ��Ϊÿ16λΪһ��
		}
	}
}




//���ƶ�λ��
Mat encoder::Draw_Location_points() {
	Mat image = Mat(IMG_HEIGHT, IMG_WIDTH, CV_8UC3, WHITE);
	//���ϽǶ�λ��
	rectangle(image, Point(0, 0), Point(LOCATION_POINT_WIDTH, LOCATION_POINT_WIDTH), WHITE, FILLED);
	rectangle(image, Point(0, 0), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH, LOCATION_POINT_WIDTH - BLOCK_WIDTH), BLACK, FILLED);
	rectangle(image, Point(BLOCK_WIDTH, BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH * 2, LOCATION_POINT_WIDTH - BLOCK_WIDTH * 2), WHITE, FILLED);
	rectangle(image, Point(BLOCK_WIDTH * 2, BLOCK_WIDTH * 2), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH * 3, LOCATION_POINT_WIDTH - BLOCK_WIDTH * 3), BLACK, FILLED);


	//���ϽǶ�λ��
	rectangle(image, Point(IMG_WIDTH, 0), Point(IMG_WIDTH - LOCATION_POINT_WIDTH, LOCATION_POINT_WIDTH), WHITE, FILLED);
	rectangle(image, Point(IMG_WIDTH, 0), Point(IMG_WIDTH - LOCATION_POINT_WIDTH + BLOCK_WIDTH, LOCATION_POINT_WIDTH - BLOCK_WIDTH), BLACK, FILLED);
	rectangle(image, Point(IMG_WIDTH - BLOCK_WIDTH, BLOCK_WIDTH), Point(IMG_WIDTH - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 2, LOCATION_POINT_WIDTH - BLOCK_WIDTH * 2), WHITE, FILLED);
	rectangle(image, Point(IMG_WIDTH - BLOCK_WIDTH * 2, BLOCK_WIDTH * 2), Point(IMG_WIDTH - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 3, LOCATION_POINT_WIDTH - BLOCK_WIDTH * 3), BLACK, FILLED);


	//���½Ƕ�λ��
	rectangle(image, Point(0, IMG_HEIGHT), Point(LOCATION_POINT_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH), WHITE, FILLED);
	rectangle(image, Point(0, IMG_HEIGHT), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH + BLOCK_WIDTH), BLACK, FILLED);
	rectangle(image, Point(BLOCK_WIDTH, IMG_HEIGHT - BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH * 2, IMG_HEIGHT - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 2), WHITE, FILLED);
	rectangle(image, Point(BLOCK_WIDTH * 2, IMG_HEIGHT - BLOCK_WIDTH * 2), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH * 3, IMG_HEIGHT - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 3), BLACK, FILLED);


	////���½Ƕ�λ��
	//rectangle(image, Point(IMG_WIDTH, IMG_HEIGHT), Point(IMG_WIDTH - LOCATION_POINT_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH), WHITE, FILLED);
	//rectangle(image, Point(IMG_WIDTH, IMG_HEIGHT), Point(IMG_WIDTH - LOCATION_POINT_WIDTH + BLOCK_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH + BLOCK_WIDTH), BLACK, FILLED);
	//rectangle(image, Point(IMG_WIDTH - BLOCK_WIDTH, IMG_HEIGHT - BLOCK_WIDTH), Point(IMG_WIDTH - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 2, IMG_HEIGHT - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 2), WHITE, FILLED);
	//rectangle(image, Point(IMG_WIDTH - BLOCK_WIDTH * 2, IMG_HEIGHT - BLOCK_WIDTH * 2), Point(IMG_WIDTH - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 3, IMG_HEIGHT - LOCATION_POINT_WIDTH + BLOCK_WIDTH * 3), BLACK, FILLED);

	return image;
}


//���ÿһ֡ͼ��
void encoder::creat_bin_img(Mat& image) {
	//�����������ݵ���ά����

	//������1
	for (size_t y = 0; y < blocks_1_y; y++){
		for (size_t x = 0; x < blocks_1_x ; x++){
			if (bin_array[++count_blocks])//�ж϶���������ļ���0����1, �����1���ڶ�Ӧ�ĵط�����ɫ������, ���򻭰�ɫ������
				rectangle(image, Point(LOCATION_POINT_WIDTH + x * BLOCK_WIDTH, y * BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH + (x+1) * BLOCK_WIDTH , (y+1) * BLOCK_WIDTH), BLACK, FILLED);
			else
				rectangle(image, Point(LOCATION_POINT_WIDTH + x * BLOCK_WIDTH, y * BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH + (x + 1) * BLOCK_WIDTH, (y + 1) * BLOCK_WIDTH), WHITE, FILLED);
		}//��
	}//��
//����ͬ��

	//������2
	for (size_t y = 0; y < blocks_2_y; y++){
		for (size_t x = 0; x < blocks_2_x; x++){
			if (bin_array[++count_blocks])
				rectangle(image, Point(x * BLOCK_WIDTH, LOCATION_POINT_WIDTH + y * BLOCK_WIDTH), Point((x + 1) * BLOCK_WIDTH, LOCATION_POINT_WIDTH + (y + 1) * BLOCK_WIDTH), BLACK, FILLED);
			else
				rectangle(image, Point(x * BLOCK_WIDTH, LOCATION_POINT_WIDTH + y * BLOCK_WIDTH), Point((x + 1) * BLOCK_WIDTH, LOCATION_POINT_WIDTH + (y + 1) * BLOCK_WIDTH), WHITE, FILLED);
		}
	}


	//������3
	for (size_t y = 0; y < blocks_3_y; y++){
		for (size_t x = 0; x < blocks_3_x ; x++){
			if (bin_array[++count_blocks])
				rectangle(image, Point(LOCATION_POINT_WIDTH + x * BLOCK_WIDTH, IMG_HEIGHT-LOCATION_POINT_WIDTH + y * BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH + (x + 1) * BLOCK_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH + (y + 1) * BLOCK_WIDTH), BLACK, FILLED);
			else
				rectangle(image, Point(LOCATION_POINT_WIDTH + x * BLOCK_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH + y * BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH + (x + 1) * BLOCK_WIDTH, IMG_HEIGHT - LOCATION_POINT_WIDTH + (y + 1) * BLOCK_WIDTH), WHITE, FILLED);
		}
	}
}


void encoder::encode(char* inputfilename, char* output_filename, char* set_video_length) {//���� ���ƶ�ά��
	input_filename = inputfilename;//��ȡ����·��
	double video_length = get_video_len(set_video_length) / 1000;
	get_binfile_size();//�õ��������ļ���С
	get_bin_array();//�Ѷ������ļ��浽����
	VideoWriter writer(output_filename, VideoWriter::fourcc('M', 'J', 'P', 'G'), FPS, Size(900, 900),false);//��ͼƬ���浽ָ�������·��
	char img_path[100];//ͼƬ����·��
	for (size_t i = 0; i < video_length*FPS; i++){//һ����Ҫ���� ��Ƶ���� �� ֡�� ����ά��
		Mat frame = Draw_Location_points();//���ƶ�λ��
		creat_bin_img(frame);//���ÿһ֡ͼ��
	//����ʱע�͵��Ĵ���:
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
	//��һ���ַ�����crc����
	unsigned char crc = src;
	for (int i = 0; i < 8; i++) {
		if (crc & 0x80)//crc �� 10000000 ��λ��, ���ж�crc��0λ(����������)�ǲ���0
			crc = (crc << 1) ^ 0x31;//����0--crc����1λ��, ������ 00110001 ��λ���, ������ 2/3/7 λȡ��
		else  
			crc = crc << 1;//��0--crc����һλ
	}

	return crc;
}

void encoder::get_CRC_Tables() {
	unsigned char tmp = 0x00;//��ʼ��tmp Ϊ 16����0
	unsigned char res;
	for (int i = 1; i <= pow(2, 8); i++) {//ѭ�� 2^(8) = 256 �� �����е��ַ����벢����
		res = cal_CRC_CODE(tmp);//res���������
		CRC_TABLE[i - 1] = res;//����CRC_TABLE[]��
		tmp += 1;
	}
}

bitset<16> encoder::return_with_CRC(unsigned char src) {//bitset���ڴ洢�̶����ȵĲ���ֵ���У�����ֵΪ16λ������
	unsigned char crc = CRC_TABLE[src];//crc����src��CRC_TABLE�ж�Ӧ���ַ�
	string res;
	res += src;
	res += crc;
	bitset<16> code(src);// ��src ��ʼ�� 16λ���������� code
	bitset<8>  crc_code(crc);//��crc��ʼ�� 8λ���������� crc_code
	for (int i = 0; i < 8; i++)
		code[8 + i] = crc_code[i];//��crc_code��ֵ��code��8��15λ
	return code;
}

int encoder::get_video_len(char* v_len) {//��ȡ��Ƶ����
	return stod(v_len);
}