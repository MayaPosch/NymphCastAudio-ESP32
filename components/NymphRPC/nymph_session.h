/*
	nymph_session.h	- header file for the NymphRPC Session class.
	
	Revision 0
	
	Notes:
			- This class declares the session class to be used by Nymph servers.
			
	History:
	2017/06/24, Maya Posch : Initial version.
	
	(c) Nyanko.ws
*/


#pragma once
#ifndef NYMPH_SESSION_H
#define NYMPH_SESSION_H

#include <string>

#ifdef NPOCO
#include <npoco/net/TCPServerConnection.h>
#include <npoco/Mutex.h>
#else
#include <Poco/Net/TCPServerConnection.h>
#include <Poco/Mutex.h>
#endif


class NymphSession : public Poco::Net::TCPServerConnection {
	std::string loggerName;
	int handle;
	static int lastSessionHandle;
	static Poco::Mutex handleMutex;
	
public:
	NymphSession(const Poco::Net::StreamSocket& socket);
	void run();
	bool send(uint8_t* msg, uint32_t length, std::string &result);
};

#endif
