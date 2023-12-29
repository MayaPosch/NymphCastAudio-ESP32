/*
	Semaphore_POSIX.h - Definition of the SemaphoreImpl class for POSIX Threads.
*/


#ifndef Core_Semaphore_POSIX_INCLUDED
#define Core_Semaphore_POSIX_INCLUDED


#include "../Core.h"
#include <pthread.h>
#include <errno.h>


namespace Poco {

class Foundation_API SemaphoreImpl {
protected:
	SemaphoreImpl(int n, int max);		
	~SemaphoreImpl();
	bool setImpl();
	bool waitImpl();
	bool waitImpl(long milliseconds);
	
private:
	volatile int    _n;
	int             _max;
	pthread_mutex_t _mutex;
	pthread_cond_t  _cond;
};


// inlines
inline bool SemaphoreImpl::setImpl() {
	if (pthread_mutex_lock(&_mutex)) {	
		//throw SystemException("cannot signal semaphore (lock)");
		return false;
	}
	
	if (_n < _max) {
		++_n;
	}
	else {
		pthread_mutex_unlock(&_mutex);
		//throw SystemException("cannot signal semaphore: count would exceed maximum");
		return false;
	}	
	
	if (pthread_cond_signal(&_cond)) {
		pthread_mutex_unlock(&_mutex);
		//throw SystemException("cannot signal semaphore");
		return false;
	}
	
	pthread_mutex_unlock(&_mutex);
}


} // namespace Poco


#endif // Core_Semaphore_POSIX_INCLUDED
