#include "pch.h"
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//using namespace common;
#include <iomanip>
#include <iostream>
#include<string>
#include<cstring>
#include "common.h"
using namespace std;



//ethernet 帧头
struct ethernet
{                   //u_char是无符号的char型 而char是有符号的char型，u_char和char都占一个字节，8个bit位
	u_int8_t ether_dst[6];   //6位目的地址字段
	u_int8_t ether_src[6];   //6位源地址字段
	u_short ether_type;   //协议类型

};

//ip头部
struct ip
{

#if defined(WORDS_BIENDIAN)   
	u_int8_t   ip_v : 4,
		ip_hl : 4;
#else   
	u_int8_t   ip_hl : 4,
		ip_v : 4;
#endif   

	u_int8_t ip_tos;  //服务类型  8
	u_int16_t ip_len; //总长度   16
	u_int16_t ip_id;  //标示,标识这个IP数据包.
	u_int16_t ip_off; //标志位碎片偏移

	u_int8_t ip_ttl;  //TTL
	u_int8_t ip_pro;    //协议类型
	u_int16_t ip_cks; //头部校验和
	struct in_addr ip_src; //ip源
	struct in_addr ip_dst; //目的地址
};


//ICMP header
struct 	icmp
{
	u_int8_t icmp_type;  //类型
	u_int8_t code;  //代码
	u_int16_t icmp_cks;  //校验和
	u_int16_t icmp_id;    //标示
	u_int16_t icmp_seq;    //序列号

};


//ARP header
struct arp
{
	u_short arp_hrd;    //硬件地址类型
	u_short arp_pro;    //协议地址类型
	u_char arp_hln;    //硬件地址长度
	u_char arp_pln;    //协议地址长度
	u_short arp_op;     // ARP/RARP 操作
	u_char arp_eth_src[6];    //发送站硬件地址MAC

	u_char arp_src[4];    //发送站协议地址IP
	u_char arp_eth_dst[6];    //目的站硬件地址MAC
	u_char  arp_dst[4];    //目的站协议地址IP
};

void ip_packet_loop(u_char *pram, const struct pcap_pkthdr* header, const u_char* data);
void arp_packet_loop(u_char *pram, const struct pcap_pkthdr* header, const u_char* data);
void icmp_packet_loop(u_char *pram, const struct pcap_pkthdr* header, const u_char* data);

void display_data(const u_char* data, int length);





void ethernet_packet_loop(u_char *pram, const struct pcap_pkthdr* header, const u_char* data)
{
	struct ethernet *eth_h;
	eth_h = (struct ethernet*)data;//获得数据包内容
	static int p_num = 1;  //从第一个数据包开始
	cout << endl;
	cout << "第" << p_num << "个数据包" << endl;

	u_short eth_type;
	eth_type = ntohs(eth_h->ether_type);//以太网类型
	u_char *macstring1;
	u_char *macstring2;
	macstring1 = eth_h->ether_src;
	macstring2 = eth_h->ether_dst;

	cout << "*************帧结构***************" << endl;
	cout << "  目标MAC地址:     ";
	cout << setw(2) << setfill('0') << hex << int(*macstring2) << ":" << setw(2) << setfill('0') << hex << int(*(macstring2 + 1)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring2 + 2)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring2 + 3)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring2 + 4)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring2 + 5));
	cout << endl;
	cout << "  源MAC地址:       ";
	cout << setw(2) << setfill('0') << hex << int(*macstring1) << ":" << setw(2) << setfill('0') << hex << int(*(macstring1 + 1)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring1 + 2)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring1 + 3)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring1 + 4)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring1 + 5));
	cout << endl;



	cout << "  以太网类型为：   " << setw(4) << setfill('0') << hex << eth_type << endl;
	cout << "  上层协议为：    ";
	switch (eth_type)
	{
	case 0x0800: cout << " IP协议 " << endl; break;
	case 0x0806: cout << " ARP协议 " << endl; break;
	case 0x86dd: cout << " ICMP协议" << endl; break;
	default:break;
	}

	cout << endl;

	if (eth_type == 0x0800)//继续分析IP协议  
	{

		ip_packet_loop(pram, header, data);
	}


	else if (eth_type == 0x0806)
	{
		arp_packet_loop(pram, header, data);
	}



	p_num++;

}


