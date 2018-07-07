
#include "SendMail.h"
#include<string>

int main()
{
	int num = 100;
	string EmailContents = "From: \"chen\"<895846885@qq.com>\r\n"
		+ string("To: \"Fang\"<945338423@qq.com>\r\n")
		+ "Subject: Hello\r\n\r\n"
		+ "test sending variable" + to_string(num) + "\n";

	char EmailTo[] = "945338423@qq.com";    //此处是送达的邮箱, 需和EmailContents里的保持一致


	SendMail(EmailTo, EmailContents.c_str());
	return 0;
}
