
#include "MyHook.h"
#include "xPacket.h"
#include "CriticalSection.h"
#include "zconf.h"
#include "zlib.h"
#include "GameEvent.h"
#include "MinHook/include/MinHook.h"
#include "json/writer.h"
#include "Encode.h"
#include "Decode.h"

#pragma comment(lib,"ws2_32.lib")




CCriticalSection g_lock;
xPacket g_Data(0x80000);
HMODULE ws2_32_dll = GetModuleHandleA("ws2_32.dll");
typedef int(__stdcall* _recv)(SOCKET s, char* buf, int len, int flags);
typedef int(__stdcall* _send)(SOCKET s, const char* buf, int len, int flags);
typedef int(__stdcall* _connect)(_In_ SOCKET s, _In_reads_bytes_(namelen) const struct sockaddr_in FAR* name, _In_ int namelen);
_connect Old_connect;
_send Old_send;
_recv Old_recv;
_recv myrecv = (_recv)GetProcAddress(ws2_32_dll, "recv");
_send mysend = (_send)GetProcAddress(ws2_32_dll, "send");
_connect myconnect = (_connect)GetProcAddress(ws2_32_dll, "connect");



void ProcessPack(xPacket& pack)
{

	while (TRUE)
	{

		if (pack.getsize() < 0x30)
		{
			//DBG("长度不够");
			break;
		}

		unsigned int v13 = 0;
		BYTE gobyOutData[0x200] = { 0 };
		
		Decode6BitBuf((unsigned __int8*)g_Data.getbuf(), (char*)gobyOutData, 0x30, &v13);
		MYPACK_DATA* pData = (MYPACK_DATA*)(gobyOutData);


		//DBG("剩余包长度:%08X --> %08X", pack.getsize(), pData->packSize);
		if (pack.getsize() < (pData->packSize + 0x30))
		{
			//DBG("缓冲区长度不足");
			break;
		}

		BYTE byHeadData[0x30] = { 0 };
		BYTE* pAllocData = new BYTE[pData->packSize];
		pack.pop(byHeadData, sizeof(byHeadData));
		pack.pop(pAllocData, pData->packSize);

		if (pData->bZIp)
		{
			int decSize = 0;
			ULONG zipSize = 0xAF000;
			BYTE* pZipData = new BYTE[0xAF000];
			uncompress(pZipData, &zipSize, pAllocData, pData->packSize);
			BYTE* pDecData = new BYTE[zipSize];
			Decode6BitBuf((unsigned __int8*)pZipData, (char*)pDecData, zipSize, &v13);
			string outStr = readString((DWORD64)pDecData);
			GameEvent(pData, outStr);
			//if (FilterPack(pData->dwHead))
			{
				if (isRecvOK)
				{
					DBG("****收包(ZIP)****包头:%08X ZIP:%d 长度:%08X", pData->dwHead, pData->bZIp, pData->packSize);

					DBG("DEC-------------------------------");
					DBGHex((unsigned char*)pDecData, pData->packSize);
					DBG("%s", outStr.c_str());
				}

			}
			delete[]pZipData;
			delete[]pDecData;
		}
		else
		{

			BYTE* pDecData = new BYTE[pData->packSize];
			//DBG("999999999999999999 pData->packSize:%08X   head:%08X", pData->packSize, pData->dwHead);
			Decode6BitBuf((unsigned __int8*)pAllocData, (char*)pDecData, pData->packSize, &v13);
			string outStr = readString((DWORD64)pDecData);
			GameEvent(pData, outStr);

			//if (FilterPack(pData->dwHead))
			{
				if (isRecvOK)
				{
					DBG("****收包****包头:%08X ZIP:%d 长度:%08X", pData->dwHead, pData->bZIp, pData->packSize);
					DBG("DEC-------------------------------");


					DBGHex((unsigned char*)pData, 0x30);
					DBG("%s", outStr.c_str());
				}


			}

			delete[]pDecData;
		}

		delete[]pAllocData;



	}

}


