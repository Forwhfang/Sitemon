#include"Sitemon.hpp"

#include<wininet.h>//һ��Windows�µ�����⣬��socket���з�װ
#include<regex>//������ʽ�������ж�����������ַ�Ƿ���Ч 
#include<iostream>//���������ʾ��Ϣ
#include<fstream>//�����ļ�����

//���ƿռ�
using std::regex;
using std::cout;
using std::cin;
using std::endl;
using std::ofstream;

#pragma comment(lib,"ws2_32.lib")//����winsock���ļ�
#pragma comment(lib, "wininet.lib")//����wininet���ļ�
#pragma warning(disable:4996)//����C�����Ĵ���

//���캯������Ŀ�������������ַ��ʼ��
Sitemon::Sitemon(std::string hostname)
{
	m_sHostname = hostname; //Ŀ��������ʼ��
}
 
//�����վ����������Ҫ������wininet��
int Sitemon::monitor(bool isSendEmail)
{
	//����Ҫ�����ʼ����������ַ
	std::string emailTo;
	if (isSendEmail)
	{	
		cout << "Please input the Email address: " << endl;
		cin >> emailTo;
		while (!IsEmailValid(emailTo))
		{
			cout << "Email address is not valid. Please enter again." << endl;
			cin >> emailTo;
		}
		cout << "Email address is valid." << endl;
	}

	//1.������
	HINTERNET hInternet = InternetOpen(TEXT("Microsoft Edge"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternet == NULL)
	{
		cout << "Open error." << endl;
		return -1;
	}

	//2.���ӵ��ض�Ŀ���������ض��˿�
	HINTERNET hHttpSession = InternetConnect(hInternet, stringToLPCWSTR(m_sHostname), 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (hHttpSession == NULL)
	{
		InternetCloseHandle(hInternet);
		cout << "Connect error" << endl;
		return -2;
	}

	//3.׼������http����
	HINTERNET hHttpRequest = HttpOpenRequest(hHttpSession, TEXT("GET"), TEXT("/"), NULL, _T(""), NULL, 0, 0);
	if (hHttpRequest == NULL)
	{
		InternetCloseHandle(hHttpSession);
		InternetCloseHandle(hInternet);
		cout << "Send error." << endl;
		return -3;
	}

	//ѭ���ж���վ�Ƿ�����
	while (true)
	{
		DWORD dwRetCode = 0;//���淵�ص�״̬��
		DWORD dwSizeOfRq = sizeof(DWORD);
		clock_t start, finish;//�����������ʱ��
		start = clock();
		HttpSendRequest(hHttpRequest, NULL, 0, NULL, 0);//4.����http����
		HttpQueryInfo(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL);//5.��ȡĿ���������ص���Ϣ
		finish = clock();
		cout << "Delay Time: " << (double)(finish - start) / CLOCKS_PER_SEC << endl;//��������ӳ�ʱ��
		cout << "HTTP Status Code: " << dwRetCode << endl;//���״̬��
		//����״̬���ж��Ƿ���վ�Ƿ���������
		if (dwRetCode == 0 || dwRetCode >= 400)//��վ����
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

			default:cout << "Unknow error." << endl; break;
			}

			if (isSendEmail)//�ж��Ƿ���Ҫ�����ʼ�
			{
				std::string EmailContents = "From: \"Sitemon\"<895846885@qq.com>\r\nTo: \"Client\"<" + emailTo + ">\r\nSubject: Hello\r\n\r\nYour website is down.\n";
				SendMail(emailTo.c_str(), EmailContents.c_str());
			}
			
			break;//����վ�������˳�ѭ��
		}
		else//��վ����
		{
			cout << "The website is on." << endl;
			Sleep(10000);
		}
	}

	//�ر����ӣ��봴�����˳���෴��
	InternetCloseHandle(hHttpRequest);
	InternetCloseHandle(hHttpSession);
	InternetCloseHandle(hInternet);
	return 0;
}

//��ȡHTML��ҳ��Ϣ������������socket���
int Sitemon::GetHtml(bool isToFile)
{
	//����Ҫ���뵽�ļ��������ļ���
	std::string fileName;
	if (isToFile)
	{
		cout << "Please input the file name: " << endl;
		cin >> fileName;
		while (!IsFileValid(fileName))
		{
			cout << "File name is not valid. Please enter again." << endl;
			cin >> fileName;
		}
		cout << "File name is valid." << endl;
	}

	//����������
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//�����׽���
	SOCKET s;
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cout << "Socket error" << endl;
		return -1;
	}

	//���sockaddr_in�ṹ�������������վ��ز���
	struct sockaddr_in serv;
	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(80);
	hostent* hptr = gethostbyname(m_sHostname.c_str());//��������������
	memcpy(&serv.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length);

	//������վ
	if ((connect(s, (struct sockaddr *)&serv, sizeof(serv)))<0)
	{
		cout << "Connect error" << endl;
		return -2;
	}

	int num = 0;
	char sndBuf[1024] = { 0 };
	char rcvBuf[2048] = { 0 };

	//����html����
	memset(sndBuf, 0, 1024);
	strcat(sndBuf, "GET / HTTP/1.1\r\n\r\n");
	num = send(s, sndBuf, 1024, 0);
	if (num < 0)
	{
		cout << "Send error" << endl;
		return -3;
	}

	//������վ������Ϣ
	std::string toFile;
	while (true)
	{
		memset(rcvBuf, 0, 2048);
		num = recv(s, rcvBuf, 2048, 0);

		//ͳһ���뷽ʽ��Ϊ����ȷ��������ַ�
		int  unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, rcvBuf, -1, NULL, 0);
		wchar_t *pUnicode = new wchar_t[unicodeLen];
		memset(pUnicode, 0, unicodeLen * sizeof(wchar_t));
		::MultiByteToWideChar(CP_UTF8, 0, rcvBuf, -1, (LPWSTR)pUnicode, unicodeLen);
		int pSize = WideCharToMultiByte(CP_OEMCP, 0, pUnicode, wcslen(pUnicode), NULL, 0, NULL, NULL);
		char *pWchar_tToChar = new char[pSize + 1];
		WideCharToMultiByte(CP_OEMCP, 0, pUnicode, wcslen(pUnicode), pWchar_tToChar, pSize, NULL, NULL);
		pWchar_tToChar[pSize] = '\0';
		cout << pWchar_tToChar << endl;
		//printf("%s", rcvBuf);

		if(isToFile)
			toFile += pWchar_tToChar;

		if (strlen(rcvBuf) < 2048)
			break;
	}

	//������ļ�
	if (isToFile)
	{
		ofstream fout(fileName + ".txt", std::ios::out);
		fout << toFile;
	}

	//������
	closesocket(s);
	WSACleanup();
	return 0;
}

