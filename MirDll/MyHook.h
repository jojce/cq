#pragma once
#include "Common.h"
#include <windows.h>

#ifdef __cplusplus

#define EXPORT extern "C" __declspec (dllexport)

#else

#define EXPORT __declspec (dllexport)

#endif

#ifdef UNICODE

//��������UNICODE���͵ĺ��� 

//#define EdrCenterText EdrCenterTextW

#else

//��������ASSIC���͵ĺ��� 

//#define EdrCenterText EdrCenterTextA

EXPORT void _stdcall chuansong(DWORD dwMapID);		//����
EXPORT void _stdcall BuyHeiShi(DWORD dwMakeIndex);	//�����
EXPORT void _stdcall PushBank(DWORD dwMakeIndex, char* sItemName, bool isVip);	//��
EXPORT void _stdcall PullBank(DWORD dwMakeIndex, char* sItemName, bool isVip);	//ȡ
EXPORT void _stdcall LostItem(DWORD dwMakeIndex, char* sItemName, int num);		//����
EXPORT void _stdcall useskill(int skill_id, char target_id[0x10]);

#endif


typedef struct
{
	BYTE enHead[0x30];
	char json[0x1024];

}MYENCODE_DATA, * PMYENCODE_DATA;

void InitHook();
void HookAll();
void HookSend();
void HookRecv();
void UnHookSend();
void UnHookRecv();

