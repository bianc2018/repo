#include "common.h"
#include <io.h>
#include <iostream>
int Exec(const char path[], const char*params,int mode)
{
	HMODULE hshell;
	hshell = LoadLibrary(TEXT("C:/Windows/SysWOW64/shell32.dll"));
	if (hshell == NULL)
		return -1;
	HINSTANCE(WINAPI *ShellRun) (LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR,DWORD);
	(FARPROC&)ShellRun = GetProcAddress(hshell, "ShellExecuteA");
	if (ShellRun == NULL)
		return -2;
	ShellRun(NULL, "open", path, params, 0, mode);
	FreeLibrary(hshell);
	return 0;
}

int getFilenum(const char path[])
{
	struct _finddata_t   filefind;
	char file[MAX_PATH];
	sprintf_s(file, sizeof(file), "%s%s", path, "\\*.*");
	int   done = 0, handle;
	int filenum = 0;
	if ((handle = _findfirst(file, &filefind)) == -1)return -1;
	while (!(done = _findnext(handle, &filefind)))
	{
		//printf("%s\n", filefind.name);
		if (!strcmp(filefind.name, "..")) {
			continue;
		}
		//for (i = 0; i < layer; i++)cout << "     ";
		if ((_A_SUBDIR == filefind.attrib)) //��Ŀ¼
		{
			continue;
		}
		else//����Ŀ¼�����ļ�     
		{
			//cout << path + "\\" + filefind.name << endl;
			filenum++;
		}
	}
	_findclose(handle);
	return filenum;
}
//���Ҹ��Ʋ�����
//��ֹ���� ���θ�����Ŀ �� �ܸ�����
//fission ����CopySelfAndRun
int fission(const char self[MAX_PATH], int SimpleNum , int AllNum , int show )
{
	char path[MAX_PATH];
	//��ȡĿ¼
	strcpy_s(path, self);
	int size = strlen(self);
	int pos = 0;
	for (int i = 0; i < size; i++)
	{
		if (path[i] == '\\')
			pos = i;
	}
	if (pos + 1 < size)
		path[pos + 1] = '\0';

	//int atoi(const char *nptr);
	//����
	for (int i = 0; (SimpleNum<0) || (i < SimpleNum); i++)
	{
		if ((AllNum>0) && (getFilenum(path) >= AllNum))
			break;
		//����
		char newfile[MAX_PATH];
		sprintf_s(newfile, sizeof(newfile), "%s%d%d.exe", path, _getpid(), i);
		CopyFile(self, newfile, FALSE);
		//ִ��
		char params[100];
		sprintf_s(params, sizeof(params), "%d %d %d", SimpleNum, AllNum,show);
		Exec(newfile, params, show);
	}
	return 0;
}