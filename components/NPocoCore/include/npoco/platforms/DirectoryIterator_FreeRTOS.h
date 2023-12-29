/*
	DirectoryIterator_FreeRTOS.h - Definition of the DirectoryIteratorImpl class for UNIX.
*/


#ifndef Core_DirectoryIterator_FreeRTOS_INCLUDED
#define Core_DirectoryIterator_FreeRTOS_INCLUDED


#include "../Core.h"
//#include <dirent.h>


namespace Poco {

class Foundation_API DirectoryIteratorImpl {
public:
	DirectoryIteratorImpl(const std::string& path);
	~DirectoryIteratorImpl();
	
	void duplicate();
	void release();
	
	const std::string& get() const;
	const std::string& next();
	
private:
	//DIR*        _pDir;
	std::string _current;
	int _rc;
};


//
// inlines
//
const std::string& DirectoryIteratorImpl::get() const
{
	return _current;
}


inline void DirectoryIteratorImpl::duplicate()
{
	++_rc;
}


inline void DirectoryIteratorImpl::release()
{
	if (--_rc == 0)
		delete this;
}


} // namespace Poco


#endif // Core_DirectoryIterator_FreeRTOS_INCLUDED