int __stdcall Myrecv(SOCKET s, char* buf, int len, int flags)
{

	int nt = Old_recv(s, buf, len, flags);

	/*if (nt <= 0)
	{
		return nt;
	}
	if (nt < 0x30)
	{
		return nt;
	}
*/

	if (GameSocket != NULL && s == GameSocket)
	{
		g_lock.Lock();
		g_Data.push(buf, nt);
		ProcessPack(g_Data);
		g_lock.UnLock();
	}

	return nt;
}



int __stdcall Mysend(SOCKET s, const char* buf, int len, int flags) 
{

	if (GameSocket == NULL)
	{
		unsigned int v13 = 0;
		BYTE byOutData[0xFFFF] = { 0 };
		Decode6BitBuf((unsigned __int8*)buf, (char*)byOutData, 0x30, &v13);

		int nlen = ReadWord((DWORD64)byOutData + 0x1C);
		int nNum = ReadWord((DWORD64)byOutData + 0x20);
		DWORD header = ReadDword((DWORD64)byOutData + 0x4);
		if (/*header == 0xBC3 ||*/ header == 0x14)
		{
			g_GameNum = nNum;
			GameSocket = s;
			DBG("已通过游戏心跳包得到SOCKET!");

		}
	}
	if (GameSocket != NULL && s == GameSocket)
	{
		unsigned int v13 = 0;
		BYTE byOutData[0xFFFF] = { 0 };
		Decode6BitBuf((unsigned __int8*)buf, (char*)byOutData, 0x30, &v13);

		MYPACK_DATA* pData = (MYPACK_DATA*)byOutData;

		int nlen = ReadWord((DWORD64)byOutData + 0x1C);

		DWORD header = ReadDword((DWORD64)byOutData + 0x4);


		if (pData->num > 0)
		{
			g_GameNum = pData->num;
		}



		memcpy(byOutData + 0x24, buf + 0x30, len - 0x30);


		string str = readString((DWORD64)byOutData + 0x30);

		GameEvent(pData, str);
		if (isSendOK)
		{
			
			DBG("**SEND** socket: %02X - head:%08X - len: %d(%04X)   flags:%d", s, header, len, len, flags);
			DBGHex(byOutData, len);
			BYTE* UTF8Str = new BYTE[nlen];
			memcpy(UTF8Str, byOutData + 0x24, nlen);
			string sUTF8Str = readString((DWORD64)UTF8Str);
			DBG("---------------DEC----------------");
			DBG("%s", sUTF8Str.c_str());
		
		}


		BYTE newBuf[0xFFFF] = { 0 };
		memcpy(newBuf, buf, len);
		BYTE MyDeData[0x24] = { 0 };
		memcpy(MyDeData, byOutData, 0x24);
		*(DWORD*)(MyDeData + 0x20) = pData->num + g_MyNum;
		BYTE MyEnData[0x30] = { 0 };
		Encode6BitBuf(MyDeData, (char*)MyEnData, 0x24, &v13);

		memcpy(newBuf, MyEnData, 0x30);

		int nt = Old_send(s, (char*)newBuf, len, flags);
		return nt;

	}


	int nt = Old_send(s, (char*)buf, len, flags);

	return nt;
}



int __stdcall Myconnect(_In_ SOCKET s, _In_reads_bytes_(namelen) const struct sockaddr_in FAR* name, _In_ int namelen) {
	int nt = Old_connect(s, name, namelen);

	return nt;

	int a1 = ReadByte((DWORD64)name + 3);
	int a2 = ReadByte((DWORD64)name + 4);
	int a3 = ReadByte((DWORD64)name + 5);
	int a4 = ReadByte((DWORD64)name + 6);

	int port = ReadWord((DWORD64)name + 2);

	SOCKADDR_IN   sockAddr;
	int   iLen = sizeof(sockAddr);



	getpeername(s, (struct   sockaddr*)&sockAddr, &iLen);

	char* strAddr = inet_ntoa(sockAddr.sin_addr);//IP 
	int  uIPPort = sockAddr.sin_port;
	DBG("IP:%s PORT:%d", strAddr, uIPPort);

	return nt;
	
}


