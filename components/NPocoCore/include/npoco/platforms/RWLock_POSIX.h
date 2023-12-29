/*
	RWLock_POSIX.h - Definition of the RWLockImpl class for POSIX Threads.
*/


#ifndef Core_RWLock_POSIX_INCLUDED
#define Core_RWLock_POSIX_INCLUDED


#include "../Core.h"
#include <pthread.h>
#include <errno.h>


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
	pthread_rwlock_t _rwl;
};


// inlines
inline bool RWLockImpl::readLockImpl() {
	if (pthread_rwlock_rdlock(&_rwl)) { return false; }
	return true;
}


inline bool RWLockImpl::tryReadLockImpl() {
	int rc = pthread_rwlock_tryrdlock(&_rwl);
	if (rc == 0)
		return true;
	else if (rc == EBUSY)
		return false;
	else
		return false;
}


inline bool RWLockImpl::writeLockImpl() {
	if (pthread_rwlock_wrlock(&_rwl)) { return false; }
	return true;
}


inline bool RWLockImpl::tryWriteLockImpl() {
	int rc = pthread_rwlock_trywrlock(&_rwl);
	if (rc == 0)
		return true;
	else if (rc == EBUSY)
		return false;
	else
		return false;
}


inline bool RWLockImpl::unlockImpl() {
	if (pthread_rwlock_unlock(&_rwl)) { return false; }
	return true;
}


} // namespace Poco


#endif // Core_RWLock_POSIX_INCLUDED
