#pragma once

#include <winbase.h>
#include <string>

using namespace std;

class xPacket
{
public:
	xPacket()
	{
		m_pBuf = NULL;
		m_iSize = 0;
		m_iMaxSize = 0;
		m_bBuildInBuffer = FALSE;
	}

	xPacket(int nSize)
	{
		m_pBuf = NULL;
		m_iSize = 0;
		m_iMaxSize = 0;
		m_bBuildInBuffer = FALSE;
		create(nSize);
	}

	xPacket(BYTE * pbuf, int nSize)
	{
		m_pBuf = NULL;
		m_iSize = 0;
		m_iMaxSize = 0;
		m_bBuildInBuffer = FALSE;
		create(pbuf, nSize);
	}

	virtual ~xPacket(void)
	{
		destroy();
	}

	BOOL create(BYTE * pbuf, int nSize)
	{
		destroy();
		m_bBuildInBuffer = FALSE;
		m_pBuf = pbuf;
		m_iMaxSize = nSize;
		return TRUE;
	}

	BOOL create(int nSize)
	{
		destroy();
		if (nSize > 0)
		{
			m_bBuildInBuffer = TRUE;
			m_pBuf = new BYTE[nSize];
			memset(m_pBuf, 0, nSize);
			m_iMaxSize = nSize;
		}
		return TRUE;
	}

	BOOL notcreated()
	{
		return (m_pBuf == NULL);
	}

	VOID destroy()
	{
		if (m_bBuildInBuffer)
			delete[]m_pBuf;
		m_pBuf = NULL;
		m_iMaxSize = 0;
		m_iSize = 0;
		m_bBuildInBuffer = FALSE;
	}

	BOOL push(LPVOID lpData, int iDatasize)
	{
		if (m_iMaxSize - m_iSize < iDatasize) return FALSE;
		memcpy(m_pBuf + m_iSize, lpData, iDatasize);
		m_iSize += iDatasize;
		return TRUE;
	}

	BOOL push(const BYTE *pData, int iSize)
	{
		return push((LPVOID)pData, iSize);
	}

	BOOL push(WORD data)
	{
		return push(&data, 2);
	}

	BOOL push(DWORD data)
	{
		return push(&data, 4);
	}

	BOOL push(BYTE data)
	{
		return push(&data, 1);
	}

	BOOL push(LONGLONG data)
	{
		return push(&data, 8);
	}

	BOOL Make(LONGLONG data, int len)
	{
		switch (len)
		{
		case 1:
			return push((BYTE)data);
			break;
		case 2:
			return push((WORD)data);
			break;
		case 4:
			return push((DWORD)data);
			break;
		case 8:
			return push((LONGLONG)data);
			break;
		}
		return false;
	}

	BOOL push(const BYTE * pszString)
	{
		int len = (int)strlen((char *)pszString);
		return push((LPVOID)pszString, len);
	}

	BOOL pushStr(string str)
	{
		int iLen = str.length();
		push((DWORD)iLen);
		return push((BYTE *)str.c_str(), iLen);
	}

	BOOL pushStr(const char *pStr)
	{
		int iLen = lstrlenA(pStr);
		push((DWORD)iLen);
		return push((BYTE *)pStr, iLen);
	}

	BOOL pushStr(const wchar_t* pStr)
	{
		int iLen = lstrlenW(pStr) * 2;
		push((DWORD)iLen);
		return push((BYTE *)pStr, iLen);
	}

	BOOL pop(LPVOID lpData, int iDatasize)
	{
		if (m_iSize < iDatasize) 
			return FALSE;
		if (lpData != NULL)
			memcpy(lpData, m_pBuf, iDatasize);
		//		m_iSize -= iDatasize;
		return free(iDatasize);
	}

	template <typename T>
	BOOL pop(T *t)
	{
		return pop(t, sizeof(T));
	}

	BOOL popStr(char *pStr)
	{
		DWORD dwLen = 0;
		pop(&dwLen);
		if (dwLen >= 0x100) {
			return false;
		}
		return pop(pStr, dwLen);
	}

	BOOL peek(LPVOID lpData, int iDatasize)
	{
		if (m_iSize < iDatasize) return FALSE;
		memcpy(lpData, m_pBuf + m_iSize, iDatasize);
		//		m_iSize -= iDatasize;
		return TRUE;
	}

	BOOL skip(int size)
	{
		m_iSize += size;
		return TRUE;
	}

	BOOL free(int iDatasize)
	{
		if (m_iSize < iDatasize)return FALSE;
		m_iSize -= iDatasize;
		if (m_iSize != 0)
			memmove(m_pBuf, m_pBuf + iDatasize, m_iSize);
		return TRUE;
	}
	VOID clear()
	{
		m_iSize = 0;
	}

