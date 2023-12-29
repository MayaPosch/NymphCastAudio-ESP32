//
// Mutex.cpp
//
// Library: Foundation
// Package: Threading
// Module:  Mutex
//
// Copyright (c) 2004-2008, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Mutex.h"


#if defined(POCO_OS_FAMILY_WINDOWS)
#if defined(_WIN32_WCE)
#include "platforms/Mutex_WINCE.cpp"
#else
#include "platforms/Mutex_WIN32.cpp"
#endif
#elif defined(POCO_VXWORKS)
#include "platforms/Mutex_VX.cpp"
#elif defined(POCO_OS_FAMILY_FREERTOS)
#include "platforms/Mutex_FreeRTOS.cpp"
#else
#include "platforms/Mutex_POSIX.cpp"
#endif


namespace Poco {


Mutex::Mutex()
{
}


Mutex::~Mutex()
{
}


FastMutex::FastMutex()
{
}


FastMutex::~FastMutex()
{
}


#ifdef POCO_HAVE_STD_ATOMICS

SpinlockMutex::SpinlockMutex()
{
}


SpinlockMutex::~SpinlockMutex()
{
}

#endif // POCO_HAVE_STD_ATOMICS


} // namespace Poco
