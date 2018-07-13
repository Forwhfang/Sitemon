#include"Sitemon.hpp"

//测试文件
int main()
{
	Sitemon obj("www.xwayne.com");//传入一个参数（需要监控的网站域名）则只监控网站而不发送邮件，传入两个参数（需要监控的网站域名，要发送到的邮箱地址）则监控网站同时当网站异常时发送邮件
	//由于monitor函数是持续监控的，若网站不返回错误则监控会一直在循环体中执行
	//obj.GetHtml();//调用获取网页内容函数
	obj.monitor();//调用监控网站函数
	return 0;
}