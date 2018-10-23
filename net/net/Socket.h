#pragma once
#include "pch.h"
using namespace std;
#ifndef SOCKET_H
#define SOCKET_H
//winsock  API链接文件

//服务器监听客户端，被动链接,绑定本地端口
#define SOCKETBIND 1
//服务器主动链接客户端,不绑定本地端口
#define SOCKETNOTBIND 2
//超时 毫秒
#define TIMEOUT 50


class Socket
{
	
public:
	SOCKET m_socket;
	//lib状态
	BOOL libState;
	//函数
	Socket(int lowver, int higver);
	virtual ~Socket();
	//初始化socket
	SOCKET init(int type, string ip, u_short port, int opt);
	//针对本地监听的处理函数
	SOCKET Accept(SOCKET s, struct sockaddr *addr, int *addrlen);
	//发送信息
	int Send(SOCKET s,const char*buf,int len,int flag,int timeout);
	//接受信息
	int Recv(SOCKET s, char*buf, int len, int flag, int timeout, char *EndMark, BOOL soonflag = FALSE);

	//初始化，winsock  API链接文件
	static BOOL initLib(int lowver, int higver);
	static BOOL freeLib();

};

#endif