/*
	Event.cpp - 
*/


#include "Event.h"


#if defined(POCO_OS_FAMILY_WINDOWS)
#include "platforms/Event_WIN32.cpp"
#elif defined(POCO_VXWORKS)
#include "platforms/Event_VX.cpp"
#else
#include "platforms/Event_POSIX.cpp"
#endif


namespace Poco {


Event::Event(EventType type): EventImpl(type == EVENT_AUTORESET) { }


Event::Event(bool autoReset): EventImpl(autoReset) { }


Event::~Event() { }


} // namespace Poco
