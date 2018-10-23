#include "pch.h"
#include "common.h"

using namespace std;
vector<ULONG>getAddr(std::string host)
{
		struct addrinfo hint;
		struct addrinfo *res = NULL, *cur = NULL;
		int ret;
		struct sockaddr_in *addr;
		vector<ULONG> addrlist;//地址列表
		//初始化
		memset(&hint, 0, sizeof(struct addrinfo));
		hint.ai_flags = AI_ALL; /* For wildcard IP address */
		hint.ai_family = 0;
		hint.ai_socktype = 0;
		hint.ai_protocol = 0;
		hint.ai_addrlen = 0;
		hint.ai_canonname = NULL;
		hint.ai_addr = NULL;
		hint.ai_next = NULL;

		ret = getaddrinfo(host.c_str(), NULL, &hint, &res);
		if (ret == -1 || res == NULL)
		{
			//Socket::tip(gai_strerror(ret));
			return addrlist;
		}
		for (cur = res; cur != NULL; cur = cur->ai_next) {
			addr = (struct sockaddr_in *)cur->ai_addr;
			addrlist.push_back(addr->sin_addr.S_un.S_addr);
			//printf("%s\n", inet_ntop(AF_INET,&addr->sin_addr, ipbuf, 16));
		}
		freeaddrinfo(res);
		return addrlist;
}

ULONG tranfer(string ip)
{
	ULONG s;
	int state = inet_pton(AF_INET, ip.c_str(), (void*)&s);
	if (state <= 0)
		return INADDR_NONE;
	return s;
}

string tranfer(ULONG ip)
{
	char buf[128];
	inet_ntop(AF_INET, (void*)&(ip), buf, sizeof(buf));
	return string(buf);
}
struct in_addr dtranfer(string ips)
{
	ULONG ip = tranfer(ips);
	return dtranfer(ip);
}

struct in_addr dtranfer(ULONG ip)
{
	char strip[128];
	struct in_addr s;
	inet_ntop(AF_INET, &ip, strip, sizeof(strip));
	inet_pton(AF_INET, strip, (void*)&s);

	return s;
}
const char* Level[] = { "error" ,"warnning","info","debug"};
int Log(fstream &_stdout,int level, const char* messsage)
{
	//时间
	time_t timep;
	tm t;
	char tmp[64];
	time(&timep);
	localtime_s(&t, &timep);
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", &t);
	//组成字符串
	char buf[1024];
	sprintf_s(buf,sizeof(buf), "\n[%s][%s]: %s", tmp, Level[level], messsage);
	_stdout << buf;
	return 0;
}
