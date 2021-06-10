# Lab7: 代理服务器软件

## 实验思路

  本实验通过打开代理服务器实现局域网(通过虚拟机)访问外部网络, 要注意bind()端口以及连接正确的代理服务器(主机IP).</br>



## 本实验相关参考

* 实验环境: [CygWin](https://www.cygwin.com/)</br>

* SOCKS_PROXY示例程序: [SOCKS_PROXY](https://github.com/fgssfgss/socks_proxy/)</br>

编译命令:</br> 

>  gcc main.c -pthread -Wno-unused-result -g -std=gnu99 -Wall - o proxy.exe

## Socks 协议下载地址 

[Socks4](https://www.openssh.com/txt/socks4.protocol)</br>

[Socks4a](https://www.openssh.com/txt/socks4a.protocol) </br>

[Socks5 用户认证](https://tools.ietf.org/html/rfc1929)</br>

[Socks5](https://tools.ietf.org/html/rfc1928)</br>