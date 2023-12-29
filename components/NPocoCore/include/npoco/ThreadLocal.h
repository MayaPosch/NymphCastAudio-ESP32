/*
	ThreadLocal.h - Definition of the ThreadLocal template and related classes.
*/


#ifndef Core_ThreadLocal_INCLUDED
#define Core_ThreadLocal_INCLUDED


#include "Core.h"
#include <map>


namespace Poco {

/// This is the base class for all objects
/// that the ThreadLocalStorage class manages.
class Foundation_API TLSAbstractSlot {
public:
	TLSAbstractSlot();
	virtual ~TLSAbstractSlot();
};


/// The Slot template wraps another class
/// so that it can be stored in a ThreadLocalStorage
/// object. This class is used internally, and you
/// must not create instances of it yourself.
template <class C>
class TLSSlot: public TLSAbstractSlot {
public:
	TLSSlot(): _value() { }
	
	~TLSSlot() { }
	
	C& value() {
		return _value;
	}
	
private:
	TLSSlot(const TLSSlot&);
	TLSSlot& operator = (const TLSSlot&);

	C _value;
};


/// This class manages the local storage for each thread.
/// Never use this class directly, always use the
/// ThreadLocal template for managing thread local storage.
class Foundation_API ThreadLocalStorage {
public:
	ThreadLocalStorage();
		/// Creates the TLS.
		
	~ThreadLocalStorage();
		/// Deletes the TLS.

	TLSAbstractSlot*& get(const void* key);
		/// Returns the slot for the given key.
		
	static ThreadLocalStorage& current();
		/// Returns the TLS object for the current thread
		/// (which may also be the main thread).
		
	static void clear();
		/// Clears the current thread's TLS object.
		/// Does nothing in the main thread.
	
private:
	typedef std::map<const void*, TLSAbstractSlot*> TLSMap;
	
	TLSMap _map;

	friend class Thread;
};


/// This template is used to declare type safe thread local variables. It can be used like
/// a smart pointer class with the special feature that it references a different object
/// in every thread. The underlying object will be created when it is referenced for the first
/// time.
/// See the NestedDiagnosticContext class for an
/// example how to use this template.
/// Every thread only has access to its own
/// thread local data. There is no way for a thread
/// to access another thread's local data.
template <class C>
class ThreadLocal {
	typedef TLSSlot<C> Slot;

public:
	ThreadLocal() { }
	
	~ThreadLocal() { }
	
	C* operator -> () {
		return &get();
	}
	
	/// "Dereferences" the smart pointer and returns a reference
	/// to the underlying data object. The reference can be used
	/// to modify the object.
	C& operator * () {
		return get();
	}

	/// Returns a reference to the underlying data object.
	/// The reference can be used to modify the object.
	C& get() {
		TLSAbstractSlot*& p = ThreadLocalStorage::current().get(this);
		if (!p) p = new Slot;
		return static_cast<Slot*>(p)->value();
	}
	
private:
	ThreadLocal(const ThreadLocal&);
	ThreadLocal& operator = (const ThreadLocal&);
};

} // namespace Poco


#endif // Core_ThreadLocal_INCLUDED
