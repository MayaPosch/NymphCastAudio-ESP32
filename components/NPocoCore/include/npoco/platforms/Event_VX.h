/*
	Event_VX.h - Definition of the EventImpl class for VxWorks.
*/


#ifndef Core_Event_VX_INCLUDED
#define Core_Event_VX_INCLUDED


#include "../Core.h"
#include <semLib.h>


namespace Poco {


class Foundation_API EventImpl {
protected:
	EventImpl(bool autoReset);		
	~EventImpl();
	bool setImpl();
	bool waitImpl();
	bool waitImpl(long milliseconds);
	bool resetImpl();
	
private:
	bool          _auto;
	volatile bool _state;
	SEM_ID        _sem;
};


} // namespace Poco


#endif // Core_Event_VX_INCLUDED
