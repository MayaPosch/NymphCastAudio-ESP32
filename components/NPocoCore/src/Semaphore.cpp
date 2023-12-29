/*
	Semaphore.cpp - 
*/


#include "Semaphore.h"


#if defined(POCO_OS_FAMILY_WINDOWS)
#include "platforms/Semaphore_WIN32.cpp"
#elif defined(POCO_VXWORKS)
#include "platforms/Semaphore_VX.cpp"
#elif defined(POCO_OS_FAMILY_FREERTOS)
#include "platforms/Semaphore_FreeRTOS.cpp"
#else
#include "platforms/Semaphore_POSIX.cpp"
#endif


namespace Poco {

Semaphore::Semaphore(int n): SemaphoreImpl(n, n) { }


Semaphore::Semaphore(int n, int max): SemaphoreImpl(n, max) { }


Semaphore::~Semaphore() { }


} // namespace Poco
