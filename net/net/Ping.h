#pragma once
#include "pch.h"
#include "common.h"
#include <thread>
#include <mutex>
#ifndef PING_H
#define PING_H
//���� IP_OPTION_INFORMATION �ṹ
//ms
#define PING_TIMEOUT 2000
using namespace std;
struct IP_OPTION_INFORMATION
{
	unsigned char Ttl;//���ʱ��
	unsigned char Tos;//Type of Service
	unsigned char Flags;//ip header flags
	unsigned char OptionsSize;//���ݴ�С
	unsigned char FAR* OptionsData;//����ָ��
};
//���� ICMP_ECHO_REPLY �ṹ
struct ICMP_ECHO_REPLY
{
	ULONG Address;					//��ַ
	unsigned long Status;			//״̬
	unsigned long RoundTripTime;	//��Ӧʱ�� RRT 
	unsigned short DataSize;		//�������ݴ�С �ֽ�
	unsigned short Reserved;        //Reserved for system use
	void FAR	   *Data;			//����ָ��
	IP_OPTION_INFORMATION Options; //����

};
//�����
typedef IP_OPTION_INFORMATION FAR*LPIP_OPTION_INFORMATION;
typedef ICMP_ECHO_REPLY FAR*LPICMP_ECHO_REPLY;
// ���庯������ָ������
typedef HANDLE(WINAPI* IcmpCreateFile)(VOID);
typedef BOOL(WINAPI* IcmpCloseHandle)(HANDLE IcmpHandle);
typedef DWORD(WINAPI* IcmpSendEcho)(HANDLE IcmpHandle, ULONG DestAddr, LPVOID Req, WORD ReqSize,LPIP_OPTION_INFORMATION ReqOpt, LPVOID buf, DWORD ReplySize, DWORD timeout); 

class Ping
{
	//���
	HINSTANCE icmphandle;
	//������
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