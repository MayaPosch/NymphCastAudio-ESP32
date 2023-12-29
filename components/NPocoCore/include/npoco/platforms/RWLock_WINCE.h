/*
	RWLock_WINCE.h - Definition of the RWLockImpl class for WINCE.
*/


#ifndef Foundation_RWLock_WINCE_INCLUDED
#define Foundation_RWLock_WINCE_INCLUDED


#include "../Core.h"
#include "UnWindows.h"


namespace Poco {

/// This implementation is based on the one from Stone Steps Inc,
/// licensed under the BSD license.
/// http://forums.stonesteps.ca/thread.asp?t=105
///
/// Note that with this implementation, writers always take
/// precedence over readers.
class Foundation_API RWLockImpl {
protected:
	RWLockImpl();
	~RWLockImpl();
	bool readLockImpl();
	bool tryReadLockImpl(DWORD timeout = 1);
	bool writeLockImpl();
	bool tryWriteLockImpl(DWORD timeout = 1);
	bool unlockImpl();
	
private:
	DWORD _readerCount;
	DWORD _readerWaiting;
	DWORD _writerCount;
	DWORD _writerWaiting;
	HANDLE _readerGreen;
	HANDLE _writerGreen;
	CRITICAL_SECTION _cs;
	bool _writeLock;
};

} // namespace Poco


#endif // Foundation_RWLock_WINCE_INCLUDED
