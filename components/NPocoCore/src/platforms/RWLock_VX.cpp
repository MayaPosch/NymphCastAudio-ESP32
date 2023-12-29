/*
	RWLock_VX.cpp - 
*/


#include "RWLock_VX.h"
#include <cstring>


namespace Poco {

RWLockImpl::RWLockImpl() {
#if defined(POCO_VXWORKS)
	// This workaround is for VxWorks 5.x where
	// pthread_mutex_init() won't properly initialize the mutex
	// resulting in a subsequent freeze in pthread_mutex_destroy()
	// if the mutex has never been used.
	std::memset(&_mutex, 0, sizeof(_mutex));
#endif
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	if (pthread_mutex_init(&_mutex, &attr)) {
		pthread_mutexattr_destroy(&attr);
		//throw SystemException("cannot create mutex");
		// TODO: Handle error.
	}
	pthread_mutexattr_destroy(&attr);
}


RWLockImpl::~RWLockImpl()
{
	pthread_mutex_destroy(&_mutex);
}


} // namespace Poco
