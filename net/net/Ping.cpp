#include "pch.h"
#include "Ping.h"
#define MIN_ICMP_PACKET_SIZE 8
#define MAX_ICMP_PACKET_SIZE 1024
using namespace std;
Ping::Ping()
{
	create = NULL;
	close = NULL;
	sendEcho = NULL;
	//����ICMP DLL��
	icmphandle = LoadLibrary(TEXT("C:/Windows/SysWOW64/icmp.dll"));
	if (icmphandle == NULL)
	{
		cout << GetLastError() << endl; // ��ô�����Ϣ�����ȱ��dll����126,��ȷ����0
		//Socket::tip(gai_strerror(GetLastError()));
		return;
	}
	//�õ�IcmpCreateFile��IcmpSendEcho��IcmpCloseHandle ������ַ
	create = (IcmpCreateFile)GetProcAddress(icmphandle, "IcmpCreateFile");
	sendEcho = (IcmpSendEcho)GetProcAddress(icmphandle, "IcmpSendEcho");
	close = (IcmpCloseHandle)GetProcAddress(icmphandle, "IcmpCloseHandle");
	
	//���Ƿ���سɹ�
	if (create == NULL || sendEcho == NULL || close == NULL)
	{
		cout<<"ICMP ��������ʧ�ܣ�\n";
		//�ͷŶ�̬��
		if (icmphandle)
		{
			FreeLibrary(icmphandle);
			icmphandle = NULL;
		}
	}
}


void Ping::print(const ICMP_ECHO_REPLY Reply)
{
	
	cout << "ICMP_ECHO_REPLY��";
	cout << "Address:" << tranfer(Reply.Address);
	cout << ",Status:" << Reply.Status ;
	cout << ",RoundTripTime(RTT):" << Reply.RoundTripTime<<" ms";
	cout << ",DataSize:" << Reply.DataSize <<" Byte";
	cout << ",Reserved��" << Reply.Reserved ;
	
	cout << "\nIP_OPTION_REPLY��(";

	cout << "Ttl:" << Reply.Options.Ttl;
	cout << ",Tos:" << Reply.Options.Tos ;
	cout << ",Flags:" << Reply.Options.Flags;
	cout << ",OptionSize:" << Reply.Options.OptionsSize <<")"<< endl;

}

ICMP_ECHO_REPLY Ping::ping(UINT TimeOut, LPCSTR Host, HWND IcmpHandle)
{
	ICMP_ECHO_REPLY reply;
	memset(&reply, 0, sizeof(reply));
	reply.Status = 1;
	if (icmphandle == NULL)
		return reply;

	//DWORD dwTimeout = 1000;//��ʱʱ��
	UCHAR nPacketSize = 32;//���ݰ���С
	//��������ʽ��IP��ַת��Ϊһ�����ֵ�Internet��ַ
	ULONG addr = INADDR_NONE;
	addr = tranfer(Host);
	if (addr== INADDR_NONE)
	{
		std::vector<ULONG> addrlist = getAddr(Host);
		if (addrlist.size() >= 1)
		{
			for (int i = 0; i < addrlist.size(); i++)
			{
				addr = addrlist[i];
				if (addr != 0)
					break;
			}
			if (addr == 0)
				return reply;
		}
		else
			return reply;
	}
	//����ICMP���
	HANDLE hIP = create();
	if (hIP == INVALID_HANDLE_VALUE)
		return reply;
	//���ṹ��
	IP_OPTION_INFORMATION OptInfo;
	ZeroMemory(&OptInfo, sizeof(IP_OPTION_INFORMATION));
	OptInfo.Ttl = 128;
	//���Ҫ���͵�����
	unsigned char pBuf[36];
	memset(pBuf, 'E', nPacketSize);
	
	//ping
	int nReplySize = sizeof(ICMP_ECHO_REPLY) + max(MIN_ICMP_PACKET_SIZE, nPacketSize);
	unsigned char pReply[100];

	ICMP_ECHO_REPLY *p = (ICMP_ECHO_REPLY*)pReply;
	DWORD nRecvPackets = sendEcho(hIP, addr, pBuf, nPacketSize, &OptInfo, &pReply, nReplySize, TimeOut);
	//����Ƿ��а�
	memcpy(&reply, p, sizeof(ICMP_ECHO_REPLY));
	if ((nRecvPackets == 1) && (reply.Status!=0))
	{
		SetLastError(reply.Status);
	}
	close(hIP);
	return reply;
}

