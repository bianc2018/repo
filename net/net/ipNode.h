#pragma once
#include <fstream>
#include <iostream>
#include "common.h"
using namespace std;
class IPNode
{
private:
	long m_lIPAddress;                //IP��ַ
	long m_lCount;                   //�������ݰ���
public:
	IPNode*pNext;                   //ָ����һ��IP�ڵ�
	//���캯��
	IPNode(long sourceIP)
	{
		m_lIPAddress = sourceIP;
		m_lCount = 1;                 //��ʼ�����ݰ�����Ϊ1
	}
	//���ݰ�������1
	void addCount()
	{
		m_lCount++;
	}
	//�������ݰ�����
	long getCount()
	{
		return m_lCount;
	}
	//����IP��ַ
	long getIPAddress()
	{
		return m_lIPAddress;
	}
};
//�ڵ�����
class NodeList
{
	IPNode*pHead;                //����ͷ
	IPNode*pTail;                 //����β
public:
	NodeList()
	{
		pHead = pTail = NULL;
	}
	~NodeList()
	{

		if (pHead != NULL)
		{
			IPNode*pTemp = pHead;
			pHead = pHead->pNext;
			delete  pTemp;
		}
	}
	//IP�ڵ��������
	void addNode(long sourceIP)
	{
		IPNode* pTemp;
		if (pHead == NULL)             //������Ϊ��ʱ
		{
			pTail = new IPNode(sourceIP);
			pHead = pTail;
			pTail->pNext = NULL;
		}
		else                      //��Ϊ��ʱ
		{
			for (pTemp = pHead; pTemp; pTemp = pTemp->pNext)
			{
				//��������д��ڴ�IP���������ݰ�������1
				if (pTemp->getIPAddress() == sourceIP)
				{
					pTemp->addCount();
					break;
				}
			}
			//���������û�д�IP�����������
			if (pTemp == NULL)
			{
				pTail->pNext = new IPNode(sourceIP);
				pTail = pTail->pNext;
				pTail->pNext = NULL;
			}
		}
	}
	//���IP��㣬��IP��ַ���������͵�IP������
	ostream& print(ostream& os)
	{
		for (IPNode*pTemp = pHead; pTemp; pTemp = pTemp->pNext)
		{
			long lTemp = pTemp->getIPAddress();
			os << tranfer(lTemp) << '\t';
			os << pTemp->getCount() << endl;
		}
		return os;
	}
};
