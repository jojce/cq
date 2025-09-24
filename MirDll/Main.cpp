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

		//MessageBoxA(0, "注入成功!", "提示",0);
		g_hInst = hModule;

		DisableThreadLibraryCalls(hModule);
		g_dwGameBase = (DWORD64)GetModuleHandleA(NULL);
		DBG("开始");
		InitHook();
		HookAll();

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MyDlgThread, NULL, NULL, NULL);


		DBG("完成");
		return true;

	}
	break;
	case DLL_PROCESS_DETACH:
	{
		//UnHook();
		
		DBG("模块已卸载!");

		return TRUE;
	}
	break;
	}

	return FALSE;
}
