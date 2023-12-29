/*
	Semaphore_WIN32.h - Definition of the SemaphoreImpl class for WIN32.
*/


#ifndef Core_Semaphore_WIN32_INCLUDED
#define Core_Semaphore_WIN32_INCLUDED


#include "../Core.h"
#include "UnWindows.h"


namespace Poco {

class Foundation_API SemaphoreImpl {
protected:
	SemaphoreImpl(int n, int max);		
	~SemaphoreImpl();
	bool setImpl();
	bool waitImpl();
	bool waitImpl(long milliseconds);
	
private:
	HANDLE _sema;
};


//
// inlines
//
inline bool SemaphoreImpl::setImpl() {
	if (!ReleaseSemaphore(_sema, 1, NULL)) {
		//throw SystemException("cannot signal semaphore");
		return false;
	}
	
	return true;
}


} // namespace Poco


#endif // Core_Semaphore_WIN32_INCLUDED