void ip_packet_loop(u_char *pram, const struct pcap_pkthdr* header, const u_char* data)
{

	struct ip *ip_h;
	ip_h = (struct ip *) (data + 14);

	u_int offset;
	offset = ntohs(ip_h->ip_off);

	cout << "*************IP协议*************" << endl;
	cout << "  版本号          " << int(ip_h->ip_v) << endl;
	cout << "  首部长度        " << int(ip_h->ip_hl) << endl;
	cout << "  服务质量        " << ntohs(ip_h->ip_tos) << endl;
	cout << "  总长度          " << ntohs(ip_h->ip_len) << endl;
	cout << "  标识            " << ntohs(ip_h->ip_id) << endl;
	cout << "  偏移            " << setw(4) << setfill('0') << ntohs(ip_h->ip_off) << endl;
	cout << "  生存时间        " << int(ip_h->ip_ttl) << endl;
	cout << "  协议类型        " << setw(2) << setfill('0') << int(ip_h->ip_pro) << endl;
	cout << "  校验和          " << setw(4) << setfill('0') << ntohs(ip_h->ip_cks) << endl;

	cout << "  源IP地址：      " << tranfer(ip_h->ip_src.S_un.S_addr) << endl;
	cout << "  目的IP地址：    " << tranfer(ip_h->ip_dst.S_un.S_addr);

	cout << endl;


	cout << endl;


	if (ip_h->ip_pro == 1)   //转到分析Icmp
	{
		cout << "上层协议是ICMP协议" << endl;
		icmp_packet_loop(pram, header, data);
	}


}




void arp_packet_loop(u_char *pram, const struct pcap_pkthdr* header, const u_char* data)
{

	struct arp * arp_h;
	arp_h = (struct arp*)(data + 14);

	u_char *macstring1;
	u_char *macstring2;
	macstring1 = arp_h->arp_eth_src;
	macstring2 = arp_h->arp_eth_dst;

	switch (ntohs(arp_h->arp_op))
	{
	case 1:cout << "ARP请求协议" << endl; break;
	case 2:cout << "ARP应答协议" << endl; break;
	case 3:cout << "RARP请求协议" << endl; break;
	case 4:cout << "RARP应答协议" << endl; break;
	default:break;
	}
	cout << "*************ARP协议*************" << endl;

	cout << "  硬件类型:        " << ntohs(arp_h->arp_hrd) << endl;;
	cout << "  协议类型:        " << setw(4) << setfill('0') << ntohs(arp_h->arp_pro) << endl;
	cout << "  硬件地址长度:    " << int(arp_h->arp_hln) << endl;
	cout << "  协议地址长度:    " << int(arp_h->arp_pln) << endl;
	cout << "  操作码：         " << ntohs(arp_h->arp_op) << endl;


	cout << "  发送方MAC地址:   ";
	cout << setw(2) << setfill('0') << hex << int(*macstring1) << ":" << setw(2) << setfill('0') << hex << int(*(macstring1 + 1)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring1 + 2)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring1 + 3)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring1 + 4)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring1 + 5));
	cout << endl;


	struct in_addr arp_ip_dst;
	struct in_addr arp_ip_src;

	memcpy((void*)&arp_ip_src, (void*)&arp_h->arp_dst, sizeof(struct in_addr));

	cout << "  发送方IP:        " << tranfer(arp_ip_src.S_un.S_addr) << endl;

	cout << "  接收方MAC地址:   ";
	cout << setw(2) << setfill('0') << hex << int(*macstring2) << ":" << setw(2) << setfill('0') << hex << int(*(macstring2 + 1)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring2 + 2)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring2 + 3)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring2 + 4)) << ":" << setw(2) << setfill('0') << hex << int(*(macstring2 + 5));
	cout << endl;
	memcpy((void*)&arp_ip_dst, (void*)&arp_h->arp_dst, sizeof(struct in_addr));
	cout << "  接收方IP地址:    " << tranfer(arp_ip_dst.S_un.S_addr) << endl;

	cout << endl;

}


