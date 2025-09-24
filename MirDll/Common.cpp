#include "Common.h"
#include <map>
#include <__msvc_chrono.hpp>
#include <intsafe.h>


DWORD64 g_dwGameBase = 0;
SOCKET GameSocket = NULL;
HWND g_hGameWnd = NULL;
HINSTANCE g_hInst = NULL;
HWND g_hMgDlg = NULL;

int g_GameNum = 0;
int g_MyNum = 0;
bool isSendOK = false;
bool isRecvOK = false;


namespace global
{
	
	DWORD64 g_hGameImage = NULL;
}


std::map<int, ITEM_NAME> g_ItemHash;


bool FilterPack(DWORD dwHeadID)
{
	if (dwHeadID == 0xEA60
		|| dwHeadID == 0x375
		|| dwHeadID == 0x64
		|| dwHeadID == 1
		)
	{
		return false;
	}
	else
	{
		return true;
	}
}



bool FilterPack2(DWORD dwHeadID)
{
	if (dwHeadID == 0xC9)
	{
		return true;
	}
	else
	{
		return false;
	}
}


wstring MBytesToWString(const char* lpcszString)
{
	int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	wstring wString = (wchar_t*)pUnicode;
	delete[] pUnicode;
	return wString;
}


string WStringToMBytes(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen;
	// wide char to multi char
	iTextLen = ::WideCharToMultiByte(CP_ACP, 0, lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_ACP, 0, lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}


wstring UTF8ToWString(const char* lpcszString)
{
	int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode;
	pUnicode = new wchar_t[unicodeLen + 1];
	memset((void*)pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_UTF8, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	wstring wstrReturn(pUnicode);
	delete[] pUnicode;
	return wstrReturn;
}


string WStringToUTF8(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}


std::string& std_string_format(std::string& _str, const char* _Format, ...)
{
	std::string tmp;

	va_list marker = NULL;
	va_start(marker, _Format);

	size_t num_of_chars = _vscprintf(_Format, marker);

	if (num_of_chars > tmp.capacity()) {
		tmp.resize(num_of_chars + 1);
	}

	vsprintf_s((char*)tmp.data(), tmp.capacity(), _Format, marker);

	va_end(marker);

	_str = tmp.c_str();
	return _str;
}


string readString(const DWORD64 dwPtr)
{
	string ret = WStringToMBytes(UTF8ToWString((const char *)dwPtr).c_str());
	return ret;
}

string get_string(string res) {

	//É¾³ý»»ÐÐ·û

	int r = res.find(' ');
	while (r != string::npos)
	{
		if (r != string::npos)
		{
			res.replace(r, 1, "");
			r = res.find(' ');
		}
		
	}

	//É¾³ýËùÓÐ¿Õ¸ñ

	res.erase(std::remove_if(res.begin(), res.end(), std::isspace), res.end());

	return res;

}

int getUnixTime() {

	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		);
	long long  time = ms.count();

	char timestr[0x200];


	sprintf_s(timestr, "%lld", time);

	string strOut = timestr;

	string time2 = strOut.substr(4, strOut.size()).c_str();

	int iTime = atoi(time2.c_str());
	return iTime;
}

char stringTochar(string str) {
	char cStr[0x1024];
	for (int i = 0; i < str.length(); i++)
		cStr[i] = str[i];

	return cStr[0x1024];
}

void DBG(const char* format, ...)
{

	std::string temp;
	va_list marker = { 0 };
	va_start(marker, format);
	size_t num_of_chars = _vscprintf(format, marker);
	if (num_of_chars > temp.capacity())
	{
		temp.reserve(num_of_chars + 1);
	}
	vsprintf_s(const_cast<char*>(temp.c_str()), num_of_chars + 1, format, marker);
	OutputDebugStringA(temp.c_str());
}

void DBGW(const wchar_t* format, ...)
{

	std::wstring temp;
	va_list marker = { 0 };
	va_start(marker, format);
	size_t num_of_chars = _vscwprintf(format, marker);
	if (num_of_chars > temp.capacity())
	{
		temp.reserve(num_of_chars + 1);
	}
	vswprintf_s(const_cast<wchar_t*>(temp.c_str()), num_of_chars + 1, format, marker);
	OutputDebugStringW(temp.c_str());
}

void DBGHex(unsigned char* pData, int size)
{
	string a;
	for (int i = 0; i < size; i++)
	{
		if (i % 16 == 0) {
			a += "\r\n";
		}

		char szTemp[10] = { 0 };
		sprintf_s(szTemp, "%02X", pData[i]);
		a += string(szTemp) + " ";
	}

	string str = "";
	for (int i = 0; i < size; i++)
	{
		if (pData[i] == 0)
		{
			str += 0x2E;
		}
		else
		{
			str += pData[i];
		}
	}

	DBG(a.c_str());
	DBG("%s", str.c_str());

}

