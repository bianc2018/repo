#pragma once
#include <fstream>
#include <iostream>
#include "common.h"
using namespace std;
class IPNode
{
private:
	long m_lIPAddress;                //IP地址
	long m_lCount;                   //发送数据包数
public:
	IPNode*pNext;                   //指向下一个IP节点
	//构造函数
	IPNode(long sourceIP)
	{
		m_lIPAddress = sourceIP;
		m_lCount = 1;                 //初始化数据包个数为1
	}
	//数据包个数加1
	void addCount()
	{
		m_lCount++;
	}
	//返回数据包个数
	long getCount()
	{
		return m_lCount;
	}
	//返回IP地址
	long getIPAddress()
	{
		return m_lIPAddress;
	}
};
//节点链表
class NodeList
{
	IPNode*pHead;                //链表头
	IPNode*pTail;                 //链表尾
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
	//IP节点加入链表
	void addNode(long sourceIP)
	{
		IPNode* pTemp;
		if (pHead == NULL)             //当链表为空时
		{
			pTail = new IPNode(sourceIP);
			pHead = pTail;
			pTail->pNext = NULL;
		}
		else                      //不为空时
		{
			for (pTemp = pHead; pTemp; pTemp = pTemp->pNext)
			{
				//如果链表中存在此IP，发送数据包个数加1
				if (pTemp->getIPAddress() == sourceIP)
				{
					pTemp->addCount();
					break;
				}
			}
			//如果链表中没有此IP，则加入链表
			if (pTemp == NULL)
			{
				pTail->pNext = new IPNode(sourceIP);
				pTail = pTail->pNext;
				pTail->pNext = NULL;
			}
		}
	}
	//输出IP结点，即IP地址和其它发送的IP包个数
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