void InitHook()
{
	MH_Initialize();

	if (MH_CreateHookApi(L"ws2_32.dll", "recv", &Myrecv, reinterpret_cast<void**>(&Old_recv)) != MH_OK)
	{
		DBG("失败recv");
		return;
	}

	if (MH_CreateHookApi(L"ws2_32.dll", "send", &Mysend, reinterpret_cast<void**>(&Old_send)) != MH_OK)
	{
		DBG("失败send");
		return;
	}

	if (MH_CreateHookApi(L"ws2_32.dll", "connect", &Myconnect, reinterpret_cast<void**>(&Old_connect)) != MH_OK)
	{
		DBG("失败send");
		return;
	}

}



void HookAll()
{
	HookSend();
	HookRecv();
}

void HookSend()
{
	DBG("HOOK SEND!");
	MH_STATUS st;


	if ((st = MH_EnableHook(mysend)) != MH_OK)
	{
		DBG("enable失败");
	}

}
void HookRecv()
{
	DBG("HOOK RECV!");
	MH_STATUS st;

	if ((st = MH_EnableHook(myrecv)) != MH_OK)
	{
		DBG("enable失败");
	}
}



void UnHookSend()
{
	DBG("关闭HOOK SEND");
	MH_DisableHook(mysend);

}

void UnHookRecv()
{
	DBG("关闭HOOK RECV");
	MH_DisableHook(myrecv);

}





void __stdcall chuansong(DWORD dwMapID)
{
	g_MyNum = g_MyNum + 1;

	MYPACK_DATA mDeHead;
	mDeHead.w1 = dwMapID;
	mDeHead.w2 = 0;
	mDeHead.dwHead = 0x1499;
	mDeHead.p1 = 0;
	mDeHead.p2 = 0;
	mDeHead.p3 = 0;
	mDeHead.p4 = 0;
	mDeHead.bZIp = 0;
	mDeHead.packSize = 0;
	mDeHead.num = g_GameNum + g_MyNum;



	BYTE enDataOut[0x30] = { 0 };
	BYTE deHeadData[0x24] = { 0 };
	memcpy(deHeadData, &mDeHead, 0x24);

	unsigned int v13 = 0;
	Encode6BitBuf((unsigned __int8*)deHeadData, (char*)enDataOut, 0x24, &v13);


	DBG("{当前执行游戏传送功能,传送地图ID:[%04X]}当前游戏计数器:%08X  我的计数器:%08X", mDeHead.w1, g_GameNum, g_MyNum);
	int nt = Old_send(GameSocket, (char*)enDataOut, 0x30, 0);
}

void __stdcall BuyHeiShi(DWORD dwMakeIndex)
{

	

	g_MyNum = g_MyNum + 1;

	MYPACK_DATA2 mDeHead;
	mDeHead.w1 = dwMakeIndex;
	mDeHead.dwHead = 0x493;
	mDeHead.p1 = 0;
	mDeHead.p2 = 0;
	mDeHead.p3 = 0;
	mDeHead.p4 = 0;
	mDeHead.bZIp = 0;
	mDeHead.packSize = 0;
	mDeHead.num = g_GameNum + g_MyNum;


	BYTE enDataOut[0x30] = { 0 };
	BYTE deHeadData[0x24] = { 0 };
	memcpy(deHeadData, &mDeHead, 0x24);

	unsigned int v13 = 0;
	Encode6BitBuf((unsigned __int8*)deHeadData, (char*)enDataOut, 0x24, &v13);


	DBG("{当前执行黑市购买功能,购买ID:[%04X]}当前游戏计数器:%08X  我的计数器:%08X", mDeHead.w1, g_GameNum, g_MyNum);
	int nt = Old_send(GameSocket, (char*)enDataOut, 0x30, 0);
}



