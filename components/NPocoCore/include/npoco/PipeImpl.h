/*
 PipeImpl.h - Definition of the PipeImpl class.
*/


#ifndef Core_PipeImpl_INCLUDED
#define Core_PipeImpl_INCLUDED


#include "Core.h"


#if defined(POCO_OS_FAMILY_WINDOWS)
#if defined(_WIN32_WCE)
#include "platforms/PipeImpl_DUMMY.h"
#else
#include "platforms/PipeImpl_WIN32.h"
#endif
#elif defined(POCO_OS_FAMILY_UNIX)
#include "platforms/PipeImpl_POSIX.h"
#else
#include "platforms/PipeImpl_DUMMY.h"
#endif


#endif // Core_PipeImpl_INCLUDED
