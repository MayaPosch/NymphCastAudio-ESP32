/*
	Observer.h - Definition of the Observer class template.
*/


#ifndef Core_Observer_INCLUDED
#define Core_Observer_INCLUDED


#include "Core.h"
#include "AbstractObserver.h"
#include "Mutex.h"


namespace Poco {


template <class C, class N>
class Observer: public AbstractObserver
	/// This template class implements an adapter that sits between
	/// a NotificationCenter and an object receiving notifications
	/// from it. It is quite similar in concept to the 
	/// RunnableAdapter, but provides some NotificationCenter
	/// specific additional methods.
	/// See the NotificationCenter class for information on how
	/// to use this template class.
	///
	/// Instead of the Observer class template, you might want to
	/// use the NObserver class template, which uses an AutoPtr to
	/// pass the Notification to the callback function, thus freeing
	/// you from memory management issues.
{
public:
	typedef void (C::*Callback)(N*);

	Observer(C& object, Callback method): 
		_pObject(&object), 
		_method(method)
	{
	}
	
	Observer(const Observer& observer):
		AbstractObserver(observer),
		_pObject(observer._pObject), 
		_method(observer._method)
	{
	}
	
	~Observer()
	{
	}
	
	Observer& operator = (const Observer& observer)
	{
		if (&observer != this)
		{
			_pObject = observer._pObject;
			_method  = observer._method;
		}
		return *this;
	}

	void notify(Notification* pNf) const
	{
		Poco::Mutex::ScopedLock lock(_mutex);

		if (_pObject)
		{
			N* pCastNf = dynamic_cast<N*>(pNf);
			if (pCastNf)
			{
				pCastNf->duplicate();
				(_pObject->*_method)(pCastNf);
			}
		}
	}
	
	bool equals(const AbstractObserver& abstractObserver) const
	{
		const Observer* pObs = dynamic_cast<const Observer*>(&abstractObserver);
		return pObs && pObs->_pObject == _pObject && pObs->_method == _method;
	}

	bool accepts(Notification* pNf) const
	{
		return dynamic_cast<N*>(pNf) != 0;
	}
	
	AbstractObserver* clone() const
	{
		return new Observer(*this);
	}
	
	void disable()
	{
		Poco::Mutex::ScopedLock lock(_mutex);
		
		_pObject = 0;
	}
	
private:
	Observer();

	C*       _pObject;
	Callback _method;
	mutable Poco::Mutex _mutex;
};


} // namespace Poco


#endif // Core_Observer_INCLUDED