void __stdcall PushBank(DWORD dwMakeIndex,char* sItemName,bool isVip)
{
	DWORD dwUserID = 0;
	if (isVip)
	{
		dwUserID = 0x98967F;
	}
	else
	{
		dwUserID = GetBankNpcID();
	}
	if (dwUserID == 0)
	{
		return;
	}
	string UTF8 = WStringToUTF8(MBytesToWString(sItemName).c_str());
	DBGHex((unsigned char*)UTF8.data(), UTF8.length());

	g_MyNum = g_MyNum + 1;

	MYPACK_DATA2 mDeHead;
	mDeHead.w1 = dwUserID;
	mDeHead.dwHead = 0x407;
	mDeHead.p1 = dwMakeIndex;
	mDeHead.p2 = 1;
	mDeHead.p3 = 0;
	mDeHead.p4 = 0;
	mDeHead.bZIp = 0;
	mDeHead.packSize = UTF8.length();
	mDeHead.num = g_GameNum + g_MyNum;


	BYTE enDataOut[0x30] = { 0 };
	BYTE deHeadData[0x24] = { 0 };
	memcpy(deHeadData, &mDeHead, 0x24);
	BYTE *StrData = new BYTE[UTF8.length()];
	DBGHex(deHeadData, 0x24);
	unsigned int v13 = 0;
	Encode6BitBuf((unsigned __int8*)deHeadData, (char*)enDataOut, 0x24, &v13);
	
	int DataLen = 0x30 + UTF8.length();
	BYTE* SendData = new BYTE[DataLen];
	memcpy(SendData, enDataOut, 0x30);
	memcpy(SendData + 0x30, UTF8.data(), UTF8.length());
	DBGHex(SendData, DataLen);

	DBG("{当前执行存仓功能,ID:[%08X]--[%s]}当前游戏计数器:%08X  我的计数器:%08X", mDeHead, sItemName,g_GameNum, g_MyNum);
	int nt = Old_send(GameSocket, (char*)SendData, DataLen, 0);
}



void __stdcall PullBank(DWORD dwMakeIndex, char* sItemName, bool isVip)
{
	DWORD dwUserID = 0;
	if (isVip)
	{
		dwUserID = 0x98967F;
	}
	else
	{
		dwUserID = GetBankNpcID();
	}
	if (dwUserID == 0)
	{
		return;
	}
	string UTF8 = WStringToUTF8(MBytesToWString(sItemName).c_str());
	DBGHex((unsigned char*)UTF8.data(), UTF8.length());

	g_MyNum = g_MyNum + 1;

	MYPACK_DATA2 mDeHead;
	mDeHead.w1 = dwUserID;
	mDeHead.dwHead = 0x408;
	mDeHead.p1 = dwMakeIndex;
	mDeHead.p2 = 1;
	mDeHead.p3 = 0;
	mDeHead.p4 = 0;
	mDeHead.bZIp = 0;
	mDeHead.packSize = UTF8.length();
	mDeHead.num = g_GameNum + g_MyNum;


	BYTE enDataOut[0x30] = { 0 };
	BYTE deHeadData[0x24] = { 0 };
	memcpy(deHeadData, &mDeHead, 0x24);
	BYTE* StrData = new BYTE[UTF8.length()];
	DBGHex(deHeadData, 0x24);
	unsigned int v13 = 0;
	Encode6BitBuf((unsigned __int8*)deHeadData, (char*)enDataOut, 0x24, &v13);

	int DataLen = 0x30 + UTF8.length();
	BYTE* SendData = new BYTE[DataLen];
	memcpy(SendData, enDataOut, 0x30);
	memcpy(SendData + 0x30, UTF8.data(), UTF8.length());
	DBGHex(SendData, DataLen);

	DBG("{当前执行取仓功能,ID:[%08X]--[%s]}当前游戏计数器:%08X  我的计数器:%08X", mDeHead, sItemName, g_GameNum, g_MyNum);
	int nt = Old_send(GameSocket, (char*)SendData, DataLen, 0);
}


