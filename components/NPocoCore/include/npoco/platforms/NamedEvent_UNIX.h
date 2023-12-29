/*
	NamedEvent_UNIX.h - Definition of the NamedEventImpl class for Unix.
*/


#ifndef Core_NamedEvent_UNIX_INCLUDED
#define Core_NamedEvent_UNIX_INCLUDED


#include "../Core.h"
#if defined(sun) || defined(__APPLE__) || defined(__osf__) || defined(__QNX__) || defined(_AIX)
#include <semaphore.h>
#endif


namespace Poco {


class Foundation_API NamedEventImpl
{
protected:
	NamedEventImpl(const std::string& name);	
	~NamedEventImpl();
	bool setImpl();
	bool waitImpl();
	
private:
	std::string getFileName();

	std::string _name;
#if defined(sun) || defined(__APPLE__) || defined(__osf__) || defined(__QNX__) || defined(_AIX)
	sem_t* _sem;
#else
	int _semid;  // semaphore id
#endif
};


} // namespace Poco


#endif // Core_NamedEvent_UNIX_INCLUDED
