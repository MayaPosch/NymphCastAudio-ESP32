/*
	AbstractDelegate.h - Implementation of the AbstractDelegate template.
*/


#ifndef Core_AbstractDelegate_INCLUDED
#define Core_AbstractDelegate_INCLUDED


#include "Core.h"


namespace Poco {


template <class TArgs> 
class AbstractDelegate
	/// Base class for Delegate and Expire.
{
public:
	AbstractDelegate()
	{
	}

	AbstractDelegate(const AbstractDelegate& /*del*/)
	{
	}

	virtual ~AbstractDelegate() 
	{
	}

	virtual bool notify(const void* sender, TArgs& arguments) = 0;
		/// Invokes the delegate's callback function.
		/// Returns true if successful, or false if the delegate
		/// has been disabled or has expired.

	virtual bool equals(const AbstractDelegate& other) const = 0;
		/// Compares the AbstractDelegate with the other one for equality.

	virtual AbstractDelegate* clone() const = 0;
		/// Returns a deep copy of the AbstractDelegate.

	virtual void disable() = 0;
		/// Disables the delegate, which is done prior to removal.
		
	virtual const AbstractDelegate* unwrap() const
		/// Returns the unwrapped delegate. Must be overridden by decorators
		/// like Expire.
	{
		return this;
	}
};


template <> 
class AbstractDelegate<void>
	/// Base class for Delegate and Expire.
{
public:
	AbstractDelegate()
	{
	}

	AbstractDelegate(const AbstractDelegate&)
	{
	}

	virtual ~AbstractDelegate() 
	{
	}

	virtual bool notify(const void* sender) = 0;
		/// Invokes the delegate's callback function.
		/// Returns true if successful, or false if the delegate
		/// has been disabled or has expired.

	virtual bool equals(const AbstractDelegate& other) const = 0;
		/// Compares the AbstractDelegate with the other one for equality.

	virtual AbstractDelegate* clone() const = 0;
		/// Returns a deep copy of the AbstractDelegate.

	virtual void disable() = 0;
		/// Disables the delegate, which is done prior to removal.
		
	virtual const AbstractDelegate* unwrap() const
		/// Returns the unwrapped delegate. Must be overridden by decorators
		/// like Expire.
	{
		return this;
	}
};


} // namespace Poco


#endif // Core_AbstractDelegate_INCLUDED
