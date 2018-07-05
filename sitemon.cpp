#include"stdafx.h"

#include<iostream>
#include<string>
#include<Windows.h>
#include<wininet.h>

#pragma comment(lib, "wininet.lib")
#pragma warning(disable:4996)

using std::cout;
using std::endl;
using std::cin;
using std::string;

LPCWSTR stringToLPCWSTR(string str)
{
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(str.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, str.length() + 1, str.c_str(), _TRUNCATE);
	return wcstring;
}

int main()
{
	string temp;
	cin >> temp;
	LPCWSTR url = stringToLPCWSTR(temp);

	//Analysis
	TCHAR szHostName[128];
	TCHAR szUrlPath[256];
	URL_COMPONENTS crackedURL = { 0 };
	crackedURL.dwStructSize = sizeof(URL_COMPONENTS);
	crackedURL.lpszHostName = szHostName;
	crackedURL.dwHostNameLength = ARRAYSIZE(szHostName);
	crackedURL.lpszUrlPath = szUrlPath;
	crackedURL.dwUrlPathLength = ARRAYSIZE(szUrlPath);
	InternetCrackUrl(url, (DWORD)_tcslen(url), 0, &crackedURL);

	//Initialization
	HINTERNET hInternet = InternetOpen(_T("Microsoft InternetExplorer"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternet == NULL)
	{
		cout << "Not install Microsoft InternetExplorer" << endl;
		return -1;
	}

	HINTERNET hHttpSession = InternetConnect(hInternet, crackedURL.lpszHostName, crackedURL.nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (hHttpSession == NULL)
	{
		InternetCloseHandle(hInternet);
		cout << "Connect error" << endl;
		return -2;
	}

	HINTERNET hHttpRequest = HttpOpenRequest(hHttpSession, _T("GET"), crackedURL.lpszUrlPath, NULL, _T(""), NULL, 0, 0);
	if (hHttpRequest == NULL)
	{
		InternetCloseHandle(hHttpSession);
		InternetCloseHandle(hInternet);
		cout << "Send error" << endl;
		return -3;
	}

	//Check the HTTP Status Code in loop
	while (true)
	{
		DWORD dwRetCode = 0;
		DWORD dwSizeOfRq = sizeof(DWORD);
		HttpSendRequest(hHttpRequest, NULL, 0, NULL, 0);
		HttpQueryInfo(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL);
		if (dwRetCode >= 500)
		{
			cout << "The server is down." << endl;
			break;
		}
		else
			cout << "The server is on." << endl;
		Sleep(60000);
	}
	
	//Closed
	InternetCloseHandle(hHttpRequest);
	InternetCloseHandle(hHttpSession);
	InternetCloseHandle(hInternet);
	return 0;
}
