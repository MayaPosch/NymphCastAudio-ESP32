/*
	ffplay.h - Header file for the static Ffplay class.
	
	Revision 0
	
	Notes:
			- 
		
	2019/10/15 - Maya Posch
*/


#ifndef FFPLAY_H
#define FFPLAY_H


#include <iostream>
#include <atomic>
#include <queue>
#include <cmath>

#ifdef ESP_PLATFORM
	#include <SDL_dummy/SDL.h>
	#include <nymph.h>
#else
	#include <SDL2/SDL.h>
	#include <nymph/nymph.h>
#endif
//#include <SDL2/SDL_mutex.h>

#ifdef NPOCO
#include <npoco/Condition.h>
#include <npoco/Thread.h>
#include <npoco/Runnable.h>
#else
#include <Poco/Condition.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#endif

#include "types.h"


// Forward declarations
void finishPlayback(); 			// Defined in NymphCastServer.cpp
void sendGlobalStatusUpdate();	// Defined in NymphCastServer.cpp
bool startSlavePlayback();		// Defined in NymphCastServer.cpp


struct FileMetaInfo {
	std::atomic<uint64_t> duration;		// seconds
	std::atomic<double> position;		// seconds with remainder.
	std::atomic<float> last_update;		// seconds with remainder.
	std::atomic<uint32_t> width;		// pixels
	std::atomic<uint32_t> height;		// pixels
	std::atomic<uint32_t> video_rate;	// kilobits per second
	std::atomic<uint32_t> audio_rate;	// kilobits per second
	std::atomic<uint32_t> framrate;
	std::atomic<uint8_t> audio_channels;
	std::atomic<bool> seeking;			// true while seeking.
	std::string title;
	std::string artist;
	std::string album;
	Poco::Mutex mutex;	// Use only with non-atomic entries.
	
	void setDuration(uint64_t d) {
		std::cout << "SetDuration. " << d << std::endl;
		duration = d;
	}
	
	uint64_t getDuration() { return duration; }
	
	void setPosition(double p) {
		// Check for an invalid number (libav glitch?). Set new position otherwise.
		
		// Set new position, send client status update if more than N seconds since last time.
		if (seeking) {
			// Seeking mode got set. Always send this first update to clients.
			sendGlobalStatusUpdate();
			seeking = false;
		}
		if (last_update >= 75.0) { // ~3 seconds
			sendGlobalStatusUpdate();
			last_update = 0.0;
		}
		else {
			last_update = last_update + 1; //REFRESH_RATE; // 0.01, or 100 Hz by default.
		}
		
		position = p;
	}
	
	double getPosition() { return position; }
	
	void setTitle(std::string t) { mutex.lock(); title = t; mutex.unlock(); }
	std::string getTitle() { mutex.lock(); std::string t = title; mutex.unlock(); return t; }
	
	void setArtist(std::string a) { mutex.lock(); artist = a; mutex.unlock(); }
	std::string getArtist() { mutex.lock(); std::string a = artist; mutex.unlock(); return a; }
	
	void setAlbum(std::string a) { mutex.lock(); album = a; mutex.unlock(); }
	std::string getAlbum() { mutex.lock(); std::string a = album; mutex.unlock(); return a; }
	
	void setSeeking() { seeking = true; }
};


// --- Globals ---
//extern FileMetaInfo file_meta;

	
class Ffplay : public Poco::Runnable {
    VideoState* is = 0;
	static std::string loggerName;
	std::atomic<bool> running = { false };
	std::atomic<bool> playerStarted = { false };
	std::string castUrl;
	std::atomic<bool> castingUrl = { false };
	std::atomic<bool> playingTrack = { false };
	
	static int media_read(void* opaque, uint8_t* buf, int buf_size);
	static int64_t media_seek(void* opaque, int64_t pos, int whence);
	
public:
	virtual void run();
	uint8_t getVolume();
	void setVolume(uint8_t volume);
	bool playbackActive() { return playerStarted.load(); }
	bool streamTrack(std::string url);
	bool playTrack(int64_t delay = 0);
	void quit();
};


#endif
