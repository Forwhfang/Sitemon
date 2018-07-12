#include"Sitemon.hpp"

#include<wininet.h>//一个Windows下的网络库，将socket进行封装
#include<regex>//正则表达式，用于判断输入的邮箱地址是否有效 
#include<iostream>//用于输出提示信息

//名称空间
using std::regex;
using std::cout;
using std::endl;

#pragma comment(lib,"ws2_32.lib")//包含winsock库文件
#pragma comment(lib, "wininet.lib")//包含wininet库文件
#pragma warning(disable:4996)//忽略C产生的错误

//构造函数，对目标域名与邮箱地址初始化
Sitemon::Sitemon(std::string hostname)
{
	m_lpszHostname = stringToLPCWSTR(hostname); //目标域名初始化
	m_ptrEmailTo = NULL;//邮箱地址初始化
	m_bIsSend = false;//不发送邮件
}

Sitemon::Sitemon(std::string hostname, char *emailTo)
{
	m_lpszHostname = stringToLPCWSTR(hostname);//将参数列表中的hostname转化成LPCWSTR类型
	if (IsEmailValid(std::string(emailTo)))//判断邮箱地址是否合法
	{
		m_ptrEmailTo = emailTo;//邮箱地址初始化
		m_bIsSend = true;//发送邮件
	}
	else
	{
		cout << "The Email format is wrong." << endl;
		m_ptrEmailTo = NULL;//邮箱地址初始化
		m_bIsSend = false;//不发送邮件
	}
}
 
