#include "GameEvent.h"
#include "json/json.h"
#include "resource.h"
#include <list>
#include <stdlib.h>





 char g_UserID[0x100] = { 0 };
const char* g_MasterId = NULL;
const char* g_player_name = NULL;







static PLAY_INFO play_info;



static MONSTER_LIST m_monlist;
static HEISHI_LIST hs_list;
static BAGINFO_LIST m_baglist;
static BAGINFO_LIST m_banklist;






MONSTER_LIST* __stdcall GetMonList()
{
	return &m_monlist;
}

BAGINFO_LIST* __stdcall  GetBagList()
{
	return &m_baglist;
}

BAGINFO_LIST* __stdcall GetBankList()
{
	
	return &m_banklist;
}

HEISHI_LIST* __stdcall GetHeishiList()
{
	return &hs_list;
}

PLAY_INFO* __stdcall GetPlayInfo()
{
	return &play_info;
	
}



void GetRoleUserID(MYPACK_DATA* pData, string strJson) 
{
	string out = get_string(strJson);
	/*DBG("-------------------------------------");
	DBG("%s", out.c_str());
	DBG("-------------------------------------");*/
	Json::Reader reader;
	Json::Value json_object;
	if (reader.parse(out.c_str(), json_object))
	{
		DBG("这里获取到userid了!!!!!!!!");
		string UserID = json_object["UserID"].asString();

		
		strcpy(g_UserID, UserID.c_str());
		strcpy(play_info.UserID, UserID.c_str());

		DBG("111111111%s", g_UserID);
	}


}


void OutBagInfo()
{
	return;
	BAGINFO_LIST_IT it;
	
	HWND hwnd = ::GetDlgItem(g_hMgDlg, IDC_LIST3);
	::SendMessage(hwnd, LB_RESETCONTENT, 0, (LPARAM)NULL);
	for (it = m_baglist.begin(); it != m_baglist.end(); it++)
	{

		char sOut[0x100] = { 0 };
		sprintf(sOut, "src:%s  index:%s  makeindex:%s OverLap:%s ", it->Src,it->Index, it->MakeIndex, it->OverLap);
		::SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)sOut); 
	}

}

void OutBankInfo()
{
	return;
	BAGINFO_LIST_IT it;

	HWND hwnd = ::GetDlgItem(g_hMgDlg, IDC_LIST4);
	::SendMessage(hwnd, LB_RESETCONTENT, 0, (LPARAM)NULL);
	for (it = m_banklist.begin(); it != m_banklist.end(); it++)
	{
		char sOut[0x100] = { 0 };
		sprintf(sOut, "src:%s  index:%s  makeindex:%s OverLap:%s ",it->Src, it->Index, it->MakeIndex, it->OverLap);
		::SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)sOut);
	}

}



void FindAndDelItemListAndPushBank(char* MakeIndex)
{
	BAGINFO_LIST_IT it;

	for (it = m_baglist.begin(); it != m_baglist.end(); it++)
	{
		if (strstr(MakeIndex, it->MakeIndex))
		{
			BAGINFO b;
			strcpy(b.Src, it->Src);
			strcpy(b.Index, it->Index);
			strcpy(b.MakeIndex, it->MakeIndex);
			strcpy(b.OverLap, it->OverLap);
			strcpy(b.Durm, it->Durm);
			m_banklist.push_back(b);
			m_baglist.erase(it);
			return;
		}
	}
	//DBG("没有");
}

void FindAndDelItemList(char* MakeIndex)
{
	BAGINFO_LIST_IT it;
	for (it = m_baglist.begin(); it != m_baglist.end(); it++)
	{
		if (strstr(MakeIndex, it->MakeIndex))
		{
			m_baglist.erase(it);
			return;
		}
	}
}

void FindAndDelBankItemList(char* MakeIndex)
{
	BAGINFO_LIST_IT it;
	for (it = m_banklist.begin(); it != m_banklist.end(); it++)
	{
		if (strstr(MakeIndex, it->MakeIndex))
		{
		
			m_banklist.erase(it);
			return;
		}
	}
}

void FindAndUpdataItemList(char* MakeIndex,char* sOverLap,char* sDura)
{
	BAGINFO_LIST_IT it;
	for (it = m_baglist.begin(); it != m_baglist.end(); it++)
	{
		if (strstr(MakeIndex, it->MakeIndex))
		{
			int Dura = atoi(sDura);
			if (Dura > 0)
			{
				strcpy(it->OverLap, sDura);
			}
			if (Dura == 0)
			{
				strcpy(it->OverLap, sOverLap);
			}
			return;
		}
	}
}