	int	getsize() { return m_iSize; }
	BYTE * getbuf() { return m_pBuf; }
	BYTE * getfreebuf() { return (m_pBuf + m_iSize); }
	int	getmaxsize() { return m_iMaxSize; }
	int	getfreesize() { return m_iMaxSize - m_iSize; }
	void setsize(int nSize) { m_iSize = nSize; }
	BOOL addsize(int nSize)
	{
		if (nSize > getfreesize())return FALSE;
		m_iSize += nSize;
		return TRUE;
	}
private:

	BYTE * m_pBuf;
	int	m_iSize;
	int	m_iMaxSize;
	BOOL	m_bBuildInBuffer;
};


#define KEY_TYPE  BYTE

union KEY_NEXT
{
	BYTE b;
	WORD w;
};

class CPacket
{
public:
	CPacket(unsigned char* b, int len) : buf(b), rest(len)
	{
	}

	CPacket &Rest(unsigned char *b, int len)
	{
		buf = b;
		rest = len;
		return *this;
	}
	template <typename T>
	CPacket& Get(T& t) {
		if (rest < sizeof(t)) {
			return *this;
		}
		t = *(T*)buf;
		buf += sizeof(t);
		rest -= sizeof(t);
		return *this;
	}

	template <typename T>
	CPacket& operator >> (T& t) {
		return Get(t);
	}

	CPacket& Skip(int c) {
		buf += c;
		rest -= c;
		return *this;
	}

	CPacket& Get(BYTE* str, int len)
	{
		memcpy(str, buf, len);
		buf += (len);
		rest -= (len);
		return *this;
	}

	unsigned char* buf;
	int rest;
};

class CBuffer
{
public:
	CBuffer(char* buf) {
		cur = buf;
		buffer = buf;
		pos = 0;
	}
	char* GetBuffer() {
		return buffer;
	}
	int GetLength() {
		return pos;
	}
	CBuffer& Reset() {
		pos = 0;
		return *this;
	}

	CBuffer& Skip(int c) {
		pos += c;
		return *this;
	}
	CBuffer& Append(char* buf, int len) {
		memcpy(cur + pos, buf, len);
		pos += len;
		return *this;
	}
	CBuffer &Byte(BYTE byValue)
	{
		Append(byValue);
		return *this;
	}
	CBuffer &Word(WORD wValue)
	{
		Append(wValue);
		return *this;
	}
	CBuffer &Dword(DWORD dwValue)
	{
		Append(dwValue);
		return *this;
	}
	CBuffer &Float(FLOAT fValue)
	{
		Append(fValue);
		return *this;
	}
	CBuffer &nsWord(WORD wValue)
	{
		Append(ntohs(wValue));
		return *this;
	}
	CBuffer &nlDword(DWORD dwValue)
	{
		Append(ntohl(dwValue));
		return *this;
	}

	CBuffer &AppendStr(char *pStr)
	{
		WORD wLen = lstrlenA(pStr);
		Append(ntohs(wLen));
		Append(pStr, wLen);
		return *this;
	}
	CBuffer &AppendParamByte(char *pKey, BYTE byValue)
	{
		AppendStr(pKey);
		Byte(1);
		Byte(byValue);
		return *this;
	}

	CBuffer &AppendParam(char *pKey, char *pValue)
	{
		AppendStr(pKey);
		Byte(8);
		AppendStr(pValue);
		return *this;
	}
	CBuffer &AppendParamnsWord(char *pKey, WORD wValue)
	{
		AppendStr(pKey);
		Byte(0x12);
		nsWord(wValue);
		return *this;
	}
	CBuffer &AppendParamnlDword(char *pKey, DWORD dwValue)
	{
		AppendStr(pKey);
		Byte(4);
		nlDword(dwValue);
		return *this;
	}

	CBuffer &AppendArray(char *pKey, WORD wCount)
	{
		AppendStr(pKey);
		Byte(0x11);
		nsWord(wCount);
		return *this;
	}

	CBuffer &AppendData(char *pKey, char *pData, int size)
	{
		AppendStr(pKey);
		Byte(0x0A);
		nlDword(size);
		Append(pData, size);
		return *this;
	}
	CBuffer &AppendParamCount(WORD wCount)
	{
		Byte(0x12);
		nsWord(wCount);
		return *this;
	}

	template <typename T>
	CBuffer& Append(T t) {
		*(T*)(cur + pos) = t;
		pos += sizeof(T);
		return *this;
	}

	CBuffer& Append(char* str) {
		char len = (char)(strlen(str) & 0xFF);
		Append(len);
		Append((char*)str, len);
		return *this;
	}
	CBuffer &AppendHeader(char *str)
	{
		int len = strlen(str);
		Append((char*)str, len);
		return *this;
	}

	template <typename T>
	CBuffer& operator << (T t) {
		return Append(t);
	}

public:
	char* buffer;
	char* cur;
	int pos;
};