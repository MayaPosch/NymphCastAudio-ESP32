/*
	PipeImpl_POSIX.h - Definition of the PipeImpl class for POSIX.
*/


#ifndef Core_PipeImpl_POSIX_INCLUDED
#define Core_PipeImpl_POSIX_INCLUDED


#include "../Core.h"
#include "../RefCountedObject.h"


namespace Poco {


class Foundation_API PipeImpl: public RefCountedObject
	/// A dummy implementation of PipeImpl for platforms
	/// that do not support pipes.
{
public:
	typedef int Handle;

	PipeImpl();
	~PipeImpl();
	int writeBytes(const void* buffer, int length);
	int readBytes(void* buffer, int length);
	Handle readHandle() const;
	Handle writeHandle() const;
	void closeRead();
	void closeWrite();
	
private:
	int _readfd;
	int _writefd;
};


} // namespace Poco


#endif // Core_PipeImpl_POSIX_INCLUDED
