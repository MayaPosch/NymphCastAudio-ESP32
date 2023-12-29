/*
	NamedEvent_Android.h - Definition of the NamedEventImpl class for Android.
*/


#ifndef Core_NamedEvent_Android_INCLUDED
#define Core_NamedEvent_Android_INCLUDED


#include "../Core.h"


namespace Poco {


class Foundation_API NamedEventImpl
{
protected:
	NamedEventImpl(const std::string& name);	
	~NamedEventImpl();
	bool setImpl();
	bool waitImpl();
};


} // namespace Poco


#endif // Core_NamedEvent_Android_INCLUDED
