#ifndef SITEMON_HPP
#define SITEMON_HPP

#include<Windows.h>//LPCWSTR头文件
#include<string>//string头文件

class Sitemon
{
public:
	Sitemon(std::string hostname);//构造函数,传入需要进行操作的主机域名
	int monitor(bool isSendEmail);//监控网站的主函数
	int GetHtml(bool isToFile);//获取HTML网页源代码

private:
	//data members
	std::string m_sHostname;//储存监控网站的域名
	struct Base64Date6//用于邮件加密处理
	{
		unsigned int d4 : 6;
		unsigned int d3 : 6;
		unsigned int d2 : 6;
		unsigned int d1 : 6;
	};

	//member functions
	bool IsEmailValid(std::string email_address);//判断输入邮箱地址是否有效
	bool IsFileValid(std::string file_name);//判断输入文件名称是否有效
	LPCWSTR stringToLPCWSTR(std::string str);//将常见的string类型转化成系统类型LPCWSTR
	char ConvertToBase64(char c6);//邮件加密，基于base64的加密方式
	void EncodeBase64(char *dbuf, char *buf128, int len);//对邮件进行基于basr64的方式加密
	int  OpenSocket(struct sockaddr *addr);//打开套接字连接，用于创建网络连接
	void SendMail(const char* email, const char* body);//发送邮件主函数
};

#endif  //SITEMON_HPP