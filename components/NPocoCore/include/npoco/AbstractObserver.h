/*
	AbstractObserver.h - Definition of the AbstractObserver class.
*/


#ifndef Core_AbstractObserver_INCLUDED
#define Core_AbstractObserver_INCLUDED


#include "Core.h"
#include "Notification.h"


namespace Poco {


class Foundation_API AbstractObserver
	/// The base class for all instantiations of
	/// the Observer and NObserver template classes.
{
public:
	AbstractObserver();
	AbstractObserver(const AbstractObserver& observer);
	virtual ~AbstractObserver();
	
	AbstractObserver& operator = (const AbstractObserver& observer);

	virtual void notify(Notification* pNf) const = 0;
	virtual bool equals(const AbstractObserver& observer) const = 0;
	virtual bool accepts(Notification* pNf) const = 0;
	virtual AbstractObserver* clone() const = 0;
	virtual void disable() = 0;
};


} // namespace Poco


#endif // Core_AbstractObserver_INCLUDED
