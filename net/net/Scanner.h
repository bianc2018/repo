#pragma once
//ɨ����
#include "pch.h"
#include "Socket.h"
#include "Ping.h"
#include <thread>
#include <mutex>
#include <random>
#ifndef SCANNER_H
#define SCANNER_H
using namespace std;

//�����ȡ�˿�
class RandomGetPort
{
	int begin;
	int end;
	int num;//�ѻ�ȡ�Ķ˿���

	int *plist;

public :
	RandomGetPort(int b, int e);
	~RandomGetPort();
	int operator()();//�����귵��-1
	bool isEnd();//�����귵��-1
	double pro();
};

class Scanner
{
	mutex m; //�߳���
	//�߳�ɨ��,rs ����ֵ
	void GsgByThread(Socket s, string ip, RandomGetPort &rg, vector<int>&rs);
public:
	//socket ����ɨ��˿� [beginport-endport) TheadNUM����߳���
	//return �ɹ����� �����Ķ˿ں�
	vector<int> gsg(string ip= "127.0.0.1", int beginport=1, int endport=65535,int ThreadNUM=1000);
	//ɨ�����Σ�ping
	vector<string> scanByPing(string Host);
};

#endif