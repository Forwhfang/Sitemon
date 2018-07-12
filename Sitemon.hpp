#ifndef SITEMON_HPP
#define SITEMON_HPP

#include<Windows.h>
#include<string>

class Sitemon
{
public:
	Sitemon(std::string hostname);
	Sitemon(std::string hostname, char *emailto);
	int monitor();

private:
	//data members
	LPCWSTR m_lpszHostname;
	char *m_ptrEmailTo;
	bool m_bIsSend;
	struct Base64Date6
	{
		unsigned int d4 : 6;
		unsigned int d3 : 6;
		unsigned int d2 : 6;
		unsigned int d1 : 6;
	};

	//member functions
	bool IsEmailValid(std::string email_address);
	LPCWSTR stringToLPCWSTR(std::string str);
	char ConvertToBase64(char c6);
	void EncodeBase64(char *dbuf, char *buf128, int len);
	int  OpenSocket(struct sockaddr *addr);
	void sendMail(char* email, const char* body);
};

#endif  //SITEMON_HPP