void DelItemByPushBank(MYPACK_DATA* pData, string strJson)
{
	DBG("存物品删除");
	MYPACK_DATA2* pData2 = (MYPACK_DATA2*)pData;
	int MakeIndex = pData2->w1;
	char sMakeIndex[0x100] = { 0 };
	_itoa(MakeIndex, sMakeIndex, 10);

	FindAndDelItemListAndPushBank(sMakeIndex);
	OutBagInfo();
	OutBankInfo();

}

void DelItemByPullBank(MYPACK_DATA* pData, string strJson)
{
	DBG("取物品删除");
	MYPACK_DATA2* pData2 = (MYPACK_DATA2*)pData;
	int MakeIndex = pData2->w1;
	char sMakeIndex[0x100] = { 0 };
	_itoa(MakeIndex, sMakeIndex, 10);
	FindAndDelBankItemList(sMakeIndex);
	OutBankInfo();
	OutBagInfo();
}


DWORD GetBankNpcID()
{
	MONSTER m;
	MONSTER_LIST_IT it;
	for (it = m_monlist.begin(); it != m_monlist.end();it ++)
	{
		if (strstr(it->Name, "仓库管理员")) 
		{
			DWORD dwUserID = atoi(it->UserID);
			return dwUserID;
		}
	}
	return 0;
}
void AddBagItem(MYPACK_DATA* pData, string strJson)
{
	if (pData->w1 == 0xFFFF && pData->w2 == 0xFFFF)
	{
		string out = get_string(strJson);
		Json::Reader reader;
		Json::Value json_object;
		if (reader.parse(out.c_str(), json_object))
		{
			string Src = json_object["Src"].asString();
			int Index = json_object["Index"].asInt();
			int MakeIndex = json_object["MakeIndex"].asInt();
			int OverLap = json_object["OverLap"].asInt();
			int Dura = json_object["Dura"].asInt();

			char sIndex[0x100] = { 0 };
			char sMakeIndex[0x100] = { 0 };
			char sOverLap[0x100] = { 0 };
			char sDura[0x100];
			char sSrc[0x100];
			strcpy_s(sSrc, Src.data());

			_itoa(Index, sIndex, 10);
			_itoa(MakeIndex, sMakeIndex, 10);
			_itoa(OverLap, sOverLap, 10);
			_itoa(Dura, sDura, 10);


			BAGINFO_LIST_IT it;
			for (it = m_baglist.begin(); it != m_baglist.end(); it++)
			{
				if (strstr(sMakeIndex, it->MakeIndex))
				{
					return;
				}
			}

			BAGINFO b;
			strcpy(b.Src, sSrc);
			strcpy(b.Index, sIndex);
			strcpy(b.MakeIndex, sMakeIndex);
			if (Dura > 0)
			{
				strcpy(b.OverLap, sDura);
			}
			if (Dura == 0)
			{
				strcpy(b.OverLap, sOverLap);
			}
			m_baglist.push_back(b);
			OutBagInfo();
		}
	}
}

void UpdataBagItem(MYPACK_DATA* pData, string strJson)
{

	DBG("更新数量!!");
	string out = get_string(strJson);
	Json::Reader reader;
	Json::Value json_object;
	if (reader.parse(out.c_str(), json_object))
	{


		int MakeIndex = json_object["MakeIndex"].asInt();
		int OverLap = json_object["OverLap"].asInt();
		int Dura = json_object["Dura"].asInt();

		char sMakeIndex[0x100] = { 0 };
		char sOverLap[0x100] = { 0 };
		char sDura[0x100];

		_itoa(MakeIndex, sMakeIndex, 10);
		_itoa(OverLap, sOverLap, 10);
		_itoa(Dura, sDura, 10);

		FindAndUpdataItemList(sMakeIndex, sOverLap,sDura);
		OutBagInfo();
	}
}


void SubBagItem(MYPACK_DATA* pData, string strJson)
{
	DBG("减少物品");
	string out = get_string(strJson);
	Json::Reader reader;
	Json::Value json_object;
	if (reader.parse(out.c_str(), json_object))
	{
		int MakeIndex = json_object["MakeIndex"].asInt();

		BAGINFO b;
		char sMakeIndex[0x100] = { 0 };

		_itoa(MakeIndex, sMakeIndex, 10);
		FindAndDelItemList(sMakeIndex);
		OutBagInfo();
	}

}

