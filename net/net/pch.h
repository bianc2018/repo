#ifndef PCH_H
#define PCH_H
//the macro HAVE_REMOTE must define before
#ifndef  HAVE_REMOTE
#define HAVE_REMOTE
#endif
#ifndef WPCAP
#define WPCAP
#endif
#include <winsock2.h> 
#include <Ws2tcpip.h>
#include <string>
#include <vector>
#include <thread>
#include <iostream>
#include <pcap.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "packet.lib")
#pragma comment(lib, "wpcap.lib")

// TODO: 添加要在此处预编译的标头

#endif //PCH_H
