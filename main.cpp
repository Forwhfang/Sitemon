#include"Sitemon.hpp"

//测试文件
int main()
{
	Sitemon obj("www.baidu.com");//传入一个参数（需要监控的网站域名）则只监控网站而不发送邮件，传入两个参数（需要监控的网站域名，要发送到的邮箱地址）则监控网站同时当网站异常时发送邮件
	obj.monitor();//调用监控网站的函数
	return 0;
}