void GetBankInfo(MYPACK_DATA* pData, string strJson)
{
	
	string out = get_string(strJson);
	DBG("来获取仓库信息!!");
	DBG("%s", strJson.c_str());
	Json::Reader reader;
	Json::Value json_object;
	m_banklist.clear();
	if (reader.parse(out.c_str(), json_object))
	{
		int iSize = json_object["Items"].size();
		for (int i = 0; i < iSize; i++)
		{
			string Src = json_object["Items"][i]["Src"].asString();
			int Index = json_object["Items"][i]["Index"].asInt();
			int MakeIndex = json_object["Items"][i]["MakeIndex"].asInt();
			int OverLap = json_object["Items"][i]["OverLap"].asInt();
			int Dura = json_object["Items"][i]["Dura"].asInt();
			int DuraMax = json_object["Items"][i]["DuraMax"].asInt();

			BAGINFO b;
			char sSrc[0x100];
			strcpy_s(sSrc, Src.data());
			char sIndex[0x100];
			char sMakeIndex[0x100];
			char sOverLap[0x100];
			char sDura[0x100];
			char sDuraMax[0x100];

			_itoa(Index, sIndex, 10);
			_itoa(MakeIndex, sMakeIndex, 10);
			_itoa(OverLap, sOverLap, 10);
			_itoa(Dura, sDura, 10);
			_itoa(DuraMax, sDuraMax, 10);

			strcpy(b.Src, sSrc);
			strcpy(b.Index, sIndex);
			strcpy(b.MakeIndex, sMakeIndex);

			if (Dura > 0)
			{
				strcpy(b.OverLap, sDura);
			}
			if (Dura == 0)
			{
				strcpy(b.OverLap, sOverLap);
			}
			m_banklist.push_back(b);
		}
		OutBankInfo();
	}
}

void GetBagInfo(MYPACK_DATA* pData, string strJson)
{
	
	string out = get_string(strJson);
	Json::Reader reader;
	Json::Value json_object;
	hs_list.clear();
	if (reader.parse(out.c_str(), json_object))
	{
		int iSize = json_object.size();
		for (int i = 0; i < iSize; i++)
		{
			string Src = json_object[i]["Src"].asString();
			int Index = json_object[i]["Index"].asInt();
			int MakeIndex = json_object[i]["MakeIndex"].asInt();
			int OverLap = json_object[i]["OverLap"].asInt();
			int Dura = json_object[i]["Dura"].asInt();
			int DuraMax = json_object[i]["DuraMax"].asInt();

			BAGINFO b;
			char sSrc[0x100];
			strcpy_s(sSrc, Src.data());
			char sIndex[0x100];
			char sMakeIndex[0x100];
			char sOverLap[0x100];
			char sDura[0x100];
			char sDuraMax[0x100];

			_itoa(Index, sIndex, 10);
			_itoa(MakeIndex, sMakeIndex, 10);
			_itoa(OverLap, sOverLap, 10);
			_itoa(Dura, sDura, 10);
			_itoa(DuraMax, sDuraMax, 10);

			strcpy(b.Src, sSrc);
			strcpy(b.Index, sIndex);
			strcpy(b.MakeIndex, sMakeIndex);

			if (Dura > 0 )
			{
				strcpy(b.OverLap, sDura);
			}
			if (Dura == 0 )
			{
				strcpy(b.OverLap, sOverLap);
			}

			m_baglist.push_back(b);
		}
		OutBagInfo();
	}

}

