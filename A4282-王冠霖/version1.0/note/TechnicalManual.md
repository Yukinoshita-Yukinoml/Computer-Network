----------


## 环境配置

[配置OpenCV库到VS2019](https://www.jb51.net/article/184819.htm) </br>
 &emsp; *注意版本问题*
 
----------

# 编码部分</br>


## I.将二进制文件转化为图像</br>
&emsp; **思路概述** </br>
&emsp; &emsp; 输入: 用户随意给定一个二进制文件, 转化为图片文件.  </br>
&emsp; &emsp; 输出:  绘制二维码并保存到指定路径. </br> 
&emsp; &emsp;1.[文件转二维码] 读入文件(如果不是二进制文件先转化成二进制文件), 保存到数组当中, 等待绘制二维码时调用.</br>
&emsp; &emsp; 2.[对数据进行编码]  进行差错控制编码以及压缩编码. </br>
&emsp; &emsp; 3.[绘制识别区] 设置好, 分别在左上角、右上角、左下角重复绘制三次(因为平行来看三点即可确定一个平面), 本部分要提高识别准确度.</br>

[//]: <> "这种方法可能会造成不平行于屏幕拍摄时解析错误或者解析不出来的问题."

**代码示例:**
``` c++
//以绘制左上角代码为例
rectangle(image, Point(0, 0), Point(LOCATION_POINT_WIDTH, LOCATION_POINT_WIDTH), WHITE, FILLED);
rectangle(image, Point(0, 0), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH, LOCATION_POINT_WIDTH - BLOCK_WIDTH), BLACK, FILLED);
rectangle(image, Point(BLOCK_WIDTH, BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH * 2, LOCATION_POINT_WIDTH - BLOCK_WIDTH * 2), WHITE, FILLED);
rectangle(image, Point(BLOCK_WIDTH * 2, BLOCK_WIDTH * 2), Point(LOCATION_POINT_WIDTH - BLOCK_WIDTH * 3, LOCATION_POINT_WIDTH - BLOCK_WIDTH * 3), BLACK, FILLED);
```

&emsp; &emsp; 4.[绘制数据区]  将二进制文件中的0-1视作黑-白像素(通过枚举类型), 两层for循环代表行和列, 借助OpenCV库逐行输出数据对应的(~~像素~~)<font color = Red>正方形色块</font>  <font color = Gray> *(因为输出像素得到的图案实在太小)* </font> 得到二维码. </br>

**代码示例:**
``` c++
// 以绘制数据区1(最上面的数据区)为例
for (size_t y = 0; y < blocks_1_y; y++){
	for (size_t x = 0; x < blocks_1_x ; x++){
		if(bin_array[++count_blocks])//判断读入二进制文件是0还是1, 如果是1则在对应的地方画黑色正方形, 否则画白色正方形
			rectangle(image, Point(LOCATION_POINT_WIDTH + x * BLOCK_WIDTH, y * BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH + (x+1) * BLOCK_WIDTH , (y+1) * BLOCK_WIDTH), BLACK, FILLED);
		else
			rectangle(image, Point(LOCATION_POINT_WIDTH + x * BLOCK_WIDTH, y * BLOCK_WIDTH), Point(LOCATION_POINT_WIDTH + (x + 1) * BLOCK_WIDTH, (y + 1) * BLOCK_WIDTH), WHITE, FILLED);
	}//行
}//列
```

&emsp; &emsp; 5.[绘制多张二维码]  根据视频长度以及帧率绘制相应数量的二维码到指定路径, 需要注意的是**帧率的选择**, 我们应该重复编码, 即多次绘制同一张二维码以便转成视频时正确显示.</br>

### Q&A. </br>
&emsp; 1. 为什么选取CRC编码? </br>

&emsp; 2. 为什么选取这个帧率? </br>

&emsp; 3. 选用哪种压缩编码, 为什么用这个? </br>

&emsp; 4. 采取哪种掩膜版, 为什么?</br>


----------


## II.将图像转化为视频 </br>
- 这部分主要问题出在路径名规范的问题上(文件名要包含数字等), 而且opencv的版本可能会造成缺少 **.dll** 文件的问题. </br>

- 其余部分只需要按照文件操作打开, 再调用opencv库函数即可解决.</br>

代码示例:
``` c++
 	VideoWriter video(img_path, CAP_OPENCV_MJPEG, 1.0, Size(1080, 1080));
	//第一个参数表示读取的视频帧所存放的新的文件
	//第二个参数指的是视频存放的编码格式
	//fps表示每秒的帧数
	//size表示图像的长宽大小
	vector<String> img;
	glob(img_path, img, false);//遍历文件夹下的所有图片
	size_t count = img.size();
	for (size_t i = 0; i < count; i++){
		Mat image = imread(img[i]);
		resize(image, image, Size(1080, 1080));//resize函数改变图像大小
		video << image;
	}
```

### Q&A:</br>
&emsp; 如何提高信噪比?

----------

*To be continued...*
