#include"Sitemon.hpp"

//测试文件
int main()
{
	Sitemon obj("www.xwayne.com");
	//提供测试网站如下
	//www.baidu.com 
	//www.xwayne.com

	//------------------------------Test one------------------------------
	//检测网站是否异常以及测试网络延迟，传入参数false时，当网站异常则只在控制台输出信息
	//obj.monitor(false);

	//------------------------------Test two------------------------------
	//检测网站是否异常以及测试网络延迟，传入参数true时，当网站异常则既在控制台输出错误信息，同时会发邮件给指定用户
	//obj.monitor(true);

	//------------------------------Test three------------------------------
	//获取指定网页源代码，传入参数false时，输出到控制台中
	//obj.GetHtml(false); 

	//------------------------------Test three------------------------------
	//获取指定网页源代码，传入参数true时，输出到控制台中,同时输出到指定文件
	//obj.GetHtml(true);

	return 0;
}