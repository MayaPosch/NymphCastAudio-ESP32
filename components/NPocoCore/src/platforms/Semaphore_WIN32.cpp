/*
	Semaphore_WIN32.cpp - 
*/


#include "Semaphore_WIN32.h"


namespace Poco {

SemaphoreImpl::SemaphoreImpl(int n, int max) {
	poco_assert (n >= 0 && max > 0 && n <= max);

	_sema = CreateSemaphoreW(NULL, n, max, NULL); 
	if (!_sema) {
		//throw SystemException("cannot create semaphore");
		// TODO: Handle error.
	}
}


SemaphoreImpl::~SemaphoreImpl() {
	CloseHandle(_sema);
}


bool SemaphoreImpl::waitImpl() {
	switch (WaitForSingleObject(_sema, INFINITE)) {
	case WAIT_OBJECT_0:
		return true;
	default:
		return false;
	}
}


bool SemaphoreImpl::waitImpl(long milliseconds) {
	switch (WaitForSingleObject(_sema, milliseconds + 1)) {
	case WAIT_TIMEOUT:
		return false;
	case WAIT_OBJECT_0:
		return true;
	default:
		return false;
	}
}

} // namespace Poco
