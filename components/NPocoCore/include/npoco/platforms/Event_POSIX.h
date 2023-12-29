/*
	Event_POSIX.h - Definition of the EventImpl class for POSIX Threads.
*/


#ifndef Core_Event_POSIX_INCLUDED
#define Core_Event_POSIX_INCLUDED


#include "../Core.h"
#include <pthread.h>
#include <errno.h>


namespace Poco {

class Foundation_API EventImpl {
protected:
	EventImpl(bool autoReset);		
	~EventImpl();
	bool setImpl();
	bool waitImpl();
	bool waitImpl(long milliseconds);
	bool resetImpl();
	
private:
	bool            _auto;
	volatile bool   _state;
	pthread_mutex_t _mutex;
	pthread_cond_t  _cond;
};


// inlines
inline bool EventImpl::setImpl() {
	if (pthread_mutex_lock(&_mutex)) { return false; }
	
	_state = true;
	if (pthread_cond_broadcast(&_cond)) {
		pthread_mutex_unlock(&_mutex);
		return false;
	}
	
	pthread_mutex_unlock(&_mutex);
	return true;
}


inline bool EventImpl::resetImpl() {
	if (pthread_mutex_lock(&_mutex)) { return false; }
	
	_state = false;
	pthread_mutex_unlock(&_mutex);
	return true;
}

} // namespace Poco


#endif // Core_Event_POSIX_INCLUDED
