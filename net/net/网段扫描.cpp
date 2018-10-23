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
		sprintf_s(buf, sizeof(buf), "ɨ�����Σ�����������:%d", size);
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
			sprintf_s(buf, sizeof(buf), "ip:%s,�����˿���:%d", iplist[i].c_str(), psize);
			Log(file, LDEBUG, buf);
			cout << buf << endl;

			for (int j = 0; j < psize; j++)
			{
				sprintf_s(buf, sizeof(buf), "�ѿ����˿�:%d", plist[j]);
				Log(file, LDEBUG, buf);
				cout << buf << endl;
			}
		}
		file.close();
		Socket::freeLib();
	}
	catch (...)
	{
		cout << "����" << endl;
	}
	system("pause");
}