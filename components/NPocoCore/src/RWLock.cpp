/*
	RWLock.cpp - 
*/


#include "RWLock.h"

#if defined(POCO_OS_FAMILY_FREERTOS)
#include "platforms/RWLock_FreeRTOS.cpp"
#elif defined(POCO_OS_FAMILY_WINDOWS)
#if defined(_WIN32_WCE)
#include "platforms/RWLock_WINCE.cpp"
#else
#include "platforms/RWLock_WIN32.cpp"
#endif
#elif POCO_OS == POCO_OS_ANDROID
#include "platforms/RWLock_Android.cpp"
#elif defined(POCO_VXWORKS)
#include "platforms/RWLock_VX.cpp"
#else
#include "platforms/RWLock_POSIX.cpp"
#endif


namespace Poco {


RWLock::RWLock()
{
}

	
RWLock::~RWLock()
{
}


} // namespace Poco
