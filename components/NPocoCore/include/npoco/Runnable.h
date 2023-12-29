/*
	Runnable.h - Definition of the Runnable class.
*/


#ifndef Core_Runnable_INCLUDED
#define Core_Runnable_INCLUDED


#include "Core.h"


namespace Poco {

/// The Runnable interface with the run() method
/// must be implemented by classes that provide
/// an entry point for a thread.
class Foundation_API Runnable {
public:	
	Runnable();
	virtual ~Runnable();
	
	/// Do whatever the thread needs to do. Must
	/// be overridden by subclasses.
	virtual void run() = 0;
};

} // namespace Poco


#endif // Foundation_Runnable_INCLUDED
