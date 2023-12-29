/*
	ActiveStarter.h - Definition of the ActiveStarter class.
*/


#ifndef Core_ActiveStarter_INCLUDED
#define Core_ActiveStarter_INCLUDED


#include "Core.h"
#include "ThreadPool.h"
#include "ActiveRunnable.h"


namespace Poco {


template <class OwnerType>
class ActiveStarter
	/// The default implementation of the StarterType 
	/// policy for ActiveMethod. It starts the method
	/// in its own thread, obtained from the default
	/// thread pool.
{
public:
	static void start(OwnerType* /*pOwner*/, ActiveRunnableBase::Ptr pRunnable)
	{
		ThreadPool::defaultPool().start(*pRunnable);
		pRunnable->duplicate(); // The runnable will release itself.
	}
};


} // namespace Poco


#endif // Core_ActiveStarter_INCLUDED
