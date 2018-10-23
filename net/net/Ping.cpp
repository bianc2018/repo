#include "pch.h"
#include "Ping.h"
#define MIN_ICMP_PACKET_SIZE 8
#define MAX_ICMP_PACKET_SIZE 1024
using namespace std;
Ping::Ping()
{
	create = NULL;
	close = NULL;
	sendEcho = NULL;
	//加载ICMP DLL库
	icmphandle = LoadLibrary(TEXT("C:/Windows/SysWOW64/icmp.dll"));
	if (icmphandle == NULL)
	{
		cout << GetLastError() << endl; // 获得错误信息，如果缺少dll，报126,正确返回0
		//Socket::tip(gai_strerror(GetLastError()));
		return;
	}
	//得到IcmpCreateFile、IcmpSendEcho、IcmpCloseHandle 函数地址
	create = (IcmpCreateFile)GetProcAddress(icmphandle, "IcmpCreateFile");
	sendEcho = (IcmpSendEcho)GetProcAddress(icmphandle, "IcmpSendEcho");
	close = (IcmpCloseHandle)GetProcAddress(icmphandle, "IcmpCloseHandle");
	
	//看是否加载成功
	if (create == NULL || sendEcho == NULL || close == NULL)
	{
		cout<<"ICMP 函数加载失败！\n";
		//释放动态库
		if (icmphandle)
		{
			FreeLibrary(icmphandle);
			icmphandle = NULL;
		}
	}
}


void Ping::print(const ICMP_ECHO_REPLY Reply)
{
	
	cout << "ICMP_ECHO_REPLY：";
	cout << "Address:" << tranfer(Reply.Address);
	cout << ",Status:" << Reply.Status ;
	cout << ",RoundTripTime(RTT):" << Reply.RoundTripTime<<" ms";
	cout << ",DataSize:" << Reply.DataSize <<" Byte";
	cout << ",Reserved：" << Reply.Reserved ;
	
	cout << "\nIP_OPTION_REPLY：(";

	cout << "Ttl:" << Reply.Options.Ttl;
	cout << ",Tos:" << Reply.Options.Tos ;
	cout << ",Flags:" << Reply.Options.Flags;
	cout << ",OptionSize:" << Reply.Options.OptionsSize <<")"<< endl;

}

ICMP_ECHO_REPLY Ping::ping(UINT TimeOut, LPCSTR Host, HWND IcmpHandle)
{
	ICMP_ECHO_REPLY reply;
	memset(&reply, 0, sizeof(reply));
	reply.Status = 1;
	if (icmphandle == NULL)
		return reply;

	//DWORD dwTimeout = 1000;//超时时间
	UCHAR nPacketSize = 32;//数据包大小
	//将点间隔格式的IP地址转换为一个数字的Internet地址
	ULONG addr = INADDR_NONE;
	addr = tranfer(Host);
	if (addr== INADDR_NONE)
	{
		std::vector<ULONG> addrlist = getAddr(Host);
		if (addrlist.size() >= 1)
		{
			for (int i = 0; i < addrlist.size(); i++)
			{
				addr = addrlist[i];
				if (addr != 0)
					break;
			}
			if (addr == 0)
				return reply;
		}
		else
			return reply;
	}
	//创建ICMP句柄
	HANDLE hIP = create();
	if (hIP == INVALID_HANDLE_VALUE)
		return reply;
	//填充结构体
	IP_OPTION_INFORMATION OptInfo;
	ZeroMemory(&OptInfo, sizeof(IP_OPTION_INFORMATION));
	OptInfo.Ttl = 128;
	//填充要发送的数据
	unsigned char pBuf[36];
	memset(pBuf, 'E', nPacketSize);
	
	//ping
	int nReplySize = sizeof(ICMP_ECHO_REPLY) + max(MIN_ICMP_PACKET_SIZE, nPacketSize);
	unsigned char pReply[100];

	ICMP_ECHO_REPLY *p = (ICMP_ECHO_REPLY*)pReply;
	DWORD nRecvPackets = sendEcho(hIP, addr, pBuf, nPacketSize, &OptInfo, &pReply, nReplySize, TimeOut);
	//检测是否有包
	memcpy(&reply, p, sizeof(ICMP_ECHO_REPLY));
	if ((nRecvPackets == 1) && (reply.Status!=0))
	{
		SetLastError(reply.Status);
	}
	close(hIP);
	return reply;
}

int Ping::pingThread(vector<string>& iplist, vector<ICMP_ECHO_REPLY>& replist, int & pos, UINT TimeOut)
{
	//DWORD dwTimeout = 1000;//超时时间
	UCHAR nPacketSize = 32;//数据包大小
	//将点间隔格式的IP地址转换为一个数字的Internet地址
	ULONG addr = INADDR_NONE;
	// 创建ICMP句柄
	HANDLE hIP = create();
	if (hIP == INVALID_HANDLE_VALUE)
		return -1;
	//填充结构体
	IP_OPTION_INFORMATION OptInfo;
	ZeroMemory(&OptInfo, sizeof(IP_OPTION_INFORMATION));
	OptInfo.Ttl = 128;
	//填充要发送的数据
	unsigned char pBuf[36];
	memset(pBuf, 'E', nPacketSize);

	ICMP_ECHO_REPLY reply;
	m_lock.lock();
	int size = iplist.size();
	m_lock.unlock();

	while (true)
	{
		//取地址
		m_lock.lock();
		if (pos < size)
		{
			cout << "ping:" << iplist[pos] << endl;
			addr = tranfer(iplist[pos]);
			pos++;
		}
		else
		{
			close(hIP);
			m_lock.unlock();
			return 1;
		}
		m_lock.unlock();
		//ping
		int nReplySize = sizeof(ICMP_ECHO_REPLY) + max(MIN_ICMP_PACKET_SIZE, nPacketSize);
		unsigned char pReply[100];

		ICMP_ECHO_REPLY *p = (ICMP_ECHO_REPLY*)pReply;
		DWORD nRecvPackets = sendEcho(hIP, addr, pBuf, nPacketSize, &OptInfo, &pReply, nReplySize, TimeOut);
		//检测是否有包
		memcpy(&reply, p, sizeof(ICMP_ECHO_REPLY));

		m_lock.lock();
		cout << "Ping over:" <<tranfer(reply.Address)<<"[ Status:"<< reply.Status<<"]"<<"[Num:"<<replist.size()<<"]"<< endl;
		replist.push_back(reply);
		m_lock.unlock();
	}
	close(hIP);
	return -1;
}


Ping::~Ping()
{
	//动态链接库中的函数使用完毕后要用FreeLibrary()函数卸载该动态链接库
	if (icmphandle)
	{
		FreeLibrary(icmphandle);
		icmphandle = NULL;
	}
}
//int pingThread(LPCSTR ip, vector<string> &iplist, vector<ICMP_ECHO_REPLY> &replist, int &pos, UINT TimeOut);
vector<ICMP_ECHO_REPLY> Ping::pings(vector<string>& iplist, UINT TimeOut,int ThreadNUM)
{
	vector<ICMP_ECHO_REPLY> replist;
	//线程池
	vector<thread> threadpool;
	int pos = 0;
	for (int i = 0; i < ThreadNUM; i++)
	{
		threadpool.push_back(thread(&Ping::pingThread, this, std::ref(iplist),std::ref(replist), std::ref(pos),TimeOut));
	}
	int size = threadpool.size();
	for (int i = 0; i < size; i++)
		threadpool[i].join();
	return replist;
}
