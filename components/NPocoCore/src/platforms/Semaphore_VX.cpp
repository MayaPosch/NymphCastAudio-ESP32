/*
	Semaphore_VX.cpp - 
*/


#include "Semaphore_VX.h"
#include <sysLib.h>


namespace Poco {

SemaphoreImpl::SemaphoreImpl(int n, int max) {
	poco_assert (n >= 0 && max > 0 && n <= max);

	_sem = semCCreate(SEM_Q_PRIORITY, n);
	if (_sem == 0) {
		//throw Poco::SystemException("cannot create semaphore");
		// TODO: Handle error.
	}
}


SemaphoreImpl::~SemaphoreImpl() {
	semDelete(_sem);
}


bool SemaphoreImpl::waitImpl() {
	if (semTake(_sem, WAIT_FOREVER) != OK) { return false; }
	return true;
}


bool SemaphoreImpl::waitImpl(long milliseconds) {
	int ticks = milliseconds*sysClkRateGet()/1000;
	return semTake(_sem, ticks) == OK;
}

} // namespace Poco
