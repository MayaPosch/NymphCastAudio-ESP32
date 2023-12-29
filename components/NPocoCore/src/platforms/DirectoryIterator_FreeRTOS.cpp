/*
	DirectoryIterator_FreeRTOS.cpp - 
*/


#include "DirectoryIterator_FreeRTOS.h"
/* #if defined(POCO_VXWORKS)
#include "File_VX.h"
#else */
#include "File_FreeRTOS.h"
//#endif
#include "../Path.h"


namespace Poco {


//DirectoryIteratorImpl::DirectoryIteratorImpl(const std::string& path): _pDir(0), _rc(1)
DirectoryIteratorImpl::DirectoryIteratorImpl(const std::string& path): _rc(1)
{
	/* Path p(path);
	p.makeFile();

#if defined(POCO_VXWORKS)
	_pDir = opendir(const_cast<char*>(p.toString().c_str()));
#else
	_pDir = opendir(p.toString().c_str());
#endif
	if (!_pDir) File::handleLastError(path);

	next(); */
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
	//if (_pDir) closedir(_pDir);
}


const std::string& DirectoryIteratorImpl::next()
{
	/* do
	{
		struct dirent* pEntry = readdir(_pDir);
		if (pEntry)
			_current = pEntry->d_name;
		else
			_current.clear();
	}
	while (_current == "." || _current == "..");*/
	return _current;
}


} // namespace Poco
