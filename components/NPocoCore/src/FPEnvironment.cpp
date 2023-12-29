/*
	FPEnvironment.cpp - 
*/


// pull in platform identification macros needed below
#include "Platform.h"
#include "FPEnvironment.h"


#if defined(POCO_NO_FPENVIRONMENT)
#include "platforms/FPEnvironment_DUMMY.cpp"
#elif defined(__osf__)
#include "platforms/FPEnvironment_DEC.cpp"
#elif defined(sun) || defined(__sun)
#include "platforms/FPEnvironment_SUN.cpp"
#elif defined(__QNX__)
#include "platforms/FPEnvironment_QNX.cpp"
#elif defined(POCO_OS_FAMILY_UNIX)
#include "platforms/FPEnvironment_C99.cpp"
#elif defined(POCO_OS_FAMILY_WINDOWS)
#include "platforms/FPEnvironment_WIN32.cpp"
#else
#include "platforms/FPEnvironment_DUMMY.cpp"
#endif


namespace Poco {

FPEnvironment::FPEnvironment() { }


FPEnvironment::FPEnvironment(RoundingMode rm) {
	setRoundingMode(rm);
}


FPEnvironment::FPEnvironment(const FPEnvironment& env): FPEnvironmentImpl(env) { }


FPEnvironment::~FPEnvironment() { }


FPEnvironment& FPEnvironment::operator = (const FPEnvironment& env) {
	if (&env != this) 	{
		FPEnvironmentImpl::operator = (env);
	}
	
	return *this;
}


void FPEnvironment::keepCurrent() {
	keepCurrentImpl();
}


void FPEnvironment::clearFlags() {
	clearFlagsImpl();
}

} // namespace Poco
