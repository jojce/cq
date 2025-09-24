#pragma once
#include <intsafe.h>
#include <string>
#include "Common.h"
#include <vector>



using namespace std;



typedef struct 
{
	char Name[0x100];
	char UserID[0x100];
	char MasterID[0x100];
	char Map[0x100];
	int x;
	int y;

}PLAY_INFO, * PPLAY_INFO;


typedef struct
{
	char Name[0x100];
	char UserID[0x100];
	char x[0x100];
	char y[0x100];
	char type[0x100];
	int index = 0;
}MONSTER, * PMONSTER;


typedef std::vector<MONSTER> MONSTER_LIST;
typedef MONSTER_LIST::iterator MONSTER_LIST_IT;


typedef struct
{
	char Src[0x100];
	char Index[0x100];
	char MakeIndex[0x100];
	char OverLap[0x100];
	char price[0x100];
}HEISHI, * PHEISHI;


typedef std::vector<HEISHI> HEISHI_LIST;
typedef HEISHI_LIST::iterator HEISHI_LIST_IT;


typedef struct
{
	char Src[0x100];
	char Index[0x100];
	char MakeIndex[0x100];
	char OverLap[0x100];
	char Durm[0x100];

}BAGINFO, * PBAGINFO;
typedef std::vector<BAGINFO> BAGINFO_LIST;
typedef BAGINFO_LIST::iterator BAGINFO_LIST_IT;




#ifdef __cplusplus

#define EXPORT extern "C" __declspec (dllexport)

#else

#define EXPORT __declspec (dllexport)

#endif

#ifdef UNICODE

//这里声明UNICODE类型的函数 

//#define EdrCenterText EdrCenterTextW

#else

EXPORT MONSTER_LIST* __stdcall GetMonList();	//周围列表
EXPORT BAGINFO_LIST* __stdcall GetBagList();	//背包信息
EXPORT BAGINFO_LIST* __stdcall GetBankList();//仓库信息
EXPORT HEISHI_LIST* __stdcall GetHeishiList();	//黑市信息
EXPORT PLAY_INFO* __stdcall GetPlayInfo();	//人物信息
//这里声明ASSIC类型的函数 

//#define EdrCenterText EdrCenterTextA


#endif






void GetRoleLocation(MYPACK_DATA* pData, string strJson);
void GetRoleMap(MYPACK_DATA* pData, string strJson);
void GetRoleInfo(string strJson);





void GameEvent(MYPACK_DATA* pData, string strJson);

void GetRoleUserID(MYPACK_DATA* pData, string strJson);
void OutBagInfo();
DWORD GetBankNpcID();
void GetHeiShiInfo(MYPACK_DATA* pData, string strJson);
void AddMonster(MYPACK_DATA* pData, string strJson);
void DelMonster(MYPACK_DATA* pData, string strJson);