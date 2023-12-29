/*
	PipeImpl.cpp
*/


#include "PipeImpl.h"


#if defined(POCO_OS_FAMILY_WINDOWS)
#if defined(_WIN32_WCE)
#include "platforms/PipeImpl_DUMMY.cpp"
#else
#include "platforms/PipeImpl_WIN32.cpp"
#endif
#elif defined(POCO_OS_FAMILY_UNIX)
#include "platforms/PipeImpl_POSIX.cpp"
#else
#include "platforms/PipeImpl_DUMMY.cpp"
#endif
