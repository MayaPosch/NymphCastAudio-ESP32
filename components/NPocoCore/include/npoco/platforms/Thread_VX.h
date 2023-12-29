/*
	Thread_VX.h - Definition of the ThreadImpl class for VxWorks tasks.
*/


#ifndef Core_Thread_VX_INCLUDED
#define Core_Thread_VX_INCLUDED


#include "../Core.h"
#include "../Runnable.h"
#include "../SignalHandler.h"
#include "../Event.h"
#include "../RefCountedObject.h"
#include "../AutoPtr.h"
#include <taskLib.h>
#include <taskVarLib.h>


namespace Poco {


class Foundation_API ThreadImpl
{
public:	
	typedef int TIDImpl;
	typedef void (*Callable)(void*);

	enum Priority
	{
		PRIO_LOWEST_IMPL,
		PRIO_LOW_IMPL,
		PRIO_NORMAL_IMPL,
		PRIO_HIGH_IMPL,
		PRIO_HIGHEST_IMPL
	};

	enum Policy
	{
		POLICY_DEFAULT_IMPL = 0
	};

	enum
	{
		DEFAULT_THREAD_STACK_SIZE = 65536
	};

	struct CallbackData: public RefCountedObject
	{
		CallbackData(): callback(0), pData(0)
		{
		}

		Callable  callback;
		void*     pData; 
	};

	ThreadImpl();
	~ThreadImpl();

	TIDImpl tidImpl() const;
	bool setPriorityImpl(int prio);
	int getPriorityImpl() const;
	bool setOSPriorityImpl(int prio, int policy = 0);
	int getOSPriorityImpl() const;
	static int getMinOSPriorityImpl(int policy);
	static int getMaxOSPriorityImpl(int policy);
	void setStackSizeImpl(int size);
	int getStackSizeImpl() const;
	bool startImpl(Runnable& target);
	bool startImpl(Callable target, void* pData = 0);

	bool joinImpl();
	bool joinImpl(long milliseconds);
	bool isRunningImpl() const;
	static void sleepImpl(long milliseconds);
	static void yieldImpl();
	static ThreadImpl* currentImpl();
	static TIDImpl currentTidImpl();

protected:
	static void runnableEntry(void* pThread, int, int, int, int, int, int, int, int, int);
	static void callableEntry(void* pThread, int, int, int, int, int, int, int, int, int);
	static int mapPrio(int prio);
	static int reverseMapPrio(int osPrio);

	struct ThreadData: public RefCountedObject
	{
		ThreadData():
			pRunnableTarget(0),
			pCallbackTarget(0),
			task(0),
			prio(PRIO_NORMAL_IMPL),
			osPrio(127),
			done(false),
			stackSize(POCO_THREAD_STACK_SIZE)
		{
		}

		Runnable* pRunnableTarget;
		AutoPtr<CallbackData> pCallbackTarget;
		int       task;
		int       prio;
		int       osPrio;
		Event     done;
		int       stackSize;
	};

private:
	AutoPtr<ThreadData> _pData;
	static ThreadImpl* _pCurrent;
};


//
// inlines
//
inline int ThreadImpl::getPriorityImpl() const {
	return _pData->prio;
}


inline int ThreadImpl::getOSPriorityImpl() const {
	return _pData->osPrio;
}


inline bool ThreadImpl::isRunningImpl() const {
	return _pData->pRunnableTarget != 0 ||
		(_pData->pCallbackTarget.get() != 0 && _pData->pCallbackTarget->callback != 0);
}


inline void ThreadImpl::yieldImpl() {
	taskDelay(0);
}


inline int ThreadImpl::getStackSizeImpl() const {
	return _pData->stackSize;
}


inline ThreadImpl::TIDImpl ThreadImpl::tidImpl() const {
	return _pData->task;
}

} // namespace Poco


#endif // Core_Thread_VX_INCLUDED
