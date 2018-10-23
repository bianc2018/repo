#pragma once
#include "pch.h"
#include <fstream>
#define LERROR 0
#define LWARN  1
#define LINFO  2
#define LDEBUG 3
using namespace std;
vector<ULONG> getAddr(std::string host);

ULONG tranfer(string ip);

string tranfer(ULONG ip);

in_addr dtranfer(string ips);

in_addr dtranfer(ULONG ip);

int Log(fstream & _stdout, int level, const char * messsage);
