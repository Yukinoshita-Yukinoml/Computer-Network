# TCP和UDP

## socketAPI编程

>### 数据报socket(UDP)--SOCKET_DGRAM
	 
	 1. Server端首先接受一个端口进入循环,等待数据报到达后将报文复制到Buffer里 recvfrom() 类似文件操作中的 read
    
    2. Client端发送一个socket(特定的主机号+端口号) sendto()类似文件操作的write
    
3. UDP无连接,不需要listen()和accept(),数据报socket
    
       ## 流socket(TCP)--SOCKET_STREAM
    
       1. TCP面向连接,基于流的socket,用于连接不相关的进程



## SocketAPI在Client-Server模式执行流程

* TCP<img src="D:\计算机资料\计算机网络\Lab\Lab5\note\ClientServer_TCP.png"  />

* UDP<img src="D:\计算机资料\计算机网络\Lab\Lab5\note\ClientServer_UDP.png" style="zoom:75%;" />



网络编程:

* 服务器端接受连接请求的套接字创建过程: 
  1.调用socket()创建套接字
  2.调用bind()分配IP地址和端口号
  3.调用listen()转为可接受请求状态
  4.调用accept()受理连接请求	
* 客户端:
  1.调用socket()创建套接字
  2.调用connet()向Server端发送连接请求