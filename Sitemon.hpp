#ifndef SITEMON_HPP
#define SITEMON_HPP

#include<Windows.h>//LPCWSTRͷ�ļ�
#include<string>//stringͷ�ļ�

class Sitemon
{
public:
	Sitemon(std::string hostname);//���캯��,������Ҫ���в�������������
	int monitor(bool isSendEmail);//�����վ��������
	int GetHtml(bool isToFile);//��ȡHTML��ҳԴ����

private:
	//data members
	std::string m_sHostname;//��������վ������
	struct Base64Date6//�����ʼ����ܴ���
	{
		unsigned int d4 : 6;
		unsigned int d3 : 6;
		unsigned int d2 : 6;
		unsigned int d1 : 6;
	};

	//member functions
	bool IsEmailValid(std::string email_address);//�ж����������ַ�Ƿ���Ч
	bool IsFileValid(std::string file_name);//�ж������ļ������Ƿ���Ч
	LPCWSTR stringToLPCWSTR(std::string str);//��������string����ת����ϵͳ����LPCWSTR
	char ConvertToBase64(char c6);//�ʼ����ܣ�����base64�ļ��ܷ�ʽ
	void EncodeBase64(char *dbuf, char *buf128, int len);//���ʼ����л���basr64�ķ�ʽ����
	int  OpenSocket(struct sockaddr *addr);//���׽������ӣ����ڴ�����������
	void SendMail(const char* email, const char* body);//�����ʼ�������
};

#endif  //SITEMON_HPP