void __stdcall LostItem(DWORD dwMakeIndex, char* sItemName,int num)
{
	
	string UTF8 = WStringToUTF8(MBytesToWString(sItemName).c_str());
	DBGHex((unsigned char*)UTF8.data(), UTF8.length());

	g_MyNum = g_MyNum + 1;

	MYPACK_DATA2 mDeHead;
	mDeHead.w1 = dwMakeIndex;
	mDeHead.dwHead = 0x3E8;
	mDeHead.p1 = num;
	mDeHead.p2 = 0;
	mDeHead.p3 = 0;
	mDeHead.p4 = 0;
	mDeHead.bZIp = 0;
	mDeHead.packSize = UTF8.length();
	mDeHead.num = g_GameNum + g_MyNum;


	BYTE enDataOut[0x30] = { 0 };
	BYTE deHeadData[0x24] = { 0 };
	memcpy(deHeadData, &mDeHead, 0x24);
	BYTE* StrData = new BYTE[UTF8.length()];
	DBGHex(deHeadData, 0x24);
	unsigned int v13 = 0;
	Encode6BitBuf((unsigned __int8*)deHeadData, (char*)enDataOut, 0x24, &v13);

	int DataLen = 0x30 + UTF8.length();
	BYTE* SendData = new BYTE[DataLen];
	memcpy(SendData, enDataOut, 0x30);
	memcpy(SendData + 0x30, UTF8.data(), UTF8.length());
	DBGHex(SendData, DataLen);

	DBG("{当前执行丢弃功能,ID:[%08X]--[%s]}当前游戏计数器:%08X  我的计数器:%08X", mDeHead, sItemName, g_GameNum, g_MyNum);
	int nt = Old_send(GameSocket, (char*)SendData, DataLen, 0);
}


void __stdcall useskill(int skill_id,char target_id[0x10])
{

	Json::Value useSkill;

	useSkill["srcX"] = 86;
	useSkill["srcY"] = 40;
	useSkill["destX"] = 79;
	useSkill["destY"] = 37;
	useSkill["dir"] = 4;
	useSkill["targetID"] = target_id;
	useSkill["time"] = getUnixTime();
	useSkill["skillID"] = skill_id;


	string str = get_string(useSkill.toStyledString());
	int len = str.length();
	DBG("%s  len:%d", str.c_str(), str.length());



	g_MyNum = g_MyNum + 1;
	MYPACK_DATA mDeHead;
	mDeHead.w1 = 0;
	mDeHead.w2 = 0;
	mDeHead.dwHead = 0x0BC9;
	mDeHead.p1 = 0;
	mDeHead.p2 = 0;
	mDeHead.p3 = 0;
	mDeHead.p4 = 0;
	mDeHead.bZIp = 0;
	mDeHead.packSize = len;
	mDeHead.num = g_GameNum + g_MyNum;


	BYTE enDataOut[0x30] = { 0 };
	BYTE deHeadData[0x24] = { 0 };
	memcpy(deHeadData, &mDeHead, 0x24);

	unsigned int v13 = 0;
	Encode6BitBuf((unsigned __int8*)deHeadData, (char*)enDataOut, 0x24, &v13);

	MYENCODE_DATA mEnData;
	memcpy(mEnData.enHead, enDataOut, sizeof(enDataOut));





	strcpy_s(mEnData.json, str.c_str());

	BYTE sendData[0x1024] = { 0 };
	memcpy(sendData, &mEnData, (0x30 + len));


	//const char *pp = str.c_str();
	/*for (int i = 0 ; i < str.length() ; i ++)
		bbb[i] = str[i];*/

		//DBG("%s", str.c_str());
		//DBGHex((unsigned char*)sendData, (0x30 + len));

	DBG("{当前准备释放技能}当前游戏计数器:%08X  我的计数器:%08X", g_GameNum, g_MyNum);
	int nt = Old_send(GameSocket, (char*)sendData, (0x30 + len), 0);

}
