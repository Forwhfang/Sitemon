#ifndef SITEMON_H
#define SITEMON_H

#include<Windows.h>
#include<string>

class Sitemon
{
public:
	Sitemon(std::string hostname, int port = 80, std::string urlpath = "/", std::string verb = "GET");
	int monitor();

private:
	//data members
	LPCWSTR m_lpszHostname;
	int m_iPort;
	LPCWSTR m_lpszUrlpath;
	LPCWSTR m_lpszVerb;
	struct Base64Date6
	{
		unsigned int d4 : 6;
		unsigned int d3 : 6;
		unsigned int d2 : 6;
		unsigned int d1 : 6;
	};

	//member functions
	LPCWSTR stringToLPCWSTR(std::string str);
	char ConvertToBase64(char c6);
	void EncodeBase64(char *dbuf, char *buf128, int len);
	int  OpenSocket(struct sockaddr *addr);
	void sendMail(char* email, const char* body);
};

#endif  //SITEMON_H