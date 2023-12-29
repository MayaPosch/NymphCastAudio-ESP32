/*
	RWLock_Android.cpp -
*/


#include "RWLock_Android.h"


namespace Poco {

RWLockImpl::RWLockImpl() {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	if (pthread_mutex_init(&_mutex, &attr)) {
		pthread_mutexattr_destroy(&attr);
		//throw SystemException("cannot create mutex");
		// TODO: Handle error.
	}
	
	pthread_mutexattr_destroy(&attr);}


RWLockImpl::~RWLockImpl() {
	pthread_mutex_destroy(&_mutex);
}


} // namespace Poco
