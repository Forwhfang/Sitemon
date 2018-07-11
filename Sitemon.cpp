#include"stdafx.h"
#include"Sitemon.h"

#include<wininet.h>
#include<iostream>
using std::cout;
using std::endl;

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "wininet.lib")
#pragma warning(disable:4996)

Sitemon::Sitemon(std::string hostname, int port, std::string verb, std::string urlpath)
{
	m_lpszHostname = stringToLPCWSTR(hostname);
	m_iPort = port;
	m_lpszVerb = stringToLPCWSTR(verb);
	m_lpszUrlpath = stringToLPCWSTR(urlpath);
}
 
int Sitemon::monitor()
{
	//Initialization
	HINTERNET hInternet = InternetOpen(TEXT("Microsoft Edge"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternet == NULL)
	{
		cout << "Can not open the Internet." << endl;
		return -1;
	}

	HINTERNET hHttpSession = InternetConnect(hInternet, m_lpszHostname, m_iPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (hHttpSession == NULL)
	{
		InternetCloseHandle(hInternet);
		cout << "Can not connect to the host in the particular port." << endl;
		return -2;
	}

	HINTERNET hHttpRequest = HttpOpenRequest(hHttpSession, m_lpszVerb, m_lpszUrlpath, NULL, _T(""), NULL, 0, 0);
	if (hHttpRequest == NULL)
	{
		InternetCloseHandle(hHttpSession);
		InternetCloseHandle(hInternet);
		cout << "Can not send the request to the particular urlpath." << endl;
		return -3;
	}

	//Check the HTTP Status Code in loop
	while (true)
	{
		DWORD dwRetCode = 0;
		DWORD dwSizeOfRq = sizeof(DWORD);
		HttpSendRequest(hHttpRequest, NULL, 0, NULL, 0);
		HttpQueryInfo(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL);
		cout << "State Code: " << dwRetCode << endl;
		//--------------------错误判定（需要修改）--------------------
		if (dwRetCode>=400)//bad state code
		{
			std::string EmailContents = "From: \"Sitemon\"<895846885@qq.com>\r\nTo: \"Fang\"<945338423@qq.com>\r\nSubject: Hello\r\n\r\nYour website is down.\n";
			char EmailTo[] = "945338423@qq.com";
			sendMail(EmailTo, EmailContents.c_str());
			cout << "The website is down." << endl;
			break;
		}
		else
		{
			cout << "The website is on." << endl;
			Sleep(10000);
		}
	}

	//Closed
	InternetCloseHandle(hHttpRequest);
	InternetCloseHandle(hHttpSession);
	InternetCloseHandle(hInternet);
	return 0;
}


LPCWSTR Sitemon::stringToLPCWSTR(std::string str)
{
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(str.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, str.length() + 1, str.c_str(), _TRUNCATE);
	return wcstring;
}


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

// 打开TCP Socket连接
int Sitemon::OpenSocket(struct sockaddr *addr)
{
	int sockfd = 0;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		cout << "Open sockfd(TCP) error!" << endl;
		exit(-1);
	}
	if (connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
	{
		cout << "Connect sockfd(TCP) error!" << endl;
		exit(-1);
	}
	return sockfd;
}

//--------------------有两个地方需要修改--------------------
void Sitemon::sendMail(char *email, const char *body)
{
	int sockfd = { 0 };
	char buf[1500] = { 0 };
	char rbuf[1500] = { 0 };
	char login[128] = { 0 };
	char pass[128] = { 0 };

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

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

	// 连接邮件服务器，如果连接后没有响应，则2秒后重新连接
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
	sprintf_s(buf, 1500, "MAIL FROM: <895846885@qq.com>\r\n");  //此处要和发邮件的邮箱保持一致
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "set Mail From Receive: " << rbuf << endl;

	// RCPT TO 第一个收件人
	sprintf_s(buf, 1500, "RCPT TO:<%s>\r\n", "945338423@qq.com");  //此处要和收邮件的邮箱保持一致
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