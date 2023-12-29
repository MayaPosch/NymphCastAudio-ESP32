/*
	Semaphore.h - Definition of the Semaphore class.
*/


#ifndef Core_Semaphore_INCLUDED
#define Core_Semaphore_INCLUDED


#include "Core.h"


#if defined(POCO_OS_FAMILY_WINDOWS)
#include "platforms/Semaphore_WIN32.h"
#elif defined(POCO_VXWORKS)
#include "platforms/Semaphore_VX.h"
#elif defined(POCO_OS_FAMILY_FREERTOS)
#include "platforms/Semaphore_FreeRTOS.h"
#else
#include "platforms/Semaphore_POSIX.h"
#endif


namespace Poco {

/// A Semaphore is a synchronization object with the following 
/// characteristics:
/// A semaphore has a value that is constrained to be a non-negative
/// integer and two atomic operations. The allowable operations are V 
/// (here called set()) and P (here called wait()). A V (set()) operation 
/// increases the value of the semaphore by one. 
/// A P (wait()) operation decreases the value of the semaphore by one, 
/// provided that can be done without violating the constraint that the 
/// value be non-negative. A P (wait()) operation that is initiated when 
/// the value of the semaphore is 0 suspends the calling thread. 
/// The calling thread may continue when the value becomes positive again.
class Foundation_API Semaphore: private SemaphoreImpl {
public:
	Semaphore(int n);
	Semaphore(int n, int max);
		/// Creates the semaphore. The current value
		/// of the semaphore is given in n. The
		/// maximum value of the semaphore is given
		/// in max.
		/// If only n is given, it must be greater than
		/// zero.
		/// If both n and max are given, max must be
		/// greater than zero, n must be greater than
		/// or equal to zero and less than or equal
		/// to max.
		
	~Semaphore();
		/// Destroys the semaphore.

	bool set();
		/// Increments the semaphore's value by one and
		/// thus signals the semaphore. Another thread
		/// waiting for the semaphore will be able
		/// to continue.

	bool wait();
		/// Waits for the semaphore to become signalled.
		/// To become signalled, a semaphore's value must
		/// be greater than zero. 
		/// Decrements the semaphore's value by one.

	bool wait(long milliseconds);
		/// Waits for the semaphore to become signalled.
		/// To become signalled, a semaphore's value must
		/// be greater than zero.
		/// Throws a TimeoutException if the semaphore
		/// does not become signalled within the specified
		/// time interval.
		/// Decrements the semaphore's value by one
		/// if successful.

	bool tryWait(long milliseconds);
		/// Waits for the semaphore to become signalled.
		/// To become signalled, a semaphore's value must
		/// be greater than zero.
		/// Returns true if the semaphore
		/// became signalled within the specified
		/// time interval, false otherwise.
		/// Decrements the semaphore's value by one
		/// if successful.
	
private:
	Semaphore();
	Semaphore(const Semaphore&);
	Semaphore& operator = (const Semaphore&);
};


// inlines
inline bool Semaphore::set() {
	return setImpl();
}


inline bool Semaphore::wait() {
	return waitImpl();
}


inline bool Semaphore::wait(long milliseconds) {
	return waitImpl(milliseconds);
}


inline bool Semaphore::tryWait(long milliseconds) {
	return waitImpl(milliseconds);
}


} // namespace Poco


#endif // Core_Semaphore_INCLUDED
