/*
	Channel.h - Definition of the Channel class.
*/


#ifndef Core_Channel_INCLUDED
#define Core_Channel_INCLUDED


#include "Core.h"
#include "Configurable.h"
#include "Mutex.h"
#include "RefCountedObject.h"
#include "AutoPtr.h"


namespace Poco {


class Message;


class Foundation_API Channel: public Configurable, public RefCountedObject
	/// The base class for all Channel classes.
	///
	/// Supports reference counting based garbage
	/// collection and provides trivial implementations
	/// of getProperty() and setProperty().
{
public:
	using Ptr = AutoPtr<Channel>;

	Channel();
		/// Creates the channel and initializes
		/// the reference count to one.

	virtual void open();
		/// Does whatever is necessary to open the channel. 
		/// The default implementation does nothing.
		
	virtual void close();
		/// Does whatever is necessary to close the channel.
		/// The default implementation does nothing.
		
	virtual void log(const Message& msg) = 0;
		/// Logs the given message to the channel. Must be
		/// overridden by subclasses.
		///
		/// If the channel has not been opened yet, the log()
		/// method will open it.
		
	bool setProperty(const std::string& name, const std::string& value);
		/// Throws a PropertyNotSupportedException.

	std::string getProperty(const std::string& name) const;
		/// Throws a PropertyNotSupportedException.
		
protected:
	virtual ~Channel();
	
private:
	Channel(const Channel&);
	Channel& operator = (const Channel&);
};


} // namespace Poco


#endif // Core_Channel_INCLUDED
