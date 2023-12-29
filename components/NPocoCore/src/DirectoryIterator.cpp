/*
	DirectoryIterator.cpp - 
*/


#include "DirectoryIterator.h"


#if defined(POCO_OS_FAMILY_WINDOWS)
#include "platforms/DirectoryIterator_WIN32U.cpp"
#elif defined(POCO_OS_FAMILY_UNIX)
#include "platforms/DirectoryIterator_UNIX.cpp"
#elif defined(POCO_OS_FAMILY_FREERTOS)
#include "platforms/DirectoryIterator_FreeRTOS.cpp"
#endif


namespace Poco {


DirectoryIterator::DirectoryIterator(): _pImpl(0)
{
}


DirectoryIterator::DirectoryIterator(const std::string& path): _path(path), _pImpl(new DirectoryIteratorImpl(path))
{
	_path.makeDirectory();
	_path.setFileName(_pImpl->get());
	_file = _path;
}


DirectoryIterator::DirectoryIterator(const DirectoryIterator& iterator): _path(iterator._path), _pImpl(iterator._pImpl)
{
	if (_pImpl)
	{
		_pImpl->duplicate();
		_file = _path;
	}
}


DirectoryIterator::DirectoryIterator(const File& file): _path(file.path()), _pImpl(new DirectoryIteratorImpl(file.path()))
{
	_path.makeDirectory();
	_path.setFileName(_pImpl->get());
	_file = _path;
}


DirectoryIterator::DirectoryIterator(const Path& path): _path(path), _pImpl(new DirectoryIteratorImpl(path.toString()))
{
	_path.makeDirectory();
	_path.setFileName(_pImpl->get());
	_file = _path;
}


DirectoryIterator::~DirectoryIterator()
{
	if (_pImpl) _pImpl->release();
}


DirectoryIterator& DirectoryIterator::operator = (const DirectoryIterator& it)
{
	if (&it != this)
	{
		if (_pImpl) _pImpl->release();
		_pImpl = it._pImpl;
		if (_pImpl)
		{
			_pImpl->duplicate();
			_path = it._path;
			_file = _path;
		}
	}
	return *this;
}


DirectoryIterator& DirectoryIterator::operator = (const File& file)
{
	if (_pImpl) _pImpl->release();
	_pImpl = new DirectoryIteratorImpl(file.path());
	_path.parseDirectory(file.path());
	_path.setFileName(_pImpl->get());
	_file = _path;
	return *this;
}


DirectoryIterator& DirectoryIterator::operator = (const Path& path)
{
	if (_pImpl) _pImpl->release();
	_pImpl = new DirectoryIteratorImpl(path.toString());
	_path = path;
	_path.makeDirectory();
	_path.setFileName(_pImpl->get());
	_file = _path;
	return *this;
}


DirectoryIterator& DirectoryIterator::operator = (const std::string& path)
{
	if (_pImpl) _pImpl->release();
	_pImpl = new DirectoryIteratorImpl(path);
	_path.parseDirectory(path);
	_path.setFileName(_pImpl->get());
	_file = _path;
	return *this;
}


DirectoryIterator& DirectoryIterator::operator ++ ()
{
	if (_pImpl)
	{
		_path.setFileName(_pImpl->next());
		_file = _path;
	}
	return *this;
}


DirectoryIterator DirectoryIterator::operator ++ (int dummy)
{
	if (_pImpl)
	{
		_path.setFileName(_pImpl->next());
		_file = _path;
	}
	return *this;
}


} // namespace Poco
