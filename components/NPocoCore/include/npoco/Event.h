/*
	Event.h - Definition of the Event class.
*/


#ifndef Core_Event_INCLUDED
#define Core_Event_INCLUDED


#include "Core.h"


#if defined(POCO_OS_FAMILY_WINDOWS)
#include "platforms/Event_WIN32.h"
#elif defined(POCO_VXWORKS)
#include "platforms/Event_VX.h"
#else
#include "platforms/Event_POSIX.h"
#endif


namespace Poco {

/// An Event is a synchronization object that
/// allows one thread to signal one or more
/// other threads that a certain event
/// has happened.
/// Usually, one thread signals an event,
/// while one or more other threads wait
/// for an event to become signalled.
class Foundation_API Event: private EventImpl {
public:
	enum EventType {
		EVENT_MANUALRESET, /// Manual reset event
		EVENT_AUTORESET    /// Auto-reset event
	};

	/// Creates the event. If type is EVENT_AUTORESET,
	/// the event is automatically reset after
	/// a wait() successfully returns.
	explicit Event(EventType type = EVENT_AUTORESET);
		
	//@ deprecated
	/// Please use Event::Event(EventType) instead.
	explicit Event(bool autoReset);

	/// Destroys the event.
	~Event();

	/// Signals the event. If autoReset is true,
	/// only one thread waiting for the event 
	/// can resume execution.
	/// If autoReset is false, all waiting threads
	/// can resume execution.
	bool set();

	/// Waits for the event to become signalled.
	bool wait();

	/// Waits for the event to become signalled.
	/// Throws a TimeoutException if the event
	/// does not become signalled within the specified
	/// time interval.
	bool wait(long milliseconds);

	/// Waits for the event to become signalled.
	/// Returns true if the event
	/// became signalled within the specified
	/// time interval, false otherwise.
	bool tryWait(long milliseconds);

	/// Resets the event to unsignalled state.
	bool reset();
	
private:
	Event(const Event&);
	Event& operator = (const Event&);
};


// inlines
inline bool Event::set() {
	return setImpl();
}


inline bool Event::wait() {
	return waitImpl();
}


inline bool Event::wait(long milliseconds) {
	if (!waitImpl(milliseconds)) { return false; }
	return true;
}


inline bool Event::tryWait(long milliseconds)
{
	return waitImpl(milliseconds);
}


inline bool Event::reset() {
	return resetImpl();
}


} // namespace Poco


#endif // Core_Event_INCLUDED
