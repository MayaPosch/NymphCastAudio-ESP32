/*
	PipeImpl_WIN32.h - Definition of the PipeImpl class for WIN32.
*/


#ifndef Core_PipeImpl_WIN32_INCLUDED
#define Core_PipeImpl_WIN32_INCLUDED


#include "../Core.h"
#include "../RefCountedObject.h"
#include "UnWindows.h"


namespace Poco {


class Foundation_API PipeImpl: public RefCountedObject
	/// A dummy implementation of PipeImpl for platforms
	/// that do not support pipes.
{
public:
	typedef HANDLE Handle;

	PipeImpl();
	~PipeImpl();
	int writeBytes(const void* buffer, int length);
	int readBytes(void* buffer, int length);
	Handle readHandle() const;
	Handle writeHandle() const;
	void closeRead();
	void closeWrite();
	
private:
	HANDLE _readHandle;
	HANDLE _writeHandle;
};


} // namespace Poco


#endif // Core_PipeImpl_WIN32_INCLUDED
