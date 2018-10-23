#pragma once
//扫描器
#include "pch.h"
#include "Socket.h"
#include "Ping.h"
#include <thread>
#include <mutex>
#include <random>
#ifndef SCANNER_H
#define SCANNER_H
using namespace std;

//随机获取端口
class RandomGetPort
{
	int begin;
	int end;
	int num;//已获取的端口数

	int *plist;

public :
	RandomGetPort(int b, int e);
	~RandomGetPort();
	int operator()();//已用完返回-1
	bool isEnd();//已用完返回-1
	double pro();
};

class Scanner
{
	mutex m; //线程锁
	//线程扫描,rs 返回值
	void GsgByThread(Socket s, string ip, RandomGetPort &rg, vector<int>&rs);
public:
	//socket 连接扫描端口 [beginport-endport) TheadNUM最大线程数
	//return 成功返回 开启的端口号
	vector<int> gsg(string ip= "127.0.0.1", int beginport=1, int endport=65535,int ThreadNUM=1000);
	//扫描网段，ping
	vector<string> scanByPing(string Host);
};

#endif