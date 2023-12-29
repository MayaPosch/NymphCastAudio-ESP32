/*
	Net.cpp - 
*/


#include "Net.h"


#include "SocketDefs.h"
//#include "Poco/Net/NetException.h"


namespace Poco {
namespace Net {

bool Net_API initializeNetwork() {
#if defined(POCO_OS_FAMILY_WINDOWS)
	WORD    version = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(version, &data) != 0) { return false; }
		//throw NetException("Failed to initialize network subsystem");
#endif
	return true;
}


void Net_API uninitializeNetwork() {
#if defined(POCO_OS_FAMILY_WINDOWS)
	WSACleanup();
#endif
}


std::string htmlize(const std::string& str) {
	std::string::const_iterator it(str.begin());
	std::string::const_iterator end(str.end());
	std::string html;
	for (; it != end; ++it) {
		switch (*it) {
		case '<': html += "&lt;"; break;
		case '>': html += "&gt;"; break;
		case '"': html += "&quot;"; break;
		case '&': html += "&amp;"; break;
		default:  html += *it; break;
		}
	}
	
	return html;
}

} } // namespace Poco::Net


#if defined(POCO_OS_FAMILY_WINDOWS) && !defined(POCO_NO_AUTOMATIC_LIB_INIT)
	/// Network initializer for windows statically
	/// linked library.
	struct NetworkInitializer {
		/// Calls Poco::Net::initializeNetwork();
		NetworkInitializer() {
			Poco::Net::initializeNetwork();
		}

		/// Calls Poco::Net::uninitializeNetwork();
		~NetworkInitializer() {
			try {
				Poco::Net::uninitializeNetwork();
			}
			catch (...) {
				poco_unexpected();
			}
		}
	};

	const NetworkInitializer pocoNetworkInitializer;

#endif
