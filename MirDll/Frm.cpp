#include "Common.h"
#include <winuser.h>
#include "resource.h"
#include "MyHook.h"
#include "SendProgram.h"
#include "GameEvent.h"



INT_PTR MyWindowProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;

	if (uMsg == WM_INITDIALOG)
	{
		

		
		g_hMgDlg = hDlg;
		SetDlgItemText(hDlg, IDC_EDIT3, "4022");
		SetDlgItemText(hDlg, IDC_EDIT9, "0");
		
		ShowWindow(hDlg, SW_SHOW);
	}

	if (uMsg == WM_CLOSE)
	{
		UnHookRecv();
		UnHookSend();
		DestroyWindow(hDlg);
	}


	if (uMsg == WM_COMMAND)
	{
		if (HIWORD(wParam) == BN_CLICKED)
		{

			switch (LOWORD(wParam))
			{
				case IDC_HOOK_SEND:
				{
					
					HookSend();
				}
				break;
				case IDC_UNHOOK_SEND:
				{
					
					UnHookSend();
				}
				break;
				case IDC_HOOK_RECV:
				{
					HookRecv();
				}
				break;
				case IDC_UNHOOK_RECV:
				{
					UnHookRecv();
				}
				break;
				case IDC_CHUANSONG:
				{
	
					
					char shyperlink[0x100];
					GetDlgItemTextA(g_hMgDlg, IDC_EDIT3, shyperlink, 0x100);
					int hyperlink = stoi(shyperlink);

					chuansong(hyperlink);
				}
				break;
				case IDC_USESKILL:
				{
					char skillIDStr[0x100];
					GetDlgItemTextA(g_hMgDlg, IDC_EDIT1, skillIDStr, 0x100);

					char targetIDStr[0x100];
					GetDlgItemTextA(g_hMgDlg, IDC_EDIT2, targetIDStr, 0x100);

					if (skillIDStr[0] == '\0')
					{
						MessageBoxA(0, "请填写技能ID后再使用", "错误", 0);
						break;
					}

					if (targetIDStr[0] == '\0')
					{
						MessageBoxA(0, "请填写对象ID后再使用", "错误", 0);
						break;
					}

					int skillID = stoi(skillIDStr);
					char targetID[0x10];
					sprintf_s(targetID, "%s", targetIDStr);
					useskill(skillID, targetID);
				}
				break;
				case IDC_HEISHIBUY:
				{
					char dwMakeIndex[0x100];
					//GetDlgItemTextA(g_hMgDlg, IDC_EDIT3, dwMakeIndex, 0x100);

					if (dwMakeIndex[0] == '\0')
					{
						MessageBoxA(0, "请填写黑市ID后再使用", "错误", 0);
						break;
					}

					DWORD dwW1 = atoi(dwMakeIndex);
					BuyHeiShi(dwW1);
				}
				break;
				case IDC_PUSHBANK:
				{
					char sIsVip[0x100];
					GetDlgItemTextA(g_hMgDlg, IDC_EDIT9, sIsVip, 0x100);
					int isVip = atoi(sIsVip);

					char sItemID[0x100];
					GetDlgItemTextA(g_hMgDlg, IDC_EDIT5, sItemID, 0x100);
					if (sItemID[0] == '\0')
					{
						MessageBoxA(0, "请填写物品ID后再使用", "错误", 0);
						break;
					}


					char sName[0x100];
					GetDlgItemTextA(g_hMgDlg, IDC_EDIT7, sName, 0x100);
					int skillID = stoi(sItemID);

					if (sName[0] == '\0')
					{
						MessageBoxA(0, "请填写物品名字后再使用", "错误", 0);
						break;
					}


					PushBank(skillID, sName, isVip);
				}
				break;
				case IDC_PULLBANK:
				{
					char sIsVip[0x100];
					GetDlgItemTextA(g_hMgDlg, IDC_EDIT9, sIsVip, 0x100);
					int isVip = atoi(sIsVip);

					char sItemID[0x100];
					GetDlgItemTextA(g_hMgDlg, IDC_EDIT6, sItemID, 0x100);
					if (sItemID[0] == '\0')
					{
						MessageBoxA(0, "请填写物品ID后再使用", "错误", 0);
						break;
					}


					char sName[0x100];
					GetDlgItemTextA(g_hMgDlg, IDC_EDIT8, sName, 0x100);
					
					int skillID = stoi(sItemID);

					if (sName[0] == '\0')
					{
						MessageBoxA(0, "请填写物品名字后再使用", "错误", 0);
						break;
					}


					PullBank(skillID, sName, isVip);
				}
				break;
				case IDC_TEST:
				{
			
					//HWND i = GetDlgItem(hDlg, IDC_CHECK1);
					
				


					//LostItem(5248098, "回城石", 50);
					//OutBagInfo();
					//GameSocket = NULL;
				}
				break;
				case IDC_TEST2:
				{
					if (isSendOK)
						isSendOK = false;
					else
						isSendOK = true;

					DBG("SEND DBG:%d", isSendOK);
				}
				break;
				case IDC_TEST3:
				{
					if (isRecvOK)
						isRecvOK = false;
					else
						isRecvOK = true;

					DBG("RECV DBG:%d", isRecvOK);
				}
				break;
			}

		}
	}

	return  bRet;
}



void WINAPI MyDlgThread()
{

	/*while (true)
	{

	}*/
	DialogBoxA(g_hInst, MAKEINTRESOURCEA(IDD_DIALOG1), NULL, MyWindowProc);
	
	DBG("窗口关闭..");
	FreeLibraryAndExitThread(g_hInst, 0);
	DBG("窗口关闭..");
}
