/*
	Platform_POSIX.h - Platform and architecture identification macros and definitions for 
							various POSIX platforms.
*/


#ifndef Core_Platform_POSIX_INCLUDED
#define Core_Platform_POSIX_INCLUDED


// PA-RISC based HP-UX platforms have some issues...
#if defined(hpux) || defined(_hpux)
	#if defined(__hppa) || defined(__hppa__)
		#define POCO_NO_SYS_SELECT_H 1
		#if defined(__HP_aCC)
			#define POCO_NO_TEMPLATE_ICOMPARE 1
		#endif
	#endif
#endif


// Thread-safety of local static initialization.
#if __cplusplus >= 201103L || __GNUC__ >= 4 || defined(__clang__)
	#ifndef POCO_LOCAL_STATIC_INIT_IS_THREADSAFE
		#define POCO_LOCAL_STATIC_INIT_IS_THREADSAFE 1
	#endif
#endif


// No syslog.h on QNX/BB10.
#if defined(__QNXNTO__)
	#define POCO_NO_SYSLOGCHANNEL
#endif


#endif // Core_Platform_POSIX_INCLUDED