//����������ʽ�ж������ַ�Ƿ���Ч������C++11�±�׼�е�regex�ࣩ
bool Sitemon::IsEmailValid(std::string email_address)
{
	regex pattern("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");
	if (regex_match(email_address, pattern))
		return true;
	else
		return false;
}

//�ж��ļ������Ƿ���Ч
bool Sitemon::IsFileValid(std::string file_name)
{
	if (file_name.length() == 0 || file_name.length() > 255)
		return false;
	for (int i = 0; i < file_name.length(); i++)
	{
		if (file_name[i] == '?' || file_name[i] == '/' || file_name[i] == '\\' || file_name[i] == '*' || file_name[i] == '|' || file_name[i] == '\"' || file_name[i] == '<' || file_name[i] == '>')
			return false;
	}
	return true;
}

//��string����ת����LPCWSTR����
LPCWSTR Sitemon::stringToLPCWSTR(std::string str)
{
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(str.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, str.length() + 1, str.c_str(), _TRUNCATE);
	return wcstring;
}

//���ʼ����ݽ��л���base64��ʽ�ļ���
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

//��socket���ӣ���sendMail��������
int Sitemon::OpenSocket(struct sockaddr *addr)
{
	int sockfd = 0;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);//����socket��ͨ������ֵ�ж��Ƿ�ɹ�
	if (sockfd < 0)
	{
		cout << "Open error" << endl;
		exit(-1);
	}
	if (connect(sockfd, addr, sizeof(struct sockaddr)) < 0)//����connect��ͨ������ֵ�ж��Ƿ�ɹ�
	{
		cout << "Connect error" << endl;
		exit(-1);
	}
	return sockfd;
}

//�����ʼ�������ʵ�֣�����socket���
void Sitemon::SendMail(const char *emailTo, const char *body)
{
	int sockfd = 0;
	char buf[1500] = { 0 };
	char rbuf[1500] = { 0 };
	char login[128] = { 0 };
	char pass[128] = { 0 };

	//��������
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	//��дsockaddr_in�ṹ���������ӵ���Ϣ
	struct sockaddr_in their_addr = { 0 };
	memset(&their_addr, 0, sizeof(their_addr));
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(25);// һ����25�˿�
	hostent* hptr = gethostbyname("smtp.qq.com");// �˿ںͷ�����
	memcpy(&their_addr.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length);
	printf("IP of smpt.qq.com is : %d:%d:%d:%d\n",
		their_addr.sin_addr.S_un.S_un_b.s_b1,
		their_addr.sin_addr.S_un.S_un_b.s_b2,
		their_addr.sin_addr.S_un.S_un_b.s_b3,
		their_addr.sin_addr.S_un.S_un_b.s_b4);

	//�����ʼ���������������Ӻ�û����Ӧ����2�����������
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

	//�����Ǹ����ض��ĸ�ʽ���ʼ���������������
	//��Ҫ���õļ����������£�
	//memset��Ҫ���淢����Ϣ��char�����Ҫ������Ϣ��char�����ʼ��
	//sprintf_s��Ҫ���͵���Ϣ���浽char����
	//send������Ϣ��������
	//recv���շ��������ص���Ϣ

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
	sprintf_s(buf, 1500, "895846885@qq.com");
	memset(login, 0, 128);
	EncodeBase64(login, buf, strlen(buf));
	sprintf_s(buf, 1500, "%s\r\n", login);
	send(sockfd, buf, strlen(buf), 0);
	cout << "Base64 UserName: " << buf << endl;
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "User Login Receive: " << rbuf << endl;

	// PASSWORD
	sprintf_s(buf, 1500, "peyxjqzmjcirbdhg");
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

	// RCPT TO
	sprintf_s(buf, 1500, "RCPT TO:<%s>\r\n", emailTo);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Tell Sendto Receive: " << rbuf << endl;

	// DATA ׼����ʼ�����ʼ�����
	sprintf_s(buf, 1500, "DATA\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Prepare Receive: " << rbuf << endl;

	// �����ʼ����ݣ�\r\n.\r\n���ݽ������
	sprintf_s(buf, 1500, "%s\r\n.\r\n", body);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Receive: " << rbuf << endl;

	// �˳�����
	sprintf_s(buf, 1500, "QUIT\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Quit Receive: " << rbuf << endl;

	//������
	closesocket(sockfd);
	WSACleanup();
	return;
}