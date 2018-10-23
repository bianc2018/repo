#pragma once
#include<windows.h>
#include <process.h>
#ifndef COMMON_H
#define COMMON_H
int Exec(const char path[], const char*params=NULL,int mode= SW_HIDE);
int getFilenum(const char path[]);
int fission(const char self[MAX_PATH], int SimpleNum=-1, int AllNum=-1, int show=SW_HIDE);
#endif