//监控网站主函数，主要运用了wininet库
int Sitemon::monitor()
{
	//1.打开网络
	HINTERNET hInternet = InternetOpen(TEXT("Microsoft Edge"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternet == NULL)
	{
		cout << "Can not open the Internet." << endl;
		return -1;
	}

	//2.连接到特定目标主机的特定端口
	HINTERNET hHttpSession = InternetConnect(hInternet, m_lpszHostname, 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (hHttpSession == NULL)
	{
		InternetCloseHandle(hInternet);
		cout << "Can not connect to the host in the particular port." << endl;
		return -2;
	}

	//3.准备发送http请求
	HINTERNET hHttpRequest = HttpOpenRequest(hHttpSession, TEXT("GET"), TEXT("/"), NULL, _T(""), NULL, 0, 0);
	if (hHttpRequest == NULL)
	{
		InternetCloseHandle(hHttpSession);
		InternetCloseHandle(hInternet);
		cout << "Can not send the request to the particular urlpath." << endl;
		return -3;
	}

	//循环判断网站是否正常
	while (true)
	{
		DWORD dwRetCode = 0;//储存返回的状态码
		DWORD dwSizeOfRq = sizeof(DWORD);
		HttpSendRequest(hHttpRequest, NULL, 0, NULL, 0);//4.发送http请求
		HttpQueryInfo(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL);//5.获取目标主机返回的信息
		cout << "HTTP Status Code: " << dwRetCode << endl;//输出状态码
		//根据状态码判断是否网站是否正常运行
		if (dwRetCode == 0 || dwRetCode >= 400)//网站错误
		{
			switch (dwRetCode)
			{
			case 0:cout << "Not exist" << endl; break;

			case 400:cout << "Bad Request" << endl; break;
			case 401:cout << "Unauthorized" << endl; break;
			case 402:cout << "Payment Required" << endl; break;
			case 403:cout << "Forbidden" << endl; break;
			case 404:cout << "Not Found" << endl; break;
			case 405:cout << "Method Not Allowed" << endl; break;
			case 406:cout << "Not Acceptable" << endl; break;
			case 407:cout << "Proxy Authentication Required" << endl; break;
			case 408:cout << "Request Time-out" << endl; break;
			case 409:cout << "Conflict" << endl; break;
			case 410:cout << "Gone" << endl; break;
			case 411:cout << "Length Required" << endl; break;
			case 412:cout << "Precondition Failed" << endl; break;
			case 413:cout << "Request Entity Too Large" << endl; break;
			case 414:cout << "Request - URI Too Large" << endl; break;
			case 415:cout << "Unsupported Media Type" << endl; break;
			case 416:cout << "Requested range not satisfiable" << endl; break;
			case 417:cout << "Expectation Failed" << endl; break;

			case 500:cout << "Internal Server Error" << endl; break;
			case 501:cout << "Not Implemented" << endl; break; 
			case 502:cout << "Bad Gateway" << endl; break;
			case 503:cout << "Service Unavailable" << endl; break;
			case 504:cout << "Gateway Time-out" << endl; break;
			case 505:cout << "HTTP Version not supported" << endl; break;

			default:cout << "Unknow error." << endl;
			}

			if (m_bIsSend)//判断是否需要发送邮件
			{
				std::string EmailContents = "From: \"Sitemon\"<895846885@qq.com>\r\nTo: \"Client\"<" + std::string(m_ptrEmailTo) + ">\r\nSubject: Hello\r\n\r\nYour website is down.\n";
				sendMail(m_ptrEmailTo, EmailContents.c_str());
			}
			
			break;//若网站错误则退出循环
		}
		else//网站正常
		{
			cout << "The website is on." << endl;
			Sleep(10000);
		}
	}

	//关闭连接
	InternetCloseHandle(hHttpRequest);
	InternetCloseHandle(hHttpSession);
	InternetCloseHandle(hInternet);
	return 0;
}

//利用正则表达式判断邮箱地址是否有效
bool Sitemon::IsEmailValid(std::string email_address)
{
	regex pattern("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");
	if (regex_match(email_address, pattern))
		return true;
	else
		return false;
}

//将string类型转化成LPCWSTR类型
LPCWSTR Sitemon::stringToLPCWSTR(std::string str)
{
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(str.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, str.length() + 1, str.c_str(), _TRUNCATE);
	return wcstring;
}

//对邮件内容进行基于base64方式的加密
char Sitemon::ConvertToBase64(char ch)
{
	if (ch < 26)
		return 'A' + ch;
	else if (ch < 52)
		return 'a' + (ch - 26);
	else if (ch < 62)
		return '0' + (ch - 52);
	else if (ch == 62)
		return '+';
	else
		return '/';
}
void Sitemon::EncodeBase64(char* dbuf, char* buf128, int len)
{
	struct Base64Date6 *ddd = NULL;
	int i = 0;
	char buf[256] = { 0 };
	char* tmp = NULL;
	char cc = '\0';

	memset(buf, 0, 256);
	strcpy_s(buf, 256, buf128);

	for (i = 1; i <= len / 3; i++)
	{
		tmp = buf + (i - 1) * 3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct Base64Date6 *)tmp;
		dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
		dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
		dbuf[(i - 1) * 4 + 2] = ConvertToBase64((unsigned int)ddd->d3);
		dbuf[(i - 1) * 4 + 3] = ConvertToBase64((unsigned int)ddd->d4);
	}
	if (len % 3 == 1)
	{
		tmp = buf + (i - 1) * 3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct Base64Date6 *)tmp;
		dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
		dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
		dbuf[(i - 1) * 4 + 2] = '=';
		dbuf[(i - 1) * 4 + 3] = '=';
	}
	if (len % 3 == 2)
	{
		tmp = buf + (i - 1) * 3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct Base64Date6 *)tmp;
		dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
		dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
		dbuf[(i - 1) * 4 + 2] = ConvertToBase64((unsigned int)ddd->d3);
		dbuf[(i - 1) * 4 + 3] = '=';
	}
	return;
}

//打开socket连接，被sendMail函数调用
int Sitemon::OpenSocket(struct sockaddr *addr)
{
	int sockfd = 0;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);//创建socket，通过返回值判断是否成功
	if (sockfd < 0)
	{
		cout << "Open sockfd(TCP) error!" << endl;
		exit(-1);
	}
	if (connect(sockfd, addr, sizeof(struct sockaddr)) < 0)//创建connect，通过返回值判断是否成功
	{
		cout << "Connect sockfd(TCP) error!" << endl;
		exit(-1);
	}
	return sockfd;
}

