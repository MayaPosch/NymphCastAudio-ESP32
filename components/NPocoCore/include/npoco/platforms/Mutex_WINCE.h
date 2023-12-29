//
// Mutex_WIN32.h
//
// Library: Foundation
// Package: Threading
// Module:  Mutex
//
// Definition of the MutexImpl and FastMutexImpl classes for WIN32.
//
// Copyright (c) 2004-2010, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef Core_Mutex_WINCE_INCLUDED
#define Core_Mutex_WINCE_INCLUDED


#include "../Core.h"
#include "UnWindows.h"


namespace Poco {


class Foundation_API MutexImpl {
protected:
	MutexImpl();
	~MutexImpl();
	bool lockImpl();
	bool tryLockImpl();
	bool tryLockImpl(long milliseconds);
	void unlockImpl();
	
private:
	HANDLE _mutex;
};


typedef MutexImpl FastMutexImpl;


} // namespace Poco


#endif // Core_Mutex_WINCE_INCLUDED
