#include "windows.h"
#include "Common.h"
#include "Frm.h"
#include "MyHook.h"



BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	{

		//MessageBoxA(0, "ע��ɹ�!", "��ʾ",0);
		g_hInst = hModule;

		DisableThreadLibraryCalls(hModule);
		g_dwGameBase = (DWORD64)GetModuleHandleA(NULL);
		DBG("��ʼ");
		InitHook();
		HookAll();

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MyDlgThread, NULL, NULL, NULL);


		DBG("���");
		return true;

	}
	break;
	case DLL_PROCESS_DETACH:
	{
		//UnHook();
		
		DBG("ģ����ж��!");

		return TRUE;
	}
	break;
	}

	return FALSE;
}