int Ping::pingThread(vector<string>& iplist, vector<ICMP_ECHO_REPLY>& replist, int & pos, UINT TimeOut)
{
	//DWORD dwTimeout = 1000;//��ʱʱ��
	UCHAR nPacketSize = 32;//���ݰ���С
	//��������ʽ��IP��ַת��Ϊһ�����ֵ�Internet��ַ
	ULONG addr = INADDR_NONE;
	// ����ICMP���
	HANDLE hIP = create();
	if (hIP == INVALID_HANDLE_VALUE)
		return -1;
	//���ṹ��
	IP_OPTION_INFORMATION OptInfo;
	ZeroMemory(&OptInfo, sizeof(IP_OPTION_INFORMATION));
	OptInfo.Ttl = 128;
	//���Ҫ���͵�����
	unsigned char pBuf[36];
	memset(pBuf, 'E', nPacketSize);

	ICMP_ECHO_REPLY reply;
	m_lock.lock();
	int size = iplist.size();
	m_lock.unlock();

	while (true)
	{
		//ȡ��ַ
		m_lock.lock();
		if (pos < size)
		{
			cout << "ping:" << iplist[pos] << endl;
			addr = tranfer(iplist[pos]);
			pos++;
		}
		else
		{
			close(hIP);
			m_lock.unlock();
			return 1;
		}
		m_lock.unlock();
		//ping
		int nReplySize = sizeof(ICMP_ECHO_REPLY) + max(MIN_ICMP_PACKET_SIZE, nPacketSize);
		unsigned char pReply[100];

		ICMP_ECHO_REPLY *p = (ICMP_ECHO_REPLY*)pReply;
		DWORD nRecvPackets = sendEcho(hIP, addr, pBuf, nPacketSize, &OptInfo, &pReply, nReplySize, TimeOut);
		//����Ƿ��а�
		memcpy(&reply, p, sizeof(ICMP_ECHO_REPLY));

		m_lock.lock();
		cout << "Ping over:" <<tranfer(reply.Address)<<"[ Status:"<< reply.Status<<"]"<<"[Num:"<<replist.size()<<"]"<< endl;
		replist.push_back(reply);
		m_lock.unlock();
	}
	close(hIP);
	return -1;
}


Ping::~Ping()
{
	//��̬���ӿ��еĺ���ʹ����Ϻ�Ҫ��FreeLibrary()����ж�ظö�̬���ӿ�
	if (icmphandle)
	{
		FreeLibrary(icmphandle);
		icmphandle = NULL;
	}
}
//int pingThread(LPCSTR ip, vector<string> &iplist, vector<ICMP_ECHO_REPLY> &replist, int &pos, UINT TimeOut);
vector<ICMP_ECHO_REPLY> Ping::pings(vector<string>& iplist, UINT TimeOut,int ThreadNUM)
{
	vector<ICMP_ECHO_REPLY> replist;
	//�̳߳�
	vector<thread> threadpool;
	int pos = 0;
	for (int i = 0; i < ThreadNUM; i++)
	{
		threadpool.push_back(thread(&Ping::pingThread, this, std::ref(iplist),std::ref(replist), std::ref(pos),TimeOut));
	}
	int size = threadpool.size();
	for (int i = 0; i < size; i++)
		threadpool[i].join();
	return replist;
}
