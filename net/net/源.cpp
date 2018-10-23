#include"pch.h"
#include <iostream>
#include "common.h"
#include "Socket.h"
using namespace std;
void server()
{
	try
	{
		Socket s(2, 2);
		s.init(SOCKETBIND, "172.18.151.129", 12340, 0);
		sockaddr addr;
		int size;
		cout << "[" << s.m_socket << "]Listening..." << endl;
		SOCKET ip = s.Accept(s.m_socket, &addr, &size);
		sockaddr_in daddr;
		memcpy(&daddr, &addr, sizeof(daddr));
		cout << "ip:" << tranfer(daddr.sin_addr.S_un.S_addr) << endl;
		s.Send(ip, "hello world!", 13, 0, 1000);
	}
	catch (...)
	{
		cout << "´íÎó" << endl;
	}
	system("pause");
}
void client()
{
	try
	{
		Socket s(2, 2);
		s.init(SOCKETNOTBIND, "172.18.151.129", 12340, 0);
		cout << "[" << s.m_socket << "]connecting..." << endl;
		char message[1024];
		int size = s.Recv(s.m_socket, message, sizeof(message),0,1000,0);
		cout <<"["<<size<<"]recv£º"<< message << endl;
	}
	catch (...)
	{
		cout << "´íÎó" << endl;
	}
	system("pause");
}
int main()
{
	client();
	return 0;
}
