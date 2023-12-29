/*
	Semaphore_FreeRTOS.h - Definition of the SemaphoreImpl class for FreeRTOS Threads.
*/


#ifndef Core_Semaphore_FreeRTOS_INCLUDED
#define Core_Semaphore_FreeRTOS_INCLUDED


#include "../Core.h"
//#include <pthread.h>
//#include <errno.h>

#include <mutex>
#include <condition_variable>


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
	//pthread_mutex_t _mutex;
	//pthread_cond_t  _cond;
	std::mutex 				_mutex;
	std::condition_variable _cond;
};


// inlines
inline bool SemaphoreImpl::setImpl() {
	//if (pthread_mutex_lock(&_mutex)) {
		//throw SystemException("cannot signal semaphore (lock)");
		/*return false;
	}*/
	
	_mutex.lock();
	
	if (_n < _max) {
		++_n;
	}
	else {
		//pthread_mutex_unlock(&_mutex);
		_mutex.unlock();
		//throw SystemException("cannot signal semaphore: count would exceed maximum");
		return false;
	}
	
	_cond.notify_one();
	
	//if (pthread_cond_signal(&_cond)) {
	//if (_cond.notify_one()) {
		//pthread_mutex_unlock(&_mutex);
		//_mutex.unlock();
		//throw SystemException("cannot signal semaphore");
		//return false;
	//}
	
	//pthread_mutex_unlock(&_mutex);
	_mutex.unlock();
	
	return true;
}


} // namespace Poco


#endif // Core_Semaphore_FreeRTOS_INCLUDED
