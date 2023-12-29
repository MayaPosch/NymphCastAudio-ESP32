/*
	Mutex_POSIX.h - Definition of the MutexImpl and FastMutexImpl classes for POSIX Threads.
*/


#ifndef Core_Mutex_POSIX_INCLUDED
#define Core_Mutex_POSIX_INCLUDED


#include "../Core.h"
#include <pthread.h>
#include <errno.h>


namespace Poco {

class Foundation_API MutexImpl {
protected:
	MutexImpl();
	MutexImpl(bool fast);
	~MutexImpl();
	bool lockImpl();
	bool tryLockImpl();
	bool tryLockImpl(long milliseconds);
	void unlockImpl();
	
private:
	pthread_mutex_t _mutex;
};


class Foundation_API FastMutexImpl: public MutexImpl {
protected:
	FastMutexImpl();
	~FastMutexImpl();
};


// inlines
inline bool MutexImpl::lockImpl() {
	if (pthread_mutex_lock(&_mutex)) {
		return false;
	}
	
	return true;
}


inline bool MutexImpl::tryLockImpl() {
	int rc = pthread_mutex_trylock(&_mutex);
	if (rc == 0) { return true; }
	else if (rc == EBUSY) { return false; }
	else { return false; }
}


inline void MutexImpl::unlockImpl() {
	if (pthread_mutex_unlock(&_mutex)) {
		// TODO: Non-fatal, could fail if thread does not own mutex.
	}
}


} // namespace Poco


#endif // Core_Mutex_POSIX_INCLUDED
