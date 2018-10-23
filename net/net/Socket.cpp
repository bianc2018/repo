#include "pch.h"
#include "Socket.h"


BOOL Socket::initLib(int lowver, int higver)
{
	WORD version = MAKEWORD(lowver,higver);//ָ�����ӿ�İ汾
	WSADATA wsData;

	int errret = WSAStartup(version, LPWSADATA(&wsData));

	if (LOBYTE(wsData.wVersion) != 2 || HIBYTE(wsData.wVersion) != 2)
	{
		//MessageBoxW(NULL, TEXT("winsocket ��汾����"), TEXT("��ʾ"), MB_OK);
		//tip(TEXT("winsocket ��汾����"));
		return FALSE;
	}
	return TRUE;
}

BOOL Socket::freeLib()
{
	//�ͷ���Դ
	WSACleanup();
	return TRUE;
}

Socket::Socket(int lowver, int higver)
{
	//��ʼ�����ӿ�
	libState = initLib(lowver, higver);
	//��ʼ��Ϊ��Чsocket
	m_socket = INVALID_SOCKET;
}


Socket::~Socket()
{
	//�ͷ���Դ
	freeLib();
}

SOCKET Socket::init(int type, string ip, u_short port, int opt)
{
	//����һ���ܹ���������ͨ�ŵ��׽���
	SOCKET socketid = socket(PF_INET, SOCK_STREAM, 0);
	//socket��ַ�ṹ
	SOCKADDR_IN sockStruct;

	//ʹ��TCP/IPЭ��
	sockStruct.sin_family = AF_INET;

	if (ip.empty())
	{
		//�����ipΪ�գ�����INADDR_ANY��ʾsocket���Խ��������κ�IP����Ϣ
		sockStruct.sin_addr.S_un.S_addr = INADDR_ANY;
	}
	else
	{
		//inet�ѹ�ʱ ��<Ws2tcpip.h>��inet_pton
		 inet_pton(AF_INET,ip.c_str(), (void*)&sockStruct.sin_addr.S_un.S_addr);
	}
	//����˿�
	sockStruct.sin_port = htons(port);
	//���󶨶˿�
	if (type == SOCKETNOTBIND)
	{
		//���ó�ʱ
		int nNetTimeout = TIMEOUT;
		//����ʱ��
		setsockopt(socketid,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout, sizeof(int));
		//��sockStruct�ṹָ���������ϵļ������������
		if (connect(socketid, (LPSOCKADDR)&sockStruct, sizeof(sockStruct)) == SOCKET_ERROR)
		{
			//����ʧ��
			//tip(TEXT("����ʧ��"));
			closesocket(socketid);
			//�ر��Ѵ򿪵��׽��֣���ֹռ���ڴ�
			shutdown(socketid, 2);
			//�����׽�����Ч
			socketid = INVALID_SOCKET;
		}
		m_socket = socketid;
	}
	else if (type == SOCKETBIND)
	{
		if (::bind(socketid, (sockaddr*)&sockStruct, sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			//tip(TEXT("�󶨶˿�ʧ��"));
			//�󶨶˿�ʧ��
			closesocket(socketid);
			socketid = INVALID_SOCKET;
		}
		else
		{
			if (listen(socketid, SOMAXCONN) == SOCKET_ERROR)
			{
				//tip(TEXT("������������ʧ��"));
				//������������ʧ��
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
	//�������
	int ret, nleft = len, idx = 0;
	fd_set readfds;
	//���ó�ʱֵ
	struct timeval TimeOut;
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 500;
	DWORD s_time = GetTickCount();

	while (nleft > 0)
	{
		//������Ϣ
		MSG msg;
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		//�˳�
		if (msg.message == WM_QUIT)
			return 0;

		//��set���㣬��fd����set
		FD_ZERO(&readfds);
		FD_SET(s, &readfds);

		int errorret = select(0, NULL, &readfds, NULL, &TimeOut);
		if (errorret == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		//���㵱ǰʱ��
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
	//�������
	int ret, nleft = len, idx = 0, ncount = 0;
	//fd_set���ļ��������ļ���
	fd_set readfds;
	//���ó�ʱֵ
	struct timeval TimeOut;
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 500;
	DWORD s_time = GetTickCount();

	while (nleft > 0)
	{
		//������Ϣ
		MSG msg;
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		//�˳�
		if (msg.message == WM_QUIT)
			return 0;

		//��set���㣬��fd����set
		FD_ZERO(&readfds);
		FD_SET(s, &readfds);
		
		//select �������ڹ����׽���I/O����������޹�����
		if (select(0, &readfds, NULL, NULL, &TimeOut) == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		DWORD e_time = GetTickCount();

		if (!FD_ISSET(s, &readfds))
		{
			//��ʱ
			if (e_time - s_time > timeout * 1000)
				return SOCKET_ERROR;
			else
				continue;
		}

		//��������
		ret = recv(s, &buf[idx], nleft, flag);

		if (soonflag == TRUE)
		{
			//�������ؽ��յ����ֽ���
			return ret;
		}

		//����ʱ��
		s_time = e_time;

		if (ret <= 0)
		{
			//������
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