//发送邮件主函数实现，基于socket编程
void Sitemon::sendMail(char *emailTo, const char *body)
{
	int sockfd = { 0 };
	char buf[1500] = { 0 };
	char rbuf[1500] = { 0 };
	char login[128] = { 0 };
	char pass[128] = { 0 };

	//开启连接
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	//填写sockaddr_in结构，储存连接的信息
	struct sockaddr_in their_addr = { 0 };
	memset(&their_addr, 0, sizeof(their_addr));
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(25);// 一般是25端口
	hostent* hptr = gethostbyname("smtp.qq.com");// 端口和服务器
	memcpy(&their_addr.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length);
	printf("IP of smpt.qq.com is : %d:%d:%d:%d\n",
		their_addr.sin_addr.S_un.S_un_b.s_b1,
		their_addr.sin_addr.S_un.S_un_b.s_b2,
		their_addr.sin_addr.S_un.S_un_b.s_b3,
		their_addr.sin_addr.S_un.S_un_b.s_b4);

	//连接邮件服务器，如果连接后没有响应，则2秒后重新连接
	sockfd = OpenSocket((struct sockaddr *)&their_addr);
	memset(rbuf, 0, 1500);
	while (recv(sockfd, rbuf, 1500, 0) == 0)
	{
		cout << "reconnect..." << endl;
		Sleep(2);
		sockfd = OpenSocket((struct sockaddr *)&their_addr);
		memset(rbuf, 0, 1500);
	}
	cout << rbuf << endl;

	//以下是根据特定的格式给邮件服务器发送请求
	//主要调用的几个函数如下：
	//memset把要储存发送信息的char数组和要接收信息的char数组初始化
	//sprintf_s把要发送的信息储存到char数组
	//send发送信息给服务器
	//recv接收服务器返回的信息

	// EHLO
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "EHLO HYL-PC\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "EHLO Receive: " << rbuf << endl;

	// AUTH LOGIN
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "AUTH LOGIN\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Auth Login Receive: " << rbuf << endl;

	// USER
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "895846885@qq.com");//你的邮箱账号
	memset(login, 0, 128);
	EncodeBase64(login, buf, strlen(buf));
	sprintf_s(buf, 1500, "%s\r\n", login);
	send(sockfd, buf, strlen(buf), 0);
	cout << "Base64 UserName: " << buf << endl;
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "User Login Receive: " << rbuf << endl;

	// PASSWORD
	sprintf_s(buf, 1500, "peyxjqzmjcirbdhg");//你的邮箱密码
	memset(pass, 0, 128);
	EncodeBase64(pass, buf, strlen(buf));
	sprintf_s(buf, 1500, "%s\r\n", pass);
	send(sockfd, buf, strlen(buf), 0);
	cout << "Base64 Password: " << buf << endl;

	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Password Receive: " << rbuf << endl;

	// MAIL FROM
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "MAIL FROM: <895846885@qq.com>\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "set Mail From Receive: " << rbuf << endl;

	// RCPT TO 第一个收件人
	sprintf_s(buf, 1500, "RCPT TO:<%s>\r\n", emailTo);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Tell Sendto Receive: " << rbuf << endl;

	// DATA 准备开始发送邮件内容
	sprintf_s(buf, 1500, "DATA\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Prepare Receive: " << rbuf << endl;

	// 发送邮件内容，\r\n.\r\n内容结束标记
	sprintf_s(buf, 1500, "%s\r\n.\r\n", body);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Receive: " << rbuf << endl;

	// QUIT
	sprintf_s(buf, 1500, "QUIT\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Quit Receive: " << rbuf << endl;

	//清理工作
	closesocket(sockfd);
	WSACleanup();
	return;
}