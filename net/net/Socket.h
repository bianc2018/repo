#pragma once
#include "pch.h"
using namespace std;
#ifndef SOCKET_H
#define SOCKET_H
//winsock  API�����ļ�

//�����������ͻ��ˣ���������,�󶨱��ض˿�
#define SOCKETBIND 1
//�������������ӿͻ���,���󶨱��ض˿�
#define SOCKETNOTBIND 2
//��ʱ ����
#define TIMEOUT 50


class Socket
{
	
public:
	SOCKET m_socket;
	//lib״̬
	BOOL libState;
	//����
	Socket(int lowver, int higver);
	virtual ~Socket();
	//��ʼ��socket
	SOCKET init(int type, string ip, u_short port, int opt);
	//��Ա��ؼ����Ĵ�����
	SOCKET Accept(SOCKET s, struct sockaddr *addr, int *addrlen);
	//������Ϣ
	int Send(SOCKET s,const char*buf,int len,int flag,int timeout);
	//������Ϣ
	int Recv(SOCKET s, char*buf, int len, int flag, int timeout, char *EndMark, BOOL soonflag = FALSE);

	//��ʼ����winsock  API�����ļ�
	static BOOL initLib(int lowver, int higver);
	static BOOL freeLib();

};

#endif