void GetHeiShiInfo(MYPACK_DATA* pData, string strJson)
{

	
	string out = get_string(strJson);
	DBG("%s", out.c_str());

	Json::Reader reader;
	Json::Value json_object;
	HWND hwnd = ::GetDlgItem(g_hMgDlg, IDC_LIST2);
	::SendMessage(hwnd, LB_RESETCONTENT, 0, (LPARAM)NULL);
	hs_list.clear();
	if (reader.parse(out.c_str(), json_object))
	{
		//DBG("size:%d", json_object.size());
		int iSize = json_object.size();
		for (int i = 0; i < iSize; i++)
		{
			string Src = json_object[i]["useritem"]["Src"].asString();
			int Index = json_object[i]["useritem"]["Index"].asInt();
			int MakeIndex = json_object[i]["useritem"]["MakeIndex"].asInt();
			int OverLap = json_object[i]["useritem"]["OverLap"].asInt();
			int price = json_object[i]["price"].asInt();

			HEISHI h;
			char sSrc[0x100];
			strcpy_s(sSrc, Src.data());
			char sIndex[0x100];
			char sMakeIndex[0x100];
			char sOverLap[0x100];
			char sprice[0x100];
			_itoa(Index, sIndex, 10);
			_itoa(MakeIndex, sMakeIndex, 10);
			_itoa(OverLap, sOverLap, 10);
			_itoa(price, sprice, 10);



			strcpy(h.Src, sSrc);
			strcpy(h.Index, sIndex);
			strcpy(h.MakeIndex, sMakeIndex);
			strcpy(h.price, sprice);

			hs_list.push_back(h);

			//DBG("Src:%s index:%s makeindex:%s  price:%s", it->Src, it->Index, it->MakeIndex, it->price);
			char sOut[0x100];
			sprintf(sOut, "Src:%s index:%d makeindex:%s  price:%s num:%s", sSrc, Index, sMakeIndex, sprice,sOverLap);

			::SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)sOut);
		}
	}

}


void AddMonster(MYPACK_DATA* pData, string strJson)
{
	int x = pData->p1;
	int y = pData->p2;

	Json::Reader reader;
	Json::Value json_object;
	if (reader.parse(strJson.c_str(), json_object))
	{

		string Name = json_object["Name"].asString();
		string UserID = json_object["UserID"].asString();
		int x = json_object["X"].asInt();
		int y = json_object["Y"].asInt();
		int type = json_object["type"].asInt();

		//HWND hwnd = ::GetDlgItem(g_hMgDlg, IDC_LIST1);
		//::SendMessage(hwnd, LB_RESETCONTENT, 0, (LPARAM)NULL);
		MONSTER m;

		strcpy(m.Name, Name.data());
		strcpy(m.UserID, UserID.data());
		_itoa(x, m.x, 10);
		_itoa(y, m.y, 10);
		_itoa(type, m.type, 10);

		m_monlist.push_back(m);

		//DBG("-------------名字:%s  UserID:%s 坐标:%d,%d 类型:%d", Name.c_str(), UserID.c_str(), x, y, type);

		MONSTER_LIST_IT it;
		//DBG("当前列表有:[%d]个对象", m_monlist.size());
		for (it = m_monlist.begin(); it != m_monlist.end(); it++)
		{
			char sOut[0x100];

			sprintf(sOut, "名字:%s  UserID:%s 坐标:%s,%s 类型:%s", it->Name, it->UserID, it->x, it->y, it->type);
		}

	}

}

void FindAndDelMonList(char* UserID)
{
	MONSTER_LIST_IT it;
	for (it = m_monlist.begin(); it != m_monlist.end(); it++)
	{
		//DBG("查找值:%s -- 原始值:%s", UserID, it->UserID);

		if (strstr(UserID, it->UserID))
		{
			//DBG("有");
			m_monlist.erase(it);
			return;
		}
	}
	//DBG("没有");
}

void DelMonster(MYPACK_DATA* pData, string strJson)
{

	Json::Reader reader;
	Json::Value json_object;
	if (reader.parse(strJson.c_str(), json_object))
	{

		string UserID = json_object["UserID"].asString();



		//HWND hwnd = ::GetDlgItem(g_hMgDlg, IDC_LIST1);
		//::SendMessage(hwnd, LB_RESETCONTENT, 0, (LPARAM)NULL);


		//DBG("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");

		char sUserID[0x100];
		strcpy(sUserID, UserID.data());

		FindAndDelMonList(sUserID);
		MONSTER_LIST_IT it;
		for (it = m_monlist.begin(); it != m_monlist.end(); it++)
		{
			char sOut[0x100];
			sprintf(sOut, "名字:%s  UserID:%s 坐标:%s,%s", it->Name, it->UserID, it->x, it->y);

			//::SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)sOut);
		}

	}


}

void GetRoleLocation(MYPACK_DATA* pData, string strJson)
{
	//DBG("11111111");
	int x = pData->w1;
	int y = pData->w2;
	char sLocation[0x20];

	play_info.x = x;
	play_info.y = y;
	sprintf_s(sLocation, "%d,%d", x, y);

	SetDlgItemTextA(g_hMgDlg, IDC_PLAYER_LOCATIONS, sLocation);
}

