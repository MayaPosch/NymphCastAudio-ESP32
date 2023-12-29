/*
	Event_WIN32.h - Definition of the EventImpl class for WIN32.
*/


#ifndef Core_Event_WIN32_INCLUDED
#define Core_Event_WIN32_INCLUDED


#include "../Core.h"
#include "UnWindows.h"


namespace Poco {


class Foundation_API EventImpl
{
protected:
	EventImpl(bool autoReset);		
	~EventImpl();
	bool setImpl();
	bool waitImpl();
	bool waitImpl(long milliseconds);
	bool resetImpl();
	
private:
	HANDLE _event;
};


// inlines
inline bool EventImpl::setImpl() {
	if (!SetEvent(_event)) {
		return false;
	}
	
	return true;
}


inline bool EventImpl::resetImpl() {
	if (!ResetEvent(_event)) {
		return false;
	}
	
	return true;
}


} // namespace Poco


#endif // Core_Event_WIN32_INCLUDED
