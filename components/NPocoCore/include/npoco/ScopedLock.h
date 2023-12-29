/*
	ScopedLock.h - Definition of the ScopedLock template class.
*/


#ifndef Core_ScopedLock_INCLUDED
#define Core_ScopedLock_INCLUDED


#include "Core.h"


namespace Poco {

/// A class that simplifies thread synchronization with a mutex.
/// The constructor accepts a Mutex (and optionally a timeout value in milliseconds) 
/// and locks it.
/// The destructor unlocks the mutex.
template <class M>
class ScopedLock {
public:
	explicit ScopedLock(M& mutex): _mutex(mutex) {
		_mutex.lock();
	}
	
	ScopedLock(M& mutex, long milliseconds): _mutex(mutex) {
		_mutex.lock(milliseconds);
	}
	
	~ScopedLock() {
		_mutex.unlock();
	}

private:
	M& _mutex;

	ScopedLock();
	ScopedLock(const ScopedLock&);
	ScopedLock& operator = (const ScopedLock&);
};


/// A class that simplifies thread synchronization
/// with a mutex.
/// The constructor accepts a Mutex (and optionally
/// a timeout value in milliseconds) and locks it.
/// The destructor unlocks the mutex.
/// The unlock() member function allows for manual
/// unlocking of the mutex.
template <class M>
class ScopedLockWithUnlock {
public:
	explicit ScopedLockWithUnlock(M& mutex): _pMutex(&mutex) {
		_pMutex->lock();
	}
	
	ScopedLockWithUnlock(M& mutex, long milliseconds): _pMutex(&mutex) {
		_pMutex->lock(milliseconds);
	}
	
	~ScopedLockWithUnlock() {
		unlock();
	}
	
	void unlock() {
		if (_pMutex) {
			_pMutex->unlock();
			_pMutex = 0;
		}
	}

private:
	M* _pMutex;

	ScopedLockWithUnlock();
	ScopedLockWithUnlock(const ScopedLockWithUnlock&);
	ScopedLockWithUnlock& operator = (const ScopedLockWithUnlock&);
};


} // namespace Poco


#endif // Core_ScopedLock_INCLUDED