void icmp_packet_loop(u_char *pram, const struct pcap_pkthdr* header, const u_char* data)
{
	struct icmp *icmp_h;
	icmp_h = (struct icmp*)(data + 14 + 20);

	cout << "*************ICMP协议*************" << endl;
	cout << "  ICMP类型:    " << int(icmp_h->icmp_type) << endl;//获得ICMP类型
	cout << "  code:        " << ntohs(icmp_h->code) << " ";
	switch (icmp_h->icmp_type)
	{
		cout << "  ";
	case 0:cout << "回应应答" << endl; break;
	case 3:
		switch (icmp_h->code)
		{
		case 0:cout << "网络不可达" << endl; break;
		case 1:cout << "主机不可达" << endl; break;
		case 2:cout << "协议不可达" << endl; break;
		case 3:cout << "端口不可达" << endl; break;
		case 4:cout << "需要分段但设置为不允许分段" << endl; break;
		case 5:cout << "源路由失败" << endl; break;
		case 6:cout << "目的站点网络未知" << endl; break;
		case 7:cout << "目的主机网络未知" << endl; break;
		case 8:cout << "原主机被隔离" << endl; break;
		case 9:cout << "与目的站点网络的通信被禁止" << endl; break;
		case 10:cout << "与目的站点主机的通信被禁止" << endl; break;
		case 11:cout << "对请求的服务类型，网络不可达" << endl; break;
		case 12:cout << "对请求的服务类型,主机不可达" << endl; break;
		default:cout << "其他问题" << endl; break;
		}
		break;
	case 4:cout << "源抑制" << endl; break;
	case 5:switch (icmp_h->code)
	{
	case 0:cout << "网络重定向" << endl; break;
	case 1:cout << "主机重定向" << endl; break;
	case 2:cout << "服务类型和网络重定向" << endl; break;
	case 3:cout << "服务类型和主机重定向" << endl; break;
	}
	case 8:cout << "回应请求" << endl; break;
	case 9:cout << "路由器公告" << endl; break;
	case 10:cout << "路由器请求" << endl; break;
	case 11:switch (icmp_h->code)
	{
	case 0:cout << "传输期间TTL超时" << endl;
	case 1:cout << "数据段组装期间TTL超时" << endl;
	}break;
	case 12:switch (icmp_h->code)
	{
	case 0:cout << "损坏的IP头部" << endl;
	case 1:cout << "缺少选型" << endl;
	}break;
	case 13:cout << "时标请求" << endl; break;
	case 14:cout << "时标应答" << endl; break;
	case 15:cout << "信息请求" << endl; break;
	case 16:cout << "信息应答" << endl; break;
	case 17:cout << "地址掩码请求" << endl; break;
	case 18:cout << "地址掩码应答" << endl; break;

	default:break;
	}

	cout << "  ICMP校验和   " << setw(4) << setfill('0') << ntohs(icmp_h->icmp_cks) << endl;//获得ICMP校验和 
	cout << "  ID           " << setw(4) << setfill('0') << ntohs(icmp_h->icmp_id) << endl;
	cout << "  序列号       " << setw(4) << setfill('0') << ntohs(icmp_h->icmp_seq);
	cout << endl;

}




int getpacket()
{
	pcap_if_t *alldevs;   //所有的网络适配器
	pcap_if_t *d;         //选中的网络适配器
	int inum;
	int i = 0;              //适配器计数变量
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];  //错误缓冲区，用于保存错误
	u_int netmask;          //掩码地址    

	char packet_filter[128] = "";
	struct bpf_program fcode;

	/*
	int pcap_findalldebs_ex(
	char *source,
	struct pcap_rmtauth *auth,
	pcap_if_t **alldevs,
	char *errbuf);
	PCAP_SRC_IF_STRING代表用户想从一个本地文件开始捕获内容
	*/


	//获得设备列表
	char p[100] = PCAP_SRC_IF_STRING;
	if (pcap_findalldevs_ex(p, NULL, &alldevs, errbuf) == -1)
	{
		//结果为-1表示获取适配器表失败

		cout << "获取设备列表失败";
		exit(1);//exit（0）代表正常退出
	}

	//打印列表

	for (d = alldevs; d; d = d->next)  //从第一各设备开始，切换到另一个设备
	{
		cout << ++i << d->name;
		if (d->description)
			cout << d->description << endl;
		else
			cout << "没有获得详细信息描述" << endl;

	}
	if (i == 0)
	{
		cout << "没有找到接口，请确保您安装了Winpcap程序" << endl;
		return -1;
	}

	//cout << "请您输入设备号:  ";
	//cin >> inum;
	inum = 3;
	if (inum<1 || inum>i)
	{
		cout << "您输入的设备号超出了显示的范围" << endl;

		pcap_freealldevs(alldevs);//释放设备列表
		return -1;
	}

	//找到已选设备
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);
	//打开适配器
	if ((adhandle = pcap_open(
		d->name,           //设备名
		65536,
		PCAP_OPENFLAG_PROMISCUOUS,    //混杂模式
		1000,                         //读取超时
		NULL,
		errbuf                        //存储错误信息
	)) == NULL)
	{
		cout << "不能打开适配器,设备不被winpcap支持" << endl;

		pcap_freealldevs(alldevs);//释放设备列表
		return -1;
	}

	// 检查数据链路层，k看是否是以太网 
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		cout << "这个不工作在以太网范围" << endl;

		pcap_freealldevs(alldevs);  // 释放设备列表 
		return -1;
	}

	if (d->addresses != NULL) //获得接口第一个地址的掩码

		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else

		netmask = 0xffffff;  //如果接口没有地址，那么我们假设一个C类的掩码 

	cout << "请输入要过滤的包: ";
	cin >> packet_filter;


	//  编译过滤规则
	if (pcap_compile(adhandle, //适配器处理对象
		&fcode,
		packet_filter,  //过滤
		1,               //优化标志
		netmask        //子网掩码
	) < 0)
	{
		cout << "不能编译这个包" << endl;

		pcap_freealldevs(alldevs);
		return -1;
	}

	//设置过滤规则
	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		cout << "设置过滤规则错误" << endl;

		pcap_freealldevs(alldevs);
		return -1;
	}
	int packet_num=5;//设置抓的包的个数
	
	//cout << "请输入抓包个数:   ";
	//cin >> packet_num;

	pcap_loop(adhandle, packet_num, ethernet_packet_loop, NULL); //回调函数

	system("pause");
	return 0;
}




