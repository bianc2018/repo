#include "pch.h"
#include "Socket.h"


BOOL Socket::initLib(int lowver, int higver)
{
	WORD version = MAKEWORD(lowver,higver);//指定链接库的版本
	WSADATA wsData;

	int errret = WSAStartup(version, LPWSADATA(&wsData));

	if (LOBYTE(wsData.wVersion) != 2 || HIBYTE(wsData.wVersion) != 2)
	{
		//MessageBoxW(NULL, TEXT("winsocket 库版本过低"), TEXT("提示"), MB_OK);
		//tip(TEXT("winsocket 库版本过低"));
		return FALSE;
	}
	return TRUE;
}

BOOL Socket::freeLib()
{
	//释放资源
	WSACleanup();
	return TRUE;
}

Socket::Socket(int lowver, int higver)
{
	//初始化链接库
	libState = initLib(lowver, higver);
	//初始化为无效socket
	m_socket = INVALID_SOCKET;
}


Socket::~Socket()
{
	//释放资源
	freeLib();
}

SOCKET Socket::init(int type, string ip, u_short port, int opt)
{
	//创建一个能够进行网络通信的套接字
	SOCKET socketid = socket(PF_INET, SOCK_STREAM, 0);
	//socket地址结构
	SOCKADDR_IN sockStruct;

	//使用TCP/IP协议
	sockStruct.sin_family = AF_INET;

	if (ip.empty())
	{
		//输入的ip为空，则用INADDR_ANY表示socket可以接受来自任何IP的信息
		sockStruct.sin_addr.S_un.S_addr = INADDR_ANY;
	}
	else
	{
		//inet已过时 用<Ws2tcpip.h>的inet_pton
		 inet_pton(AF_INET,ip.c_str(), (void*)&sockStruct.sin_addr.S_un.S_addr);
	}
	//保存端口
	sockStruct.sin_port = htons(port);
	//不绑定端口
	if (type == SOCKETNOTBIND)
	{
		//设置超时
		int nNetTimeout = TIMEOUT;
		//发送时限
		setsockopt(socketid,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout, sizeof(int));
		//与sockStruct结构指定的网络上的计算机进行链接
		if (connect(socketid, (LPSOCKADDR)&sockStruct, sizeof(sockStruct)) == SOCKET_ERROR)
		{
			//链接失败
			//tip(TEXT("连接失败"));
			closesocket(socketid);
			//关闭已打开的套接字，防止占用内存
			shutdown(socketid, 2);
			//设置套接字无效
			socketid = INVALID_SOCKET;
		}
		m_socket = socketid;
	}
	else if (type == SOCKETBIND)
	{
		if (::bind(socketid, (sockaddr*)&sockStruct, sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			//tip(TEXT("绑定端口失败"));
			//绑定端口失败
			closesocket(socketid);
			socketid = INVALID_SOCKET;
		}
		else
		{
			if (listen(socketid, SOMAXCONN) == SOCKET_ERROR)
			{
				//tip(TEXT("监听函数调用失败"));
				//监听函数调用失败
				closesocket(socketid);
				socketid = INVALID_SOCKET;
			}
		}
		m_socket = socketid;
	}

	return socketid;
}

SOCKET Socket::Accept(SOCKET s, sockaddr * addr, int * addrlen)
{
	
	return accept(s,addr,addrlen);
}

int Socket::Send(SOCKET s, const char * buf, int len, int flag, int timeout)
{
	//定义变量
	int ret, nleft = len, idx = 0;
	fd_set readfds;
	//设置超时值
	struct timeval TimeOut;
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 500;
	DWORD s_time = GetTickCount();

	while (nleft > 0)
	{
		//接受信息
		MSG msg;
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		//退出
		if (msg.message == WM_QUIT)
			return 0;

		//将set清零，将fd加入set
		FD_ZERO(&readfds);
		FD_SET(s, &readfds);

		int errorret = select(0, NULL, &readfds, NULL, &TimeOut);
		if (errorret == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		//计算当前时间
		DWORD e_time = GetTickCount();
		if (!FD_ISSET(s, &readfds))
		{
			if (e_time - s_time > timeout * 1000)
			{
				return 0;
			}
			else
			{
				continue;
			}
		}
		ret = send(s, &buf[idx], nleft, flag);
		if (ret <= 0)
			return ret;
		nleft -= ret;
		idx += ret;
	}
	return len;
}

int Socket::Recv(SOCKET s, char * buf, int len, int flag, int timeout, char *EndMark, BOOL soonflag)
{
	//定义变量
	int ret, nleft = len, idx = 0, ncount = 0;
	//fd_set是文件描述符的集合
	fd_set readfds;
	//设置超时值
	struct timeval TimeOut;
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 500;
	DWORD s_time = GetTickCount();

	while (nleft > 0)
	{
		//接受信息
		MSG msg;
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		//退出
		if (msg.message == WM_QUIT)
			return 0;

		//将set清零，将fd加入set
		FD_ZERO(&readfds);
		FD_SET(s, &readfds);
		
		//select 函数用于管理套接字I/O，避免出现无故锁定
		if (select(0, &readfds, NULL, NULL, &TimeOut) == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		DWORD e_time = GetTickCount();

		if (!FD_ISSET(s, &readfds))
		{
			//超时
			if (e_time - s_time > timeout * 1000)
				return SOCKET_ERROR;
			else
				continue;
		}

		//接受数据
		ret = recv(s, &buf[idx], nleft, flag);

		if (soonflag == TRUE)
		{
			//立即返回接收到的字节数
			return ret;
		}

		//重置时间
		s_time = e_time;

		if (ret <= 0)
		{
			//错误处理
			int lastError = GetLastError();
			
			if ((-1 == ret) && (WSAETIMEDOUT == lastError))
				continue;
			if ((-1 == ret) && (WSAEWOULDBLOCK == lastError))
			{
				if (ncount < 2000)
				{
					Sleep(10);
					ncount++;
					continue;
				}
			}
			return ret;
		}
		ncount = 0;

		nleft -= ret;
		idx += ret;

		if (EndMark != NULL && idx > 5)
		{
			if (strstr(buf + (idx - 5), EndMark) != NULL)
			{
				break;
			}
		}
	}
	return idx;
}
