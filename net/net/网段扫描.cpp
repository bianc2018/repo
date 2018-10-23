#include"pch.h"
#include <iostream>
#include <fstream>
#include "common.h"
#include "Socket.h"
#include "Scanner.h"
using namespace std;

int ssmain()
{
	try
	{
		Socket::initLib(2, 2);
		fstream file;
		char buf[1024];
		file.open("./log.txt", ios::app);
		Log(file, 1, "ttttt");
		Scanner scan;
		vector<string> iplist = scan.scanByPing("xz");
		int size = iplist.size();
		sprintf_s(buf, sizeof(buf), "扫描网段：存在主机数:%d", size);
		Log(file, LDEBUG, buf);

		for (int i = 0; i < size; i++)
		{
			sprintf_s(buf, sizeof(buf), "opened ip:%s",iplist[i].c_str());
			Log(file, LDEBUG, buf);
			cout << buf << endl;
		}
		for (int i = 0; i < size; i++)
		{
			sprintf_s(buf, sizeof(buf), "ip:%s", iplist[i].c_str());
			Log(file, LDEBUG, buf);
			cout << buf << endl;

			vector<int > plist = scan.gsg(iplist[i]);
			int psize = plist.size();
			sprintf_s(buf, sizeof(buf), "ip:%s,开启端口数:%d", iplist[i].c_str(), psize);
			Log(file, LDEBUG, buf);
			cout << buf << endl;

			for (int j = 0; j < psize; j++)
			{
				sprintf_s(buf, sizeof(buf), "已开启端口:%d", plist[j]);
				Log(file, LDEBUG, buf);
				cout << buf << endl;
			}
		}
		file.close();
		Socket::freeLib();
	}
	catch (...)
	{
		cout << "错误" << endl;
	}
	system("pause");
}