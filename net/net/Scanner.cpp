#include "pch.h"
#include "Scanner.h"
using namespace std;
//using namespace common;

vector<int> Scanner::gsg(string ip, int beginport, int endport, int ThreadNUM)
{
	vector<int> opened;
	if (beginport > endport)
		return opened;
	Socket s(2, 2);
	if (!s.libState)
		return opened;
	RandomGetPort rg(beginport, endport);
	//线程池
	vector<thread> threadpool;
	for (int i = 0; i < ThreadNUM; i++)
	{
		threadpool.push_back(thread(&Scanner::GsgByThread, this, s, ip,std::ref(rg), std::ref(opened)));
	}
	for (int i = 0; i < ThreadNUM; i++)
		threadpool[i].join();
	return opened;
}

void Scanner::GsgByThread(Socket s,string ip, RandomGetPort &rg, vector<int>& rs)
{
	while (true)
	{
		m.lock();
		int port = rg();
		cout << "主机：" << ip << "扫描端口:" << port<<" 已扫描端口数:"<<rg.pro()<<" %"<< endl;
		m.unlock();
		if (port == -1)
			return;

		SOCKET sockFD = s.init(SOCKETNOTBIND, ip, port, 0);

		if (sockFD != INVALID_SOCKET)
		{
			m.lock();
			rs.push_back(port);
			cout << "主机：" << ip <<"已开启端口:"<<port << endl;
			m.unlock();
		}
		closesocket(sockFD);
	}
}

vector<string> Scanner::scanByPing(string Host)
{
	vector<string> ipList;
	vector<string> rsList;
	Ping p;
	try
	{
		//将点间隔格式的IP地址转换为一个数字的Internet地址 0,255
		ULONG addr = INADDR_NONE;
		addr = tranfer(Host);
		if (addr ==  INADDR_NONE)
		{
			std::vector<ULONG> addrlist = getAddr(Host);
			if (addrlist.size() >= 1)
			{
				for (int i = 0; i < addrlist.size(); i++)
				{

					addr = addrlist[i];
					if (addr != 0)
					{
						char ip[128];
						struct in_addr s = dtranfer(addr);
						unsigned char b1 = s.S_un.S_un_b.s_b1;
						unsigned char b2 = s.S_un.S_un_b.s_b2;
						unsigned char b3 = s.S_un.S_un_b.s_b3;

						for (int i = 0; i < 255; i++)
						{
							memset(ip, 0, sizeof(ip));
							sprintf_s(ip, "%d.%d.%d.%d", b1, b2, b3, i);
							ipList.push_back(ip);
						}
					}
				}
			}
		}
		else
		{
			struct in_addr s = dtranfer(addr);
			unsigned char b1 = s.S_un.S_un_b.s_b1;
			unsigned char b2 = s.S_un.S_un_b.s_b2;
			unsigned char b3 = s.S_un.S_un_b.s_b3;

			char ip[128];
			for (int i = 0; i < 255; i++)
			{
				memset(ip, 0, sizeof(ip));
				sprintf_s(ip, "%d.%d.%d.%d", b1, b2, b3, i);
				ipList.push_back(ip);
			}
		}

		//ping
		vector<ICMP_ECHO_REPLY> replist = p.pings(ipList, 2000, 100);
		int size = replist.size();
		for (int i = 0; i < size; i++)
		{
			if (replist[i].Status == 0)
			{
				rsList.push_back(tranfer(replist[i].Address));
			}
		}
		return rsList;
	} 
	catch (exception e)
	{
		cout << e.what() << endl;
		return rsList;
	}
}
#define RANDOM(x,y) x+rand()%(y-x)
RandomGetPort::RandomGetPort(int b, int e)
{
	if (e <= b || e < 0 || b < 0)
	{
		num = 0;
		plist = NULL;
		return ;
	}
	begin = b;
	end = e;
	num =0;

	bool* mask = new bool[e-b];
	memset(mask, true, sizeof(mask));

	srand(time(NULL));
	plist = new int[e - b];
	for (int i = 0; i < e - b;i++)
		plist[i] = i;
	cout << "init"<<endl;
	for (int i = 0; i < e - b; i++)
	{
		int k = RANDOM(0, end - begin);
		int t = plist[k];
		plist[k] = plist[i];
		plist[i] = t;
	}
	cout << "over" << endl;
	delete[]mask;
}

RandomGetPort::~RandomGetPort()
{
	delete[]plist;
}

int RandomGetPort::operator()()
{
	if (isEnd())
		return -1;
	int c = plist[num];
	num++;
	return c;
}



bool RandomGetPort::isEnd()
{
	if (plist == NULL)
		return true;
	if(end-begin>=num)
		return false;
	return true;
}

double RandomGetPort::pro()
{
	return (num/double(end-begin))*100;
}
