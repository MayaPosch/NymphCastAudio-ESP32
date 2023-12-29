/*
	RWLock_FreeRTOS.h - Definition of the RWLockImpl class for FreeRTOS Threads.
*/


#ifndef Core_RWLock_FreeRTOS_INCLUDED
#define Core_RWLock_FreeRTOS_INCLUDED


#include "../Core.h"
//#include <pthread.h>
//#include <errno.h>
#include <shared_mutex>


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
	//pthread_rwlock_t _rwl;
	std::shared_timed_mutex _rwmutex;
	volatile bool writing = false;
};


// inlines
inline bool RWLockImpl::readLockImpl() {
	//if (pthread_rwlock_rdlock(&_rwl)) { return false; }
	//std::shared_lock<std::shared_timed_mutex> lock(_rwmutex);
	_rwmutex.lock_shared();
	
	return true;
}


inline bool RWLockImpl::tryReadLockImpl() {
	/* int rc = pthread_rwlock_tryrdlock(&_rwl);
	if (rc == 0)
		return true;
	else if (rc == EBUSY)
		return false;
	else
		return false; */
	//std::shared_lock<std::shared_timed_mutex> lock(_rwmutex);
	return _rwmutex.try_lock_shared();
	return true;
}


inline bool RWLockImpl::writeLockImpl() {
	//if (pthread_rwlock_wrlock(&_rwl)) { return false; }
	//std::lock_guard<std::shared_timed_mutex> writerLock(_rwmutex);
	_rwmutex.lock();
	writing = true;
	
	return true;
}


inline bool RWLockImpl::tryWriteLockImpl() {
	/* int rc = pthread_rwlock_trywrlock(&_rwl);
	if (rc == 0)
		return true;
	else if (rc == EBUSY)
		return false;
	else
		return false; */
	//std::unique_lock<std::shared_timed_mutex> lock(_rwmutex);
	if (!_rwmutex.try_lock()) { return false; }
	writing = true;
	return true;
}


inline bool RWLockImpl::unlockImpl() {
	//if (pthread_rwlock_unlock(&_rwl)) { return false; }
	//std::unique_lock<std::shared_timed_mutex> lock(_rwmutex);
	if (writing) 	{ _rwmutex.unlock(); }
	else 			{ _rwmutex.unlock_shared(); }
	return true;
}


} // namespace Poco


#endif // Core_RWLock_FreeRTOS_INCLUDED
