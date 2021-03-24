#include<opencv2/opencv.hpp>
#include <stdio.h>
#include <io.h>
#include <string>
#include <iostream> 
#include <fstream> 
#include "ConvertImgToVideo.h"

using namespace std;
using namespace cv;

void ConvertItoV::ImgsToVideo(){
    _finddata_t FileInfo;
    //读取图片所在的路径
    string inPath = "C:/Users/lenovo/Pictures/Saved Pictures/";
    string strfind = inPath + "*";
    long Handle = _findfirst(strfind.c_str(), &FileInfo);
    char filename[300];
    Mat src;
    if (Handle == -1L){
        cerr << "can not match the folder path" << endl;
        exit(-1);
    }

    Mat frame;
    Mat src0 = imread("C:/Users/lenovo/Pictures/Saved Pictures/0000.jpg");
    Size size = src0.size();
    VideoWriter writer;
    writer.open("tset001.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, size, true);

    do {
    //判断是否有子目录
        if (FileInfo.attrib & _A_SUBDIR){
            if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0)){
                inPath += FileInfo.name;
                cout << inPath << endl;
            }
        }
        else{
            cout << inPath << FileInfo.name << endl;
            //读取图片所在的路径
            string path = "C:/Users/lenovo/Pictures/Saved Pictures/" + (string)FileInfo.name;
            src = imread(path);
            imshow("input", src);
            writer.write(src);
            waitKey(1);
        }
    }while (_findnext(Handle, &FileInfo) == 0);
    _findclose(Handle);
}
