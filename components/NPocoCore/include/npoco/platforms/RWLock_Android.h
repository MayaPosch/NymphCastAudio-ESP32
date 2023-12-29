/*
	RWLock_Android.h - Definition of the RWLockImpl class for Android Threads.
*/


#ifndef Core_RWLock_Android_INCLUDED
#define Core_RWLock_Android_INCLUDED


#include "../Core.h"
#include <pthread.h>
#include <errno.h>


namespace Poco {


class Foundation_API RWLockImpl
{
protected:
	RWLockImpl();
	~RWLockImpl();
	bool readLockImpl();
	bool tryReadLockImpl();
	bool writeLockImpl();
	bool tryWriteLockImpl();
	bool unlockImpl();
	
private:
	pthread_mutex_t _mutex;
};


// inlines
inline bool RWLockImpl::readLockImpl() {
	if (pthread_mutex_lock(&_mutex)) { return false; }
	return true;
}


inline bool RWLockImpl::tryReadLockImpl() {
	int rc = pthread_mutex_trylock(&_mutex);
	if (rc == 0)
		return true;
	else if (rc == EBUSY)
		return false;
	else
		return false;
}


inline bool RWLockImpl::writeLockImpl() {
	if (pthread_mutex_lock(&_mutex)) { return false; }
	return true;
}


inline bool RWLockImpl::tryWriteLockImpl() {
	int rc = pthread_mutex_trylock(&_mutex);
	if (rc == 0)
		return true;
	else if (rc == EBUSY)
		return false;
	else
		return false;
}


inline bool RWLockImpl::unlockImpl() {
	if (pthread_mutex_unlock(&_mutex)) { return false; }
	return true;
}


} // namespace Poco


#endif // Core_RWLock_Android_INCLUDED
