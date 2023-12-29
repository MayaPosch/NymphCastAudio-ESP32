/*
	Semaphore_VX.h - Definition of the SemaphoreImpl class for VxWorks.
*/


#ifndef Core_Semaphore_VX_INCLUDED
#define Core_Semaphore_VX_INCLUDED


#include "../Core.h"
#include <semLib.h>


namespace Poco {

class Foundation_API SemaphoreImpl {
protected:
	SemaphoreImpl(int n, int max);		
	~SemaphoreImpl();
	bool setImpl();
	bool waitImpl();
	bool waitImpl(long milliseconds);
	
private:
	SEM_ID _sem;
};


// inlines
inline bool SemaphoreImpl::setImpl() {
	if (semGive(_sem) != OK) {
		return false;
	}
	
	return true;
}

} // namespace Poco


#endif // Core_Semaphore_VX_INCLUDED
