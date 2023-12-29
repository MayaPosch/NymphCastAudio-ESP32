/*
	Mutex_FreeRTOS.cpp - 
*/


#include "Mutex_FreeRTOS.h"
#include "../Timestamp.h"
#if !defined(POCO_NO_SYS_SELECT_H)
#include <sys/select.h>
#endif

#include <unistd.h>
#include <sys/time.h>



#if defined(_POSIX_TIMEOUTS) && (_POSIX_TIMEOUTS - 200112L) >= 0L
	#if defined(_POSIX_THREADS) && (_POSIX_THREADS - 200112L) >= 0L
		#define POCO_HAVE_MUTEX_TIMEOUT
	#endif
#endif


#ifndef POCO_HAVE_CLOCK_GETTIME
	#if (defined(_POSIX_TIMERS) && defined(CLOCK_REALTIME)) || defined(POCO_VXWORKS) || defined(__QNX__)
		#ifndef __APPLE__ // See GitHub issue #1453 - not available before Mac OS 10.12/iOS 10
			#define POCO_HAVE_CLOCK_GETTIME
		#endif
	#endif
#endif


namespace Poco {

MutexImpl::MutexImpl() {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
#if defined(PTHREAD_MUTEX_RECURSIVE_NP)
	pthread_mutexattr_settype_np(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
#else
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#endif
	if (pthread_mutex_init(&_mutex, &attr)) {
		pthread_mutexattr_destroy(&attr);
		//throw SystemException("cannot create mutex");
		// TODO: Handle error.
	}
	
	pthread_mutexattr_destroy(&attr);
}


MutexImpl::MutexImpl(bool fast) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
#if defined(PTHREAD_MUTEX_RECURSIVE_NP)
	pthread_mutexattr_settype_np(&attr, fast ? PTHREAD_MUTEX_NORMAL_NP : PTHREAD_MUTEX_RECURSIVE_NP);
#elif !defined(POCO_VXWORKS)
	pthread_mutexattr_settype(&attr, fast ? PTHREAD_MUTEX_NORMAL : PTHREAD_MUTEX_RECURSIVE);
#endif
	if (pthread_mutex_init(&_mutex, &attr)) {
		pthread_mutexattr_destroy(&attr);
		//throw SystemException("cannot create mutex");
		// TODO: handle error.
	}
	
	pthread_mutexattr_destroy(&attr);
}


MutexImpl::~MutexImpl() {
	pthread_mutex_destroy(&_mutex);
}


bool MutexImpl::tryLockImpl(long milliseconds) {
	// FIXME: expand into fully timed lock.
	//if (!_mutex.try_lock()) { return false; }
	
	//return true;
	
#if defined(POCO_HAVE_MUTEX_TIMEOUT)
	struct timespec abstime;
#if defined(POCO_HAVE_CLOCK_GETTIME)
	clock_gettime(CLOCK_REALTIME, &abstime);
	abstime.tv_sec  += milliseconds / 1000;
	abstime.tv_nsec += (milliseconds % 1000)*1000000;
	if (abstime.tv_nsec >= 1000000000) {
		abstime.tv_nsec -= 1000000000;
		abstime.tv_sec++;
	}
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	abstime.tv_sec  = tv.tv_sec + milliseconds / 1000;
	abstime.tv_nsec = tv.tv_usec*1000 + (milliseconds % 1000)*1000000;
	if (abstime.tv_nsec >= 1000000000) {
		abstime.tv_nsec -= 1000000000;
		abstime.tv_sec++;
	}
#endif
	int rc = pthread_mutex_timedlock(&_mutex, &abstime);
	if (rc == 0) 				{ return true; }
	else if (rc == ETIMEDOUT) 	{ return false; }
	else						{ return false; }
#else
	const int sleepMillis = 5;
	Timestamp now;
	Timestamp::TimeDiff diff(Timestamp::TimeDiff(milliseconds)*1000);
	do {
		int rc = pthread_mutex_trylock(&_mutex);
		if (rc == 0) 			{ return true; }
		else if (rc != EBUSY) 	{ return false; }

		struct timeval tv;
		tv.tv_sec  = 0;
		tv.tv_usec = sleepMillis * 1000;
		select(0, NULL, NULL, NULL, &tv);
	}
	while (!now.isElapsed(diff));
	return false;
#endif
}


FastMutexImpl::FastMutexImpl(): MutexImpl(true) { }


FastMutexImpl::~FastMutexImpl() { }


} // namespace Poco
