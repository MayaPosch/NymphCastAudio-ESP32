/*
	Thread.cpp -
*/


#include "Thread.h"
#include "Mutex.h"
#include "ThreadLocal.h"
#include "AtomicCounter.h"
#include <sstream>


#if defined(POCO_OS_FAMILY_FREERTOS)
#include "platforms/Thread_FreeRTOS.cpp"
#elif defined(POCO_OS_FAMILY_WINDOWS)
#if defined(_WIN32_WCE)
#include "platforms/Thread_WINCE.cpp"
#else
#include "platforms/Thread_WIN32.cpp"
#endif
#elif defined(POCO_VXWORKS)
#include "platforms/Thread_VX.cpp"
#else
#include "platforms/Thread_POSIX.cpp"
#endif


namespace Poco {

namespace {

class RunnableHolder: public Runnable {
public:
	RunnableHolder(Runnable& target): _target(target) { }

	~RunnableHolder() { }

	void run() {
		_target.run();
	}

private:
	Runnable& _target;
};


class CallableHolder: public Runnable {
public:
	CallableHolder(Thread::Callable callable, void* pData):
		_callable(callable),
		_pData(pData)
	{
	}

	~CallableHolder() { }

	void run() {
		_callable(_pData);
	}

private:
	Thread::Callable _callable;
	void* _pData;
};


} // namespace


Thread::Thread():
	_id(uniqueId()),
	_name(makeName()),
	_pTLS(0),
	_event(true)
{
}


Thread::Thread(const std::string& name):
	_id(uniqueId()),
	_name(name),
	_pTLS(0),
	_event(true)
{
}


Thread::~Thread() {
	delete _pTLS;
}


bool Thread::setPriority(Priority prio) {
	return setPriorityImpl(prio);
}


Thread::Priority Thread::getPriority() const {
	return Priority(getPriorityImpl());
}


bool Thread::start(Runnable& target) {
	return startImpl(new RunnableHolder(target));
}


bool Thread::start(Poco::SharedPtr<Runnable> pTarget) {
	return startImpl(pTarget);
}


bool Thread::start(Callable target, void* pData) {
	return startImpl(new CallableHolder(target, pData));
}


bool Thread::join() {
	return joinImpl();
}


bool Thread::join(long milliseconds) {
	return joinImpl(milliseconds);
}


bool Thread::tryJoin(long milliseconds)
{
	return joinImpl(milliseconds);
}


bool Thread::trySleep(long milliseconds)
{
	Thread* pT = Thread::current();
	poco_check_ptr(pT);
	return !(pT->_event.tryWait(milliseconds));
}


void Thread::wakeUp() {
	_event.set();
}


ThreadLocalStorage& Thread::tls()
{
	if (!_pTLS)
		_pTLS = new ThreadLocalStorage;
	return *_pTLS;
}


void Thread::clearTLS()
{
	if (_pTLS)
	{
		delete _pTLS;
		_pTLS = 0;
	}
}


std::string Thread::makeName()
{
	std::ostringstream name;
	name << '#' << _id;
	return name.str();
}


int Thread::uniqueId()
{
	static Poco::AtomicCounter counter;
	return ++counter;
}


void Thread::setName(const std::string& name)
{
	FastMutex::ScopedLock lock(_mutex);

	_name = name;
}


} // namespace Poco
