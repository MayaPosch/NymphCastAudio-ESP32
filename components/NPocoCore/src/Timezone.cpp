/*
	Timezone.cpp - 
*/


#include "Timezone.h"
#include <ctime>


#if defined(POCO_OS_FAMILY_FREERTOS)
#include "platforms/Timezone_FreeRTOS.cpp"
#elif defined(POCO_OS_FAMILY_WINDOWS)
#if defined(_WIN32_WCE)
#include "platforms/Timezone_WINCE.cpp"
#else
#include "platforms/Timezone_WIN32.cpp"
#endif
#elif defined(POCO_VXWORKS)
#include "platforms/Timezone_VX.cpp"
#else
#include "platforms/Timezone_UNIX.cpp"
#endif


namespace Poco {


int Timezone::tzd()
{
	return utcOffset() + dst();
}


} // namespace Poco
