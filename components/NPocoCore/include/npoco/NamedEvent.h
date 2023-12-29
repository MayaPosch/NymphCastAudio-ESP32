/*
	NamedEvent.h - Definition of the NamedEvent class.
*/


#ifndef Core_NamedEvent_INCLUDED
#define Core_NamedEvent_INCLUDED


#include "Core.h"


#if defined(POCO_OS_FAMILY_WINDOWS) 
#include "platforms/NamedEvent_WIN32U.h"
#elif POCO_OS == POCO_OS_ANDROID
#include "platforms/NamedEvent_Android.h"
#elif defined(POCO_OS_FAMILY_UNIX)
#include "platforms/NamedEvent_UNIX.h"
#elif defined(POCO_OS_FAMILY_FREERTOS)
#include "platforms/NamedEvent_FreeRTOS.h"
#endif


namespace Poco {


class Foundation_API NamedEvent: public NamedEventImpl
	/// An NamedEvent is a global synchronization object
	/// that allows one process or thread to signal an
	/// other process or thread that a certain event
	/// has happened.
	///
	/// Unlike an Event, which itself is the unit of synchronization,
	/// a NamedEvent refers to a named operating system resource being the
	/// unit of synchronization.
	/// In other words, there can be multiple instances of NamedEvent referring
	/// to the same actual synchronization object.
	///
	/// NamedEvents are always autoresetting.
	///
	/// There should not be more than one instance of NamedEvent for
	/// a given name in a process. Otherwise, the instances may
	/// interfere with each other.
{
public:
	NamedEvent(const std::string& name);
		/// Creates the event.

	~NamedEvent();
		/// Destroys the event.

	bool set();
		/// Signals the event.
		/// The one thread or process waiting for the event
		/// can resume execution.

	bool wait();
		/// Waits for the event to become signalled.

private:
	NamedEvent();
	NamedEvent(const NamedEvent&);
	NamedEvent& operator = (const NamedEvent&);
};


//
// inlines
inline bool NamedEvent::set() {
	return setImpl();
}


inline bool NamedEvent::wait() {
	return waitImpl();
}


} // namespace Poco


#endif // Core_NamedEvent_INCLUDED
