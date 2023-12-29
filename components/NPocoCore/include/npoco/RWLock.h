/*
	RWLock.h - Definition of the RWLock class.
*/


#ifndef Core_RWLock_INCLUDED
#define Core_RWLock_INCLUDED


#include "Core.h"


#if defined(POCO_OS_FAMILY_FREERTOS)
#include "platforms/RWLock_FreeRTOS.h"
#elif defined(POCO_OS_FAMILY_WINDOWS)
#if defined(_WIN32_WCE)
#include "platforms/RWLock_WINCE.h"
#else
#include "platforms/RWLock_WIN32.h"
#endif
#elif POCO_OS == POCO_OS_ANDROID
#include "platforms/RWLock_Android.h"
#elif defined(POCO_VXWORKS)
#include "platforms/RWLock_VX.h"
#else
#include "platforms/RWLock_POSIX.h"
#endif


namespace Poco {

class ScopedRWLock;
class ScopedReadRWLock;
class ScopedWriteRWLock;

/// A reader writer lock allows multiple concurrent
/// readers or one exclusive writer.
class Foundation_API RWLock: private RWLockImpl {
public:
	using ScopedLock = ScopedRWLock;
	using ScopedReadLock = ScopedReadRWLock;
	using ScopedWriteLock = ScopedWriteRWLock;

	RWLock();
		/// Creates the Reader/Writer lock.
		
	~RWLock();
		/// Destroys the Reader/Writer lock.
	
	bool readLock();
		/// Acquires a read lock. If another thread currently holds a write lock,
		/// waits until the write lock is released.

	bool tryReadLock();
		/// Tries to acquire a read lock. Immediately returns true if successful, or
		/// false if another thread currently holds a write lock.

	bool writeLock();
		/// Acquires a write lock. If one or more other threads currently hold 
		/// locks, waits until all locks are released. The results are undefined
		/// if the same thread already holds a read or write lock

	bool tryWriteLock();
		/// Tries to acquire a write lock. Immediately returns true if successful,
		/// or false if one or more other threads currently hold 
		/// locks. The result is undefined if the same thread already
		/// holds a read or write lock.

	bool unlock();
		/// Releases the read or write lock.

private:
	RWLock(const RWLock&);
	RWLock& operator = (const RWLock&);
};


class Foundation_API ScopedRWLock
	/// A variant of ScopedLock for reader/writer locks.
{
public:
	ScopedRWLock(RWLock& rwl, bool write = false);
	~ScopedRWLock();

private:
	RWLock& _rwl;

	ScopedRWLock();
	ScopedRWLock(const ScopedRWLock&);
	ScopedRWLock& operator = (const ScopedRWLock&);
};


class Foundation_API ScopedReadRWLock : public ScopedRWLock
	/// A variant of ScopedLock for reader locks.
{
public:
	ScopedReadRWLock(RWLock& rwl);
	~ScopedReadRWLock();
};


/// A variant of ScopedLock for writer locks.
class Foundation_API ScopedWriteRWLock : public ScopedRWLock {
public:
	ScopedWriteRWLock(RWLock& rwl);
	~ScopedWriteRWLock();
};


// inlines
inline bool RWLock::readLock() {
	return readLockImpl();
}


inline bool RWLock::tryReadLock() {
	return tryReadLockImpl();
}


inline bool RWLock::writeLock() {
	return writeLockImpl();
}


inline bool RWLock::tryWriteLock() {
	return tryWriteLockImpl();
}


inline bool RWLock::unlock() {
	return unlockImpl();
}


inline ScopedRWLock::ScopedRWLock(RWLock& rwl, bool write): _rwl(rwl) {
	if (write)
		_rwl.writeLock();
	else
		_rwl.readLock();
}


inline ScopedRWLock::~ScopedRWLock() {
	if (!_rwl.unlock()) {
		poco_unexpected();
	}
}


inline ScopedReadRWLock::ScopedReadRWLock(RWLock& rwl): ScopedRWLock(rwl, false)
{
}


inline ScopedReadRWLock::~ScopedReadRWLock()
{
}


inline ScopedWriteRWLock::ScopedWriteRWLock(RWLock& rwl): ScopedRWLock(rwl, true)
{
}


inline ScopedWriteRWLock::~ScopedWriteRWLock()
{
}


} // namespace Poco


#endif // Core_RWLock_INCLUDED
