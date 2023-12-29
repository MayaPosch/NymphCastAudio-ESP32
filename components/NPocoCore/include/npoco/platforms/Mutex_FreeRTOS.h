/*
	Mutex_FreeRTOS.h - Definition of the MutexImpl and FastMutexImpl classes for FreeRTOS Threads.
*/


#ifndef Core_Mutex_FreeRTOS_INCLUDED
#define Core_Mutex_FreeRTOS_INCLUDED


#include "../Core.h"
#include <pthread.h>
#include <errno.h>

//#include <mutex>


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
	//std::mutex _mutex;
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
	
	//_mutex.lock();
	
	return true;
}


inline bool MutexImpl::tryLockImpl() {
	int rc = pthread_mutex_trylock(&_mutex);
	if (rc == 0) { return true; }
	else if (rc == EBUSY) { return false; }
	else { return false; }
	
	//if (!_mutex.try_lock()) { return false; }
	
	return true;
}


inline void MutexImpl::unlockImpl() {
	if (pthread_mutex_unlock(&_mutex)) {
		// TODO: Non-fatal, could fail if thread does not own mutex.
	}
	
	//_mutex.unlock();
}


} // namespace Poco


#endif // Core_Mutex_FreeRTOS_INCLUDED
