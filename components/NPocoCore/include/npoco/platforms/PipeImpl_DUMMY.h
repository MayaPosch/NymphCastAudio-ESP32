/*
	PipeImpl_DUMMY.h - Definition of the PipeImpl_DUMMY class.
*/


#ifndef Core_PipeImpl_DUMMY_INCLUDED
#define Core_PipeImpl_DUMMY_INCLUDED


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
};


} // namespace Poco


#endif // Core_PipeImpl_DUMMY_INCLUDED
