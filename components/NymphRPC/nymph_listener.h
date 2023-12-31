/*
	nymph_listener.h	- Declares the NymphRPC Listener class.
	
	Revision 0
	
	Notes:
			- 
			
	History:
	2017/06/24, Maya Posch	: Initial version.
	
	(c) Nyanko.ws
*/


#pragma once
#ifndef NYMPH_LISTENER_H
#define NYMPH_LISTENER_H


#include <vector>
#include <string>
#include <functional>

#ifdef NPOCO
#include <npoco/Mutex.h>
#else
#include <Poco/Mutex.h>
#endif

#include "nymph_socket_listener.h"


// TYPES

//typedef void (*NymphCallbackMethod)(uint32_t session, NymphMessage* msg, void* data);
typedef std::function<void(uint32_t, NymphMessage*, void*)> NymphCallbackMethod;


struct NymphCallback {
	std::string name;				// Callback method name.
	NymphCallbackMethod method;	// The callback.
	void* data;					// Custom user data.
};


// ---


class NymphListener {
	static std::map<int, NymphSocketListener*> listeners;
	static Poco::Mutex listenersMutex;
	static std::string loggerName;
	
	static std::map<std::string, NymphCallback>& callbacks();
	static Poco::Mutex& callbacksMutex();
	
public:
	static void stop();
	
	static bool addConnection(int handle, NymphSocket socket);
	static bool removeConnection(int handle);
	static bool addMessage(NymphRequest* &request);
	static bool removeMessage(int handle, int64_t messageId);
	static bool addCallback(NymphCallback callback);
	static bool callCallback(uint32_t session, NymphMessage* msg, void* data);
	static bool removeCallback(std::string name);
};

#endif
