/*
	UnWindows.h - Simple wrapper around the <windows.h> header file.
*/


#ifndef Core_UnWindows_INCLUDED
#define Core_UnWindows_INCLUDED


// Reduce bloat
#if defined(_WIN32)
	#if !defined(WIN32_LEAN_AND_MEAN) && !defined(POCO_BLOATED_WIN32)
		#define WIN32_LEAN_AND_MEAN
	#endif
#endif


#if !defined(POCO_NO_WINDOWS_H)
    #include <windows.h>
    #ifdef __MINGW32__
        #include <Winsock2.h>
        #include <Iphlpapi.h>
        #include <ws2tcpip.h>
    #endif // __MINGW32__
#endif


#endif // Core_UnWindows_INCLUDED
