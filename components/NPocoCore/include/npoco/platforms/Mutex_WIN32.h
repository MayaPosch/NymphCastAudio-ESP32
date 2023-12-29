/*
	Mutex_WIN32.h - Definition of the MutexImpl and FastMutexImpl classes for WIN32.
*/


#ifndef Core_Mutex_WIN32_INCLUDED
#define Core_Mutex_WIN32_INCLUDED


#include "../Core.h"
#include "UnWindows.h"


namespace Poco {


class Foundation_API MutexImpl
{
protected:
	MutexImpl();
	~MutexImpl();
	bool lockImpl();
	bool tryLockImpl();
	bool tryLockImpl(long milliseconds);
	void unlockImpl();
	
private:
	CRITICAL_SECTION _cs;
};


typedef MutexImpl FastMutexImpl;


//
// inlines
//
inline bool MutexImpl::lockImpl() {
	try {
		EnterCriticalSection(&_cs);
	}
	catch (...) {
		return false;
	}
	
	return true;
}


inline bool MutexImpl::tryLockImpl() {
	return TryEnterCriticalSection(&_cs) != 0;
}


inline void MutexImpl::unlockImpl() {
	LeaveCriticalSection(&_cs);
}


} // namespace Poco


#endif // Core_Mutex_WIN32_INCLUDED
