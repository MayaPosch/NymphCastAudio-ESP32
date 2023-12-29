/*
	BasicEvent.h - Implementation of the BasicEvent template.
*/


#ifndef Core_BasicEvent_INCLUDED
#define Core_BasicEvent_INCLUDED


#include "AbstractEvent.h"
#include "DefaultStrategy.h"
#include "AbstractDelegate.h"
#include "Mutex.h"


namespace Poco {


template <class TArgs, class TMutex = FastMutex> 
class BasicEvent: public AbstractEvent < 
	TArgs, DefaultStrategy<TArgs, AbstractDelegate<TArgs>>,
	AbstractDelegate<TArgs>,
	TMutex
>
	/// A BasicEvent uses the DefaultStrategy which 
	/// invokes delegates in the order they have been registered.
	///
	/// Please see the AbstractEvent class template documentation
	/// for more information.
{
public:
	BasicEvent()
	{
	}

	~BasicEvent()
	{
	}

private:
	BasicEvent(const BasicEvent& e);
	BasicEvent& operator = (const BasicEvent& e);
};


} // namespace Poco


#endif // Core_BasicEvent_INCLUDED
