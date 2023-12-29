/*
	Thread_WINCE.h - 
*/


#include "Thread_WINCE.h"
#include "../ErrorHandler.h"


namespace Poco {


ThreadImpl::CurrentThreadHolder ThreadImpl::_currentThreadHolder;


ThreadImpl::ThreadImpl():
	_pRunnableTarget(0),
	_thread(0),
	_threadId(0),
	_prio(PRIO_NORMAL_IMPL),
	_stackSize(POCO_THREAD_STACK_SIZE)
{
}

			
ThreadImpl::~ThreadImpl() {
	if (_thread) CloseHandle(_thread);
}


bool ThreadImpl::setPriorityImpl(int prio) {
	if (prio != _prio) {
		_prio = prio;
		if (_thread) {
			if (SetThreadPriority(_thread, _prio) == 0)
				return false;
		}
	}
	
	return true;
}


bool ThreadImpl::setOSPriorityImpl(int prio, int /* policy */) {
	return setPriorityImpl(prio);
}


bool ThreadImpl::startImpl(SharedPtr<Runnable> pTarget) {
	if (isRunningImpl()) {
		return false;
	}

	_pRunnableTarget = pTarget;

	return createImpl(runnableEntry, this);
}


bool ThreadImpl::createImpl(Entry ent, void* pData) {
	_thread = CreateThread(NULL, _stackSize, ent, pData, 0, &_threadId);

	if (!_thread) {
		//throw SystemException("cannot create thread");
		return false;
	}
	
	if (_prio != PRIO_NORMAL_IMPL && !SetThreadPriority(_thread, _prio)) {
		//throw SystemException("cannot set thread priority");
		return false;
	}
	
	return true;
}


bool ThreadImpl::joinImpl() {
	if (!_thread) { return true; }

	switch (WaitForSingleObject(_thread, INFINITE)) {
	case WAIT_OBJECT_0:
		threadCleanup();
		return true;
	default:
		return false;
	}
}


bool ThreadImpl::joinImpl(long milliseconds) {
	if (!_thread) { return true; }

	switch (WaitForSingleObject(_thread, milliseconds + 1)) {
	case WAIT_TIMEOUT:
		return false;
	case WAIT_OBJECT_0:
		threadCleanup();
		return true;
	default:
		return false;
	}
}


bool ThreadImpl::isRunningImpl() const {
	if (_thread) {
		DWORD ec = 0;
		return GetExitCodeThread(_thread, &ec) && ec == STILL_ACTIVE;
	}
	
	return false;
}


void ThreadImpl::threadCleanup() {
	if (!_thread) return;
	if (CloseHandle(_thread)) _thread = 0;
}


ThreadImpl* ThreadImpl::currentImpl() {
	return _currentThreadHolder.get();
}


ThreadImpl::TIDImpl ThreadImpl::currentTidImpl() {
    return GetCurrentThreadId();
}


DWORD WINAPI ThreadImpl::runnableEntry(LPVOID pThread) {
	_currentThreadHolder.set(reinterpret_cast<ThreadImpl*>(pThread));
	try
	{
		reinterpret_cast<ThreadImpl*>(pThread)->_pRunnableTarget->run();
	}
	catch (Exception& exc)
	{
		ErrorHandler::handle(exc);
	}
	catch (std::exception& exc)
	{
		ErrorHandler::handle(exc);
	}
	catch (...)
	{
		ErrorHandler::handle();
	}
	return 0;
}


} // namespace Poco