void GetRoleMap(MYPACK_DATA* pData, string strJson)
{
	DBG("这里清除列表");
	m_monlist.clear();
	//HWND hwnd = ::GetDlgItem(g_hMgDlg, IDC_LIST1);
	//::SendMessage(hwnd, LB_RESETCONTENT, 0, (LPARAM)NULL);
	int x = pData->p1;
	int y = pData->p2;
	char sLocation[0x20];

	sprintf_s(sLocation, "%d,%d", x, y);

	SetDlgItemTextA(g_hMgDlg, IDC_PLAYER_LOCATIONS, sLocation);

	Json::Reader reader;
	Json::Value json_object;
	if (reader.parse(strJson.c_str(), json_object))
	{
		string MapName = json_object["MapName"].asString();
		const char* sMapName;
		sMapName = MapName.c_str();
		play_info.x = x;
		play_info.y = y;
		strcpy(play_info.Map, MapName.c_str());
		SetDlgItemTextA(g_hMgDlg, IDC_PLAYER_MAP, sMapName);


	}
}

void GetRoleInfo(string strJson) {
	string out = get_string(strJson);
	Json::Reader reader;
	Json::Value json_object;
	if (reader.parse(out.c_str(), json_object))
	{
		DBG("全局人物ID:%s ", g_UserID);
		string UserID = json_object["UserID"].asString();

		if (strstr(UserID.c_str(),g_UserID))
		{
			string player_name = json_object["Name"].asString();
			//string MasterId = json_object["MasterId"].asString();



			const char* splayer_name;
			//const char* sMasterId;
			const char* sUserID;

			splayer_name = player_name.c_str();
			//sMasterId = MasterId.c_str();
			sUserID = UserID.c_str();

			//g_UserID = sUserID;
			//g_MasterId = sMasterId;
			g_player_name = splayer_name;


			strcpy(play_info.Name, player_name.c_str());
			strcpy(play_info.UserID, UserID.c_str());
			strcpy(play_info.MasterID, UserID.c_str());

			SetDlgItemTextA(g_hMgDlg, IDC_PLAYER_NAME, splayer_name);
			SetDlgItemTextA(g_hMgDlg, IDC_PLAYER_MASTERID, sUserID);
			SetDlgItemTextA(g_hMgDlg, IDC_USERID, sUserID);
			
			DBG("等于 返回");
		}

		

	}

}






void GameEvent(MYPACK_DATA* pData, string strJson)
{
	switch (pData->dwHead)
	{
	case 0x14:
	{
		//DBG("来执行人物信息!");
		//GetRoleInfo(strJson);
	}break;
	case 0x32:
	{
		//DBG("来执行人物信息!");
		GetRoleInfo(strJson);
	}break;
	case 0xA:
	{

		AddMonster(pData, strJson);

	}break;
	case 0x1E:
	{

		DelMonster(pData, strJson);

	}break;

	case 0x2A://热血合击
	{
		GetRoleInfo(strJson);

	}break;
	case 0x33://热血合击
	{
		GetRoleMap(pData, strJson);
	}break;
	case 0xC9:
	{
		GetBagInfo(pData, strJson);
	}break;
	case 0xC8:
	{
		AddBagItem(pData, strJson);
	}break;
	case 0xCA:
	{
		SubBagItem(pData, strJson);
	}break;
	case 0xCB:
	{
		UpdataBagItem(pData, strJson);
	}break;
	case 0x292:
	{
		GetRoleUserID(pData, strJson);
	}break;
	case 0x27A:
	{
		GetRoleMap(pData, strJson);
	}break;
	case 0x2C0:
	{
		GetBankInfo(pData, strJson);
	}break;
	case 0x2C1:	//取
	{
		DelItemByPullBank(pData, strJson);
	}break;
	case 0x2BD:	//存
	{
		DelItemByPushBank(pData, strJson);
	}break;
	case 0x492:
	{

		GetHeiShiInfo(pData, strJson);

	}break;
	case 0xBC3:	//send walk
	{
		GetRoleLocation(pData, strJson);
	}break;
	case 0xBC5://send run
	{
		GetRoleLocation(pData, strJson);
	}break;


	default:
		break;
	}
}