#pragma once
#include "pch.h"
#include "common.h"
#include <thread>
#include <mutex>
#ifndef PING_H
#define PING_H
//定义 IP_OPTION_INFORMATION 结构
//ms
#define PING_TIMEOUT 2000
using namespace std;
struct IP_OPTION_INFORMATION
{
	unsigned char Ttl;//存活时间
	unsigned char Tos;//Type of Service
	unsigned char Flags;//ip header flags
	unsigned char OptionsSize;//数据大小
	unsigned char FAR* OptionsData;//数据指针
};
//定义 ICMP_ECHO_REPLY 结构
struct ICMP_ECHO_REPLY
{
	ULONG Address;					//地址
	unsigned long Status;			//状态
	unsigned long RoundTripTime;	//响应时间 RRT 
	unsigned short DataSize;		//返回数据大小 字节
	unsigned short Reserved;        //Reserved for system use
	void FAR	   *Data;			//数据指针
	IP_OPTION_INFORMATION Options; //操作

};
//起别名
typedef IP_OPTION_INFORMATION FAR*LPIP_OPTION_INFORMATION;
typedef ICMP_ECHO_REPLY FAR*LPICMP_ECHO_REPLY;
// 定义函数三个指针类型
typedef HANDLE(WINAPI* IcmpCreateFile)(VOID);
typedef BOOL(WINAPI* IcmpCloseHandle)(HANDLE IcmpHandle);
typedef DWORD(WINAPI* IcmpSendEcho)(HANDLE IcmpHandle, ULONG DestAddr, LPVOID Req, WORD ReqSize,LPIP_OPTION_INFORMATION ReqOpt, LPVOID buf, DWORD ReplySize, DWORD timeout); 

class Ping
{
	//句柄
	HINSTANCE icmphandle;
	//进程锁
	mutex m_lock;
	int pingThread(vector<string> &iplist, vector<ICMP_ECHO_REPLY> &replist, int &pos, UINT TimeOut);
public:
	Ping();
	void print(const ICMP_ECHO_REPLY Reply);
	//TimeOut ms
	ICMP_ECHO_REPLY ping(UINT TimeOut,LPCSTR Host,HWND IcmpHandle);
	IcmpCreateFile create;
	IcmpCloseHandle close;
	IcmpSendEcho sendEcho;
	virtual ~Ping();
	vector<ICMP_ECHO_REPLY> pings(vector<string> &iplist ,UINT TimeOut=500, int ThreadNUN = 100);
	//

};

#endif