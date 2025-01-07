/*
	Thread_POSIX.cpp - 
*/

#include "pthread_create.h"

#include "pthread_create.cpp"

#include "Thread_FreeRTOS.h"
#include "../Thread.h"
#include "../ErrorHandler.h"
#include "../Timespan.h"
#include "../Timestamp.h"
#include <signal.h>
/* #if defined(__sun) && defined(__SVR4)
#	if !defined(__EXTENSIONS__)
#		define __EXTENSIONS__
#	endif
#endif
#if POCO_OS == POCO_OS_LINUX || POCO_OS == POCO_OS_ANDROID || POCO_OS == POCO_OS_MAC_OS_X || POCO_OS == POCO_OS_QNX
#	include <time.h>
#endif */

//
// Block SIGPIPE in main thread.
//
/* #if defined(POCO_OS_FAMILY_UNIX) && !defined(POCO_VXWORKS)
namespace { 
	class SignalBlocker {
	public:
		SignalBlocker() {
			sigset_t sset;
			sigemptyset(&sset);
			sigaddset(&sset, SIGPIPE);
			pthread_sigmask(SIG_BLOCK, &sset, 0);
		}
		
		~SignalBlocker() { }
	};

	static SignalBlocker signalBlocker;
}
#endif */


#if defined(POCO_POSIX_DEBUGGER_THREAD_NAMES)


namespace {
void setThreadName(pthread_t thread, const std::string& threadName) {
#if (POCO_OS == POCO_OS_MAC_OS_X)
	pthread_setname_np(threadName.c_str()); // __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2)
#else
	if (pthread_setname_np(thread, threadName.c_str()) == ERANGE && threadName.size() > 15) {
		std::string truncName(threadName, 0, 7);
		truncName.append("~");
		truncName.append(threadName, threadName.size() - 7, 7);
		pthread_setname_np(thread, truncName.c_str());
	}
#endif
}
}


#endif


namespace Poco {


ThreadImpl::CurrentThreadHolder ThreadImpl::_currentThreadHolder;


ThreadImpl::ThreadImpl(): _pData(new ThreadData) { }


ThreadImpl::~ThreadImpl() {
	if (_pData->started && !_pData->joined) {
		pthread_detach(_pData->thread);
		//_pData->thread->detach();
	}
}


bool ThreadImpl::setPriorityImpl(int prio) {
	/* if (prio != _pData->prio) {
		_pData->prio = prio;
		_pData->policy = SCHED_OTHER;
		if (isRunningImpl()) {
			struct sched_param par; struct MyStruct 
			{

			};
			
			par.sched_priority = mapPrio(_pData->prio, SCHED_OTHER);
			if (pthread_setschedparam(_pData->thread, SCHED_OTHER, &par)) {
				return false;
			}
		}
	} */
	
	return true;
}


bool ThreadImpl::setOSPriorityImpl(int prio, int policy) {
	/* if (prio != _pData->osPrio || policy != _pData->policy) {
		if (_pData->pRunnableTarget) {
			struct sched_param par;
			par.sched_priority = prio;
			if (pthread_setschedparam(_pData->thread, policy, &par)) {
				return false;
			}
		}
		
		_pData->prio   = reverseMapPrio(prio, policy);
		_pData->osPrio = prio;
		_pData->policy = policy;
	} */
	
	return true;
}


int ThreadImpl::getMinOSPriorityImpl(int policy) {
	return policy;
/* #if defined(POCO_THREAD_PRIORITY_MIN)
	return POCO_THREAD_PRIORITY_MIN;
#elif defined(__digital__)
	return PRI_OTHER_MIN;
#else
	return sched_get_priority_min(policy);
#endif */
}


int ThreadImpl::getMaxOSPriorityImpl(int policy) {
	return policy;
/* #if defined(POCO_THREAD_PRIORITY_MAX)
	return POCO_THREAD_PRIORITY_MAX;
#elif defined(__digital__)
	return PRI_OTHER_MAX;
#else
	return sched_get_priority_max(policy);
#endif */
}


void ThreadImpl::setStackSizeImpl(int size) {
/* #ifndef PTHREAD_STACK_MIN
	_pData->stackSize = 0;
#else */
	if (size != 0) {
/* #if defined(POCO_OS_FAMILY_BSD)
		// we must round up to a multiple of the memory page size
		const int STACK_PAGE_SIZE = 4096;
		size = ((size + STACK_PAGE_SIZE - 1)/STACK_PAGE_SIZE)*STACK_PAGE_SIZE;
#endif */
#ifdef PTHREAD_STACK_MIN
 		if (size < PTHREAD_STACK_MIN)
 			size = PTHREAD_STACK_MIN;
#endif
	}
	
 	_pData->stackSize = size;
//#endif
}


void ThreadImpl::setCoreIdImpl(int id) {
	_pData->coreId = id;
}


bool ThreadImpl::startImpl(SharedPtr<Runnable> pTarget) {
	if (_pData->pRunnableTarget) {
		return false;
	}

	pthread_attr_t attributes;
	pthread_attr_init(&attributes);

	if (_pData->stackSize != 0) {
		if (0 != pthread_attr_setstacksize(&attributes, _pData->stackSize)) {
			pthread_attr_destroy(&attributes);
			//throw SystemException("cannot set thread stack size");
			return false;
		}
	}
	
	// Set assigned thread name, if any.
	esp_pthread_cfg_t esp_pthread_cfg;
	esp_pthread_cfg = esp_pthread_get_default_config();
	std::string name = reinterpret_cast<Thread*>(this)->getName();
	esp_pthread_cfg.thread_name = name.c_str();
	if (esp_pthread_set_cfg(&esp_pthread_cfg) != ESP_OK) {
		// Error.
		return false;
	}
	/* std::string name = reinterpret_cast<Thread*>(this)->getName();
	if (!name.empty()) {
		pthread_attr_setname(&attributes, name.c_str());
	} */

	_pData->pRunnableTarget = pTarget;
	//_pData->thread = new std::thread(runnableEntry, this);
	if (!_extRam) {
		if (pthread_create(&_pData->thread, &attributes, runnableEntry, this)) {
			_pData->pRunnableTarget = 0;
			pthread_attr_destroy(&attributes);
			return false;
		}
	}
	else {
		if (pthread_create_esp32(&_pData->thread, &attributes, runnableEntry, this, name.c_str(),
									_pData->xTaskBuffer, _pData->xStack, _pData->coreId)) {
			_pData->pRunnableTarget = 0;
			pthread_attr_destroy(&attributes);
			return false;
		}
	}
	
	_pData->started = true;
	pthread_attr_destroy(&attributes);

	/* if (_pData->policy == SCHED_OTHER) {
		if (_pData->prio != PRIO_NORMAL_IMPL) {
			struct sched_param par;
			par.sched_priority = mapPrio(_pData->prio, SCHED_OTHER);
			if (pthread_setschedparam(_pData->thread, SCHED_OTHER, &par)) {
				//throw SystemException("cannot set thread priority");
				return false;
			}
		}
	}
	else {
		struct sched_param par;
		par.sched_priority = _pData->osPrio;
		if (pthread_setschedparam(_pData->thread, _pData->policy, &par)) {
			//throw SystemException("cannot set thread priority");
			return false;
		}
	} */
	
	return true;
}


bool ThreadImpl::joinImpl() {
	if (!_pData->started) { return true; }
	_pData->done.wait();
	void* result;
	if (!_extRam) {
		if (pthread_join(_pData->thread, &result)) {
			return false;
		}
	}
	else {
		// This task is not in the central list, we have to use a special approach here.
		/* if (pthread_join_extram(_pData->thread, &result, _pData->xTaskBuffer, _pData->xStack)) {
			return false;
		} */
		
		int milliseconds = 20;
		const TickType_t xDelay = milliseconds / portTICK_PERIOD_MS;
		vTaskDelay(xDelay);
	}
	
	//_pData->thread->join();
	
	_pData->joined = true;
	return true;
}


bool ThreadImpl::joinImpl(long milliseconds) {
	if (_pData->started && _pData->done.tryWait(milliseconds)) {
		void* result;
		if (pthread_join(_pData->thread, &result)) {
			return false;
		}
		
		//_pData->thread->join();
		
		_pData->joined = true;
		return true;
	}
	else if (_pData->started) 	{ return false; }
	else 						{ return true; }
}


ThreadImpl* ThreadImpl::currentImpl() {
	return _currentThreadHolder.get();
}


ThreadImpl::TIDImpl ThreadImpl::currentTidImpl() {
	return pthread_self();
	//return std::this_thread::get_id();
}


bool ThreadImpl::sleepImpl(long milliseconds) {
/* #if defined(__digital__)
		// This is specific to DECThreads
		struct timespec interval;
		interval.tv_sec  = milliseconds / 1000;
		interval.tv_nsec = (milliseconds % 1000)*1000000;
		pthread_delay_np(&interval);
#elif POCO_OS == POCO_OS_LINUX || POCO_OS == POCO_OS_ANDROID || POCO_OS == POCO_OS_MAC_OS_X || POCO_OS == POCO_OS_QNX || POCO_OS == POCO_OS_VXWORKS
	Poco::Timespan remainingTime(1000*Poco::Timespan::TimeDiff(milliseconds));
	int rc;
	do
	{
		struct timespec ts;
		ts.tv_sec  = (long) remainingTime.totalSeconds();
		ts.tv_nsec = (long) remainingTime.useconds()*1000;
		Poco::Timestamp start;
		rc = ::nanosleep(&ts, 0);
		if (rc < 0 && errno == EINTR)
		{
			Poco::Timestamp end;
			Poco::Timespan waited = start.elapsed();
			if (waited < remainingTime)
				remainingTime -= waited;
			else
				remainingTime = 0;
		}
	}
	while (remainingTime > 0 && rc < 0 && errno == EINTR);
	if (rc < 0 && remainingTime > 0) {
		//throw Poco::SystemException("Thread::sleep(): nanosleep() failed");
		return false;
	}
#else
	Poco::Timespan remainingTime(1000*Poco::Timespan::TimeDiff(milliseconds));
	int rc;
	do
	{
		struct timeval tv;
		tv.tv_sec  = (long) remainingTime.totalSeconds();
		tv.tv_usec = (long) remainingTime.useconds();
		Poco::Timestamp start;
		rc = ::select(0, NULL, NULL, NULL, &tv);
		if (rc < 0 && errno == EINTR)
		{
			Poco::Timestamp end;
			Poco::Timespan waited = start.elapsed();
			if (waited < remainingTime)
				remainingTime -= waited;
			else
				remainingTime = 0;
		}
	}
	while (remainingTime > 0 && rc < 0 && errno == EINTR);
	if (rc < 0 && remainingTime > 0) {
		//throw Poco::SystemException("Thread::sleep(): select() failed");
		return false;
	}
#endif */

	//std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
	
	const TickType_t xDelay = milliseconds / portTICK_PERIOD_MS;
	vTaskDelay(xDelay);

	return true;
}


void* ThreadImpl::runnableEntry(void* pThread) {
	_currentThreadHolder.set(reinterpret_cast<ThreadImpl*>(pThread));

#if defined(POCO_OS_FAMILY_UNIX)
	sigset_t sset;
	sigemptyset(&sset);
	sigaddset(&sset, SIGQUIT);
	sigaddset(&sset, SIGTERM);
	sigaddset(&sset, SIGPIPE);
	pthread_sigmask(SIG_BLOCK, &sset, 0);
#endif

	ThreadImpl* pThreadImpl = reinterpret_cast<ThreadImpl*>(pThread);
#if defined(POCO_POSIX_DEBUGGER_THREAD_NAMES)
	setThreadName(pThreadImpl->_pData->thread, reinterpret_cast<Thread*>(pThread)->getName());
#endif
	AutoPtr<ThreadData> pData = pThreadImpl->_pData;
	pData->pRunnableTarget->run();

	pData->pRunnableTarget = 0;
	pData->done.set();
	return 0;
}


/* int ThreadImpl::mapPrio(int prio, int policy) {
	int pmin = getMinOSPriorityImpl(policy);
	int pmax = getMaxOSPriorityImpl(policy);

	switch (prio)
	{
	case PRIO_LOWEST_IMPL:
		return pmin;
	case PRIO_LOW_IMPL:
		return pmin + (pmax - pmin)/4;
	case PRIO_NORMAL_IMPL:
		return pmin + (pmax - pmin)/2;
	case PRIO_HIGH_IMPL:
		return pmin + 3*(pmax - pmin)/4;
	case PRIO_HIGHEST_IMPL:
		return pmax;
	default:
		poco_bugcheck_msg("invalid thread priority");
	}
	
	return -1; // just to satisfy compiler - we'll never get here anyway
} */


/* int ThreadImpl::reverseMapPrio(int prio, int policy) {
	if (policy == SCHED_OTHER) {
		int pmin = getMinOSPriorityImpl(policy);
		int pmax = getMaxOSPriorityImpl(policy);
		int normal = pmin + (pmax - pmin)/2;
		if (prio == pmax)
			return PRIO_HIGHEST_IMPL;
		if (prio > normal)
			return PRIO_HIGH_IMPL;
		else if (prio == normal)
			return PRIO_NORMAL_IMPL;
		else if (prio > pmin)
			return PRIO_LOW_IMPL;
		else
			return PRIO_LOWEST_IMPL;
	}
	else return PRIO_HIGHEST_IMPL;
} */


} // namespace Poco
