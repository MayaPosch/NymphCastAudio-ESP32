/*
	Notification.h - Definition of the Notification class.
*/


#ifndef Core_Notification_INCLUDED
#define Core_Notification_INCLUDED


#include "Core.h"
#include "Mutex.h"
#include "RefCountedObject.h"
#include "AutoPtr.h"


namespace Poco {


class Foundation_API Notification: public RefCountedObject
	/// The base class for all notification classes used
	/// with the NotificationCenter and the NotificationQueue
	/// classes.
	/// The Notification class can be used with the AutoPtr
	/// template class.
{
public:
	using Ptr = AutoPtr<Notification>;
	
	Notification();
		/// Creates the notification.

	virtual std::string name() const;
		/// Returns the name of the notification.
		/// The default implementation returns the class name.

protected:
	virtual ~Notification();
};


} // namespace Poco


#endif // Foundation_Notification_INCLUDED
