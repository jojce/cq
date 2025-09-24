#pragma once
#include <windows.h>
#include <fstream>
#include <cstring>
#include <map>
#include "xPacket.h"
using namespace std;

extern DWORD64 g_dwGameBase;
extern SOCKET GameSocket;

extern int g_GameNum ;
extern int g_MyNum ;
extern bool isSendOK;
extern bool isRecvOK;



#define _WORD WORD
#define _BYTE BYTE
#define LOBYTE(x)   (*((_BYTE*)&(x)))   // low byte
#define LOWORD(x)   (*((_WORD*)&(x)))   // low word
#define LODWORD(x)  (*((DWORD*)&(x)))  // low dword
#define HIBYTE(x)   (*((_BYTE*)&(x)+1))
#define HIWORD(x)   (*((_WORD*)&(x)+1))
#define HIDWORD(x)  (*((DWORD*)&(x)+1))

typedef          __int64 ll;
typedef unsigned __int64 ull;
typedef          char   int8;
typedef   signed char   sint8;
typedef unsigned char   uint8;
typedef          short  int16;
typedef   signed short  sint16;
typedef unsigned short  uint16;
typedef          int    int32;
typedef   signed int    sint32;
typedef unsigned int    uint32;
typedef ll              int64;
typedef ll              sint64;

extern HINSTANCE g_hInst;

typedef struct
{
	char szCN[0x100];
	char szEN[0x100];
	char szTexture[0x100];
}ITEM_NAME, *PITEM_NAME;

extern std::map<int, ITEM_NAME> g_ItemHash;

template<class T>


#define M_PI		3.14159265358979323846f
#define M_RADPI		57.295779513082f
#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.
#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / M_PI_F) )
#define DEG2RAD( x )  ( (float)(x) * (float)(M_PI_F / 180.f) )

T read(const DWORD64 dwPtr)
{
	if (!IsBadReadPtr((void*)dwPtr, sizeof(T)))
		return *(T*)dwPtr;
	return 0;
}

void readMem(DWORD64 dwPtr, void *pData, int size);


#define ReadDword64(X) read<DWORD64>(X)
#define ReadInt(X) read<UINT32>(X)
#define ReadByte(X) read<BYTE>(X)
#define ReadBool(X) read<bool>(X)
#define ReadWord(X) read<WORD>(X)
#define ReadDword(X) read<DWORD>(X)
#define ReadFloat(X) read<FLOAT>(X)
#define ReadDouble(X) read<DOUBLE>(X)
extern HINSTANCE g_hInst;
extern HWND g_hMgDlg;
extern SOCKET GameSocket;

namespace global
{
	
	extern DWORD64 g_hGameImage;
}


extern HWND g_hGameWnd;

#pragma pack(2)
typedef struct
{
	WORD w1;			//0
	WORD w2;			//2
	DWORD dwHead;		//4
	DWORD p1;			//8
	DWORD p2;			//C
	DWORD p3;			//10
	DWORD p4;			//14
	BOOL bZIp;			//18
	int packSize;		//1c
	int num;			//20

}MYPACK_DATA, * PMYPACK_DATA;
#pragma pack()

#pragma pack(2)
typedef struct
{
	DWORD w1;			//0
	DWORD dwHead;		//4
	DWORD p1;			//8
	DWORD p2;			//C
	DWORD p3;			//10
	DWORD p4;			//14
	BOOL bZIp;			//18
	int packSize;		//1c
	int num;			//20

}MYPACK_DATA2, * PMYPACK_DATA2;
#pragma pack()

wstring MBytesToWString(const char* lpcszString);
string WStringToMBytes(const wchar_t* lpwcszWString);
wstring UTF8ToWString(const char* lpcszString);
string WStringToUTF8(const wchar_t* lpwcszWString);
void dump_hex(const uint8_t* ptr, uint16_t buflen);
string readString(const DWORD64 dwPtr);
string get_string(string res);
int getUnixTime();
char stringTochar(string str);
void DBG(const char* format, ...);
void DBGW(const wchar_t* format, ...);
void DBGHex(unsigned char* pData, int size);
bool FilterPack(DWORD dwHeadID);
bool FilterPack2(DWORD dwHeadID);