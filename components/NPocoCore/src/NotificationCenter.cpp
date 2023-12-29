/*
	NotificationCenter.cpp
*/


#include "NotificationCenter.h"
#include "Notification.h"
#include "Observer.h"
#include "AutoPtr.h"
#include "SingletonHolder.h"


namespace Poco {


NotificationCenter::NotificationCenter()
{
}


NotificationCenter::~NotificationCenter()
{
}


void NotificationCenter::addObserver(const AbstractObserver& observer)
{
	Mutex::ScopedLock lock(_mutex);
	_observers.push_back(observer.clone());
}


void NotificationCenter::removeObserver(const AbstractObserver& observer)
{
	Mutex::ScopedLock lock(_mutex);
	for (ObserverList::iterator it = _observers.begin(); it != _observers.end(); ++it)
	{
		if (observer.equals(**it))
		{
			(*it)->disable();
			_observers.erase(it);
			return;
		}
	}
}


bool NotificationCenter::hasObserver(const AbstractObserver& observer) const
{
	Mutex::ScopedLock lock(_mutex);
	for (const auto& p: _observers)
		if (observer.equals(*p)) return true;

	return false;
}


void NotificationCenter::postNotification(Notification::Ptr pNotification)
{
	poco_check_ptr (pNotification);

	ScopedLockWithUnlock<Mutex> lock(_mutex);
	ObserverList observersToNotify(_observers);
	lock.unlock();
	for (auto& p: observersToNotify)
	{
		p->notify(pNotification);
	}
}


bool NotificationCenter::hasObservers() const
{
	Mutex::ScopedLock lock(_mutex);

	return !_observers.empty();
}


std::size_t NotificationCenter::countObservers() const
{
	Mutex::ScopedLock lock(_mutex);

	return _observers.size();
}


namespace
{
	static SingletonHolder<NotificationCenter> sh;
}


NotificationCenter& NotificationCenter::defaultCenter()
{
	return *sh.get();
}


} // namespace Poco
