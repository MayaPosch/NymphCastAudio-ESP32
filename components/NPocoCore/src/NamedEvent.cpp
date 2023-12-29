/*
	NamedEvent.cpp 
*/


#include "NamedEvent.h"


#if defined(POCO_OS_FAMILY_WINDOWS)
#include "platforms/NamedEvent_WIN32U.cpp"
#elif POCO_OS == POCO_OS_ANDROID
#include "platforms/NamedEvent_Android.cpp"
#elif defined(POCO_OS_FAMILY_UNIX)
#include "platforms/NamedEvent_UNIX.cpp"
#elif defined(POCO_OS_FAMILY_FREERTOS)
#include "platforms/NamedEvent_FreeRTOS.cpp"
#endif


namespace Poco {


NamedEvent::NamedEvent(const std::string& name):
	NamedEventImpl(name)
{
}


NamedEvent::~NamedEvent()
{
}


} // namespace Poco
