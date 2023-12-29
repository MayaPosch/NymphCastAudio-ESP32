/*
	RWLock_WIN32.h - Definition of the RWLockImpl class for WIN32.
*/


#ifndef Core_RWLock_WIN32_INCLUDED
#define Core_RWLock_WIN32_INCLUDED


#include "../Core.h"
#include "UnWindows.h"


namespace Poco {

class Foundation_API RWLockImpl {
protected:
	RWLockImpl();
	~RWLockImpl();
	bool readLockImpl();
	bool tryReadLockImpl();
	bool writeLockImpl();
	bool tryWriteLockImpl();
	bool unlockImpl();
	
private:
	bool addWriter();
	bool removeWriter();
	DWORD tryReadLockOnce();

	HANDLE   _mutex;
	HANDLE   _readEvent;
	HANDLE   _writeEvent;
	unsigned _readers;
	unsigned _writersWaiting;
	unsigned _writers;
};


} // namespace Poco


#endif // Core_RWLock_WIN32_INCLUDED
