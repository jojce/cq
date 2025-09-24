#ifndef	__CRIRITCAL_SECTION_H__
#define	__CRIRITCAL_SECTION_H__
#include <minwinbase.h>

class CCriticalSection
{
public:
	CCriticalSection();
	~CCriticalSection();

public:
	void Lock();
	void UnLock();

private:
	CRITICAL_SECTION	m_CS;
};


class MutexLock
{
public:
	MutexLock(CCriticalSection& mutex) : m_mutex(mutex)
	{
		mutex.Lock();
	}
	~MutexLock()
	{
		m_mutex.UnLock();
	}

private:
	CCriticalSection& m_mutex;
};

#define THREAD_SAFE_HEADER \
	CCriticalSection m_Mutex;

#define LockIt() \
	MutexLock lock(m_Mutex);

#endif	//	__CRIRITCAL_SECTION_H__