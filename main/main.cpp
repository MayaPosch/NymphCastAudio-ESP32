/*
	main.cpp - Main file of the NymphCast Audio project.
	
	Features:
			- Service discovery via NyanSD: port 4004, name 'nymphcast-audio'.
			- NymphCast Audio receiver via NymphRPC server API.
			- Playback of formats supported by the Ffplay class.
			- Audio output via I2S to external codec.
			- Tentative: NymphCast Apps support.
			- Tentative: Read-out & displaying of meta information (external display).
			
	Notes:
			- Targeting ESP32 (ESP-IDF) with this MVP.
*/

#include <string.h>

#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
//#include "esp_bt.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/uart.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#else
	#include <iostream>
#endif

#include "databuffer.h"
#include "ffplay/ffplay.h"
#include "ffplay/types.h"
#include "ffplay/sdl_renderer.h"
#include "ffplay/stream_handler.h"
#include <nyansd.h>

#include <map>
#include <vector>
#include <cstring>

#include <npoco/NumberFormatter.h>


#ifdef ESP_PLATFORM
#include "../wifi_stuff.h"	// WiFi credentials and stuff
//#define EXAMPLE_ESP_MAXIMUM_RETRY CONFIG_ESP_MAXIMUM_RETRY
#define EXAMPLE_ESP_MAXIMUM_RETRY 10


#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK


/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "wifi station";

static int s_retry_num = 0;


static void event_handler(void* arg, esp_event_base_t event_base,
								int32_t event_id, void* event_data) {
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
			esp_wifi_connect();
			s_retry_num++;
			ESP_LOGI(TAG, "retry to connect to the AP");
		} else {
			xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
		}
		ESP_LOGI(TAG,"connect to the AP fail");
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
		ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		s_retry_num = 0;
		xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
	}
}


// --- WiFi & hostname globals.
// Set host name to the default value from the wifi_stuff header, if set.
// This will be updated with the name from NVS on boot, if present.
#ifdef ESP_WIFI_SSID
std::string hostName 	= ESP_HOST_NAME;
std::string wifi_ssid 	= ESP_WIFI_SSID;
std::string wifi_pass	= ESP_WIFI_PASS;
#else
std::string hostName;
std::string wifi_ssid;
std::string wifi_pass;
#endif


bool wifi_init_sta() {
	s_wifi_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
														ESP_EVENT_ANY_ID,
														&event_handler,
														NULL,
														&instance_any_id));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
														IP_EVENT_STA_GOT_IP,
														&event_handler,
														NULL,
														&instance_got_ip));

	wifi_config_t wifi_config = { }; // zero initialise.
	strncpy((char*) wifi_config.sta.ssid, (const char*) wifi_ssid.c_str(), 
																	(size_t) wifi_ssid.size());
	strncpy((char*) wifi_config.sta.password, (const char*) wifi_pass.c_str(), 
																	(size_t) wifi_pass.size());
	
	//wifi_config_t wifi_config = {
		//.sta = {
			//.ssid = ESP_WIFI_SSID,
			//.password = ESP_WIFI_PASS,
			/* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
			 * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
			 * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
		 * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
			 */
			/*.threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,*/
			/*.sae_pwe_h2e = WPA3_SAE_PWE_BOTH,*/
		//},
	//};
	
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
	ESP_ERROR_CHECK(esp_wifi_start() );

	ESP_LOGI(TAG, "wifi_init_sta finished.");

	/* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
	 * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
	EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
			WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
			pdFALSE,
			pdFALSE,
			portMAX_DELAY);

	/* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
	 * happened. */
	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
				 ESP_WIFI_SSID, ESP_WIFI_PASS);
		return true;
	} else if (bits & WIFI_FAIL_BIT) {
		ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
				 ESP_WIFI_SSID, ESP_WIFI_PASS);
	} else {
		ESP_LOGE(TAG, "UNEXPECTED EVENT");
	}
	
	return false;
}

#endif


// --- Globals ---
/* options specified by the user */
AVInputFormat *file_iformat;
const char *input_filename;
const char *window_title;
int default_width  = 640;
int default_height = 480;
std::atomic<int> screen_width  = { 0 };
std::atomic<int> screen_height = { 0 };
//int screen_left = SDL_WINDOWPOS_CENTERED;
int screen_left = 0;
//int screen_top = SDL_WINDOWPOS_CENTERED;
int screen_top = 0;
int audio_disable;
int video_disable;
bool subtitle_disable = true;
const char* wanted_stream_spec[AVMEDIA_TYPE_NB] = {0};
int seek_by_bytes = -1;
float seek_interval = 10;
int display_disable;
bool gui_enable;
bool screensaver_enable;
int borderless;
int alwaysontop;
int startup_volume = 100;
int show_status = 1;
int av_sync_type = AV_SYNC_AUDIO_MASTER;
//int av_sync_type = AV_SYNC_EXTERNAL_CLOCK;
int64_t start_time = AV_NOPTS_VALUE;
int64_t duration = AV_NOPTS_VALUE;
int fast = 0;
int genpts = 0;
int lowres = 0;
int decoder_reorder_pts = -1;
int autoexit = 1;
int exit_on_keydown;
int exit_on_mousedown;
int loop = 1;
int framedrop = -1;
int infinite_buffer = -1;
enum ShowMode show_mode = SHOW_MODE_NONE;
const char *audio_codec_name;
const char *subtitle_codec_name;
const char *video_codec_name;
double rdftspeed = 0.02;
int64_t cursor_last_shown;
int cursor_hidden = 0;
#if CONFIG_AVFILTER
const char **vfilters_list = NULL;
int nb_vfilters = 0;
char *afilters = NULL;
#endif
int autorotate = 1;
int find_stream_info = 1;
int filter_nbthreads = 0;
std::atomic<uint32_t> audio_volume = { 100 };
std::atomic<bool> muted = { false };
std::atomic<uint32_t> muted_volume;

// Other globals.
std::atomic<bool> playerPaused = { false };
std::atomic<bool> playerStopped = { false };	// Playback was stopped by the user.
Poco::Thread avThread("ffplay");
Poco::Thread sdlThread("sdl_loop");
Poco::Condition slavePlayCon;
Poco::Mutex slavePlayMutex;
Ffplay ffplay;

const uint32_t nymph_seek_event = SDL_RegisterEvents(1);
std::atomic<bool> running = { true };
std::string loggerName = "NymphCastServer";


// -- BLOCK SIZE ---
// This defines the size of the data blocks requested from the client with a read request.
// Defined in kilobytes.
uint32_t readBlockSize = 20;
// ---


// Data structure.
struct SessionParams {
	int max_buffer;
};

struct NymphCastSlaveRemote {
	std::string name;
	std::string ipv4;
	std::string ipv6;
	uint16_t port;
	uint32_t handle;
	int64_t delay;
};


enum NymphRemoteStatus {
	NYMPH_PLAYBACK_STATUS_STOPPED = 1,
	NYMPH_PLAYBACK_STATUS_PLAYING = 2,
	NYMPH_PLAYBACK_STATUS_PAUSED = 3
};


struct CastClient {
	std::string name;
	int handle;
	bool sessionActive;
	uint32_t filesize;
};

std::map<int, CastClient> clients;


enum NymphSeekType {
	NYMPH_SEEK_TYPE_BYTES = 1,
	NYMPH_SEEK_TYPE_PERCENTAGE = 2
};


NcsMode serverMode = NCS_MODE_STANDALONE;
std::vector<NymphCastSlaveRemote> slave_remotes;
uint32_t slaveLatencyMax = 0;	// Max latency to slave remote in milliseconds.
// ---

// --- Callbacks ---
// --- MEDIA READ CALLBACK ---
// Called by slave remotes during a read request.
void MediaReadCallback(uint32_t session, NymphMessage* msg, void* data) {
	// Handled by the usual client & master routines.
}


// --- MEDIA SEEK CALLBACK ---
// Called by a slave remote during a seek request.
void MediaSeekCallback(uint32_t session, NymphMessage* msg, void* data) {
	// Handled by the usual client & master routines.
}


// --- MEDIA STOP CALLBACK ---
// Called by a slave remote as a stop notification.
void MediaStopCallback(uint32_t session, NymphMessage* msg, void* data) {
	// 
}


// --- MEDIA STATUS CALLBACK ---
// Called by a slave remote during a status update.
void MediaStatusCallback(uint32_t session, NymphMessage* msg, void* data) {
	// Handled by the usual client & master routines.
}


// --- GET PLAYBACK STATUS ---
std::map<std::string, NymphPair>* getPlaybackStatus() {
	// Set the playback status.
	// We're sending back whether we are playing something currently. If so, also includes:
	// * duration of media in seconds.
	// * position in the media, in seconds with remainder.
	// * title of the media, if available.
	// * artist of the media, if available.
	std::map<std::string, NymphPair>* pairs = new std::map<std::string, NymphPair>();
	NymphPair pair;
	std::string* key;
	if (ffplay.playbackActive()) {
		// Distinguish between playing and paused for the player.
		if (playerPaused) {
			key = new std::string("status");
			pair.key = new NymphType(key, true);
			pair.value = new NymphType((uint32_t) NYMPH_PLAYBACK_STATUS_PAUSED);
			pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		}
		else {
			key = new std::string("status");
			pair.key = new NymphType(key, true);
			pair.value = new NymphType((uint32_t) NYMPH_PLAYBACK_STATUS_PLAYING);
			pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		}
		
		key = new std::string("playing");
		pair.key = new NymphType(key, true);
		pair.value = new NymphType(true);
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("stopped");
		pair.key = new NymphType(key, true);
		pair.value = new NymphType(false);
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("duration");
		pair.key = new NymphType(key, true);
		//pair.value = new NymphType(FileMetaInfo::getDuration());
		//pair.value = new NymphType(file_meta.getDuration());
		pair.value = new NymphType(StreamHandler::getDuration());
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("position");
		pair.key = new NymphType(key, true);
		//pair.value = new NymphType(FileMetaInfo::getPosition());
		//pair.value = new NymphType(file_meta.getPosition());
		pair.value = new NymphType(StreamHandler::getPosition());
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("title");
		pair.key = new NymphType(key, true);
		//std::string* val = new std::string(FileMetaInfo::getTitle());
		//std::string* val = new std::string(file_meta.getTitle());
		std::string* val = new std::string(StreamHandler::getTitle());
		pair.value = new NymphType(val, true);
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("artist");
		pair.key = new NymphType(key, true);
		//val = new std::string(FileMetaInfo::getArtist());
		//val = new std::string(file_meta.getArtist());
		val = new std::string(StreamHandler::getArtist());
		pair.value = new NymphType(val, true);
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("volume");
		pair.key = new NymphType(key, true);
		pair.value = new NymphType((uint8_t) audio_volume.load());
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("subtitle_disable");
		pair.key = new NymphType(key, true);
		pair.value = new NymphType(subtitle_disable);
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
	}
	else {
		if (playerStopped) {
			// Stopped by user.
			key = new std::string("stopped");
			pair.key = new NymphType(key, true);
			pair.value = new NymphType(true);
			pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		}
		else {
			key = new std::string("stopped");
			pair.key = new NymphType(key, true);
			pair.value = new NymphType(false);
			pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		}
		
		key = new std::string("status");
		pair.key = new NymphType(key, true);
		pair.value = new NymphType((uint32_t) NYMPH_PLAYBACK_STATUS_STOPPED);
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("playing");
		pair.key = new NymphType(key, true);
		pair.value = new NymphType(false);
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("duration");
		pair.key = new NymphType(key, true);
		pair.value = new NymphType((uint64_t) 0);
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("position");
		pair.key = new NymphType(key, true);
		pair.value = new NymphType((double) 0.0);
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("title");
		pair.key = new NymphType(key, true);
		pair.value = new NymphType((char*) 0, 0);
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("artist");
		pair.key = new NymphType(key, true);
		pair.value = new NymphType((char*) 0, 0);
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("volume");
		pair.key = new NymphType(key, true);
		pair.value = new NymphType((uint8_t) audio_volume.load());
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
		
		key = new std::string("subtitle_disable");
		pair.key = new NymphType(key, true);
		pair.value = new NymphType(subtitle_disable);
		pairs->insert(std::pair<std::string, NymphPair>(*key, pair));
	}
	
	return pairs;
}


// --- SEND STATUS UPDATE ---
void sendStatusUpdate(uint32_t handle) {
	// Call the status update callback with the current playback status.
	std::vector<NymphType*> values;
	std::map<std::string, NymphPair>* status = getPlaybackStatus();
	values.push_back(new NymphType(status, true));
	NymphType* resVal = 0;
	std::string result;
	if (!NymphRemoteClient::callCallback(handle, "MediaStatusCallback", values, result)) {
		NYMPH_LOG_ERROR("Calling media status callback failed: " + result);
		return;
	}
}


// --- SEND GLOBAL STATUS UPDATE ---
// Send playback status update to all connected clients.
void sendGlobalStatusUpdate() {
	NYMPH_LOG_INFORMATION("Sending status update to all " + Poco::NumberFormatter::format(clients.size()) 
					+ " clients.");
					
	std::map<std::string, NymphPair>* pairs = getPlaybackStatus();
	std::map<int, CastClient>::const_iterator it;
	for (it = clients.begin(); it != clients.end();/**/) {
		NYMPH_LOG_DEBUG("Client ID: " + Poco::NumberFormatter::format(it->first) + "/" + it->second.name);
		
		// Call the status update callback with the current playback status.
		NymphType* resVal = 0;
		std::string result;
		std::vector<NymphType*> values;
		values.push_back(new NymphType(pairs));
		if (!NymphRemoteClient::callCallback(it->first, "MediaStatusCallback", values, result)) {
			NYMPH_LOG_ERROR("Calling media status callback failed: " + result);
			
			// An error here very likely means that the client no long exists. Remove it.
			it = clients.erase(it);
		}
		else {
			++it;
		}
	}
	
	// Delete the map and its values.
	// TODO: persist the data somehow.
	std::map<std::string, NymphPair>::iterator sit;
	for (sit = pairs->begin(); sit != pairs->end(); ++sit) {
		delete sit->second.key;
		delete sit->second.value;
	}
	
	delete pairs;
}


// --- START SLAVE PLAYBACK ---
// [Master] Signal slaves that they can begin playback.
bool startSlavePlayback() {
	// Start slaves according to the average connection latency.
	Poco::Timestamp ts;
	int64_t now = (int64_t) ts.epochMicroseconds();
	//then = now + (slaveLatencyMax * 2);
		
	// Timing: 	Multiply the max slave latency by the number of slaves. After sending this delay
	// 			to the first slave (minus half its recorded latency), 
	// 			subtract the time it took to send to this slave from the
	//			first delay, then send this new delay to the second slave, and so on.
	int64_t countdown = slaveLatencyMax * slave_remotes.size();
	
	int64_t then = 0;
	for (int i = 0; i < slave_remotes.size(); ++i) {
		NymphCastSlaveRemote& rm = slave_remotes[i];
		//then = slaveLatencyMax - rm.delay;
		then = countdown - (rm.delay / 2);
			
		int64_t send = (int64_t) ts.epochMicroseconds();
		
		// Prepare data vector.
		std::vector<NymphType*> values;
		values.push_back(new NymphType(then));
			
		std::string result;
		NymphType* returnValue = 0;
		if (!NymphRemoteServer::callMethod(rm.handle, "slave_start", values, returnValue, result)) {
			NYMPH_LOG_ERROR("Calling slave_start failed: " + result);
			return false;
		}
			
		delete returnValue;
			
		int64_t receive = (int64_t) ts.epochMicroseconds();
			
		countdown -= (receive - send);
	}
		
	// Wait out the countdown before returning.
	std::condition_variable cv;
	std::mutex cv_m;
	std::unique_lock<std::mutex> lk(cv_m);
	std::chrono::microseconds dur(countdown);
	while (cv.wait_for(lk, dur) != std::cv_status::timeout) { }
	
	/* for (uint32_t i = 0; i < slave_remotes.size(); ++i) {
		//
		NymphType* resVal = 0;
		std::string result;
		std::vector<NymphType*> values;
		if (!NymphRemoteClient::callCallback(slave_remotes[i].handle, "slave_start", values, result)) {
			NYMPH_LOG_ERROR("Calling slave_start failed: " + result);
			return false;
		}
	} */
	
	return true;
}


// --- DATA REQUEST HANDLER ---
// Allows the DataBuffer to request more file data from a client.
bool dataRequestHandler(uint32_t session) {
	if (DataBuffer::seeking()) {
		NYMPH_LOG_ERROR("Cannot request data while seeking. Abort.");
		return false;
	}
	
	DataBuffer::dataRequestPending = true;
	
	NYMPH_LOG_INFORMATION("Asking for data...");

	// Request more data.
	std::vector<NymphType*> values;
	values.push_back(new NymphType(readBlockSize));
	std::string result;
	if (!NymphRemoteClient::callCallback(DataBuffer::getSessionHandle(), "MediaReadCallback", values, result)) {
		NYMPH_LOG_ERROR("Calling callback failed: " + result);
		return false;
	}
	
	return true;
}


// --- SEEKING HANDLER ---
void seekingHandler(uint32_t session, int64_t offset) {
	if (DataBuffer::seeking()) {
		if (serverMode == NCS_MODE_MASTER) {
			// Send data buffer reset notification. This ensures that those are all reset as well.
			for (int i = 0; i < slave_remotes.size(); ++i) {
				NymphCastSlaveRemote& rm = slave_remotes[i];
				std::vector<NymphType*> values;
				std::string result;
				NymphType* returnValue = 0;
				if (!NymphRemoteServer::callMethod(rm.handle, "slave_buffer_reset", values, returnValue, result)) {
					// TODO: Handle error. Check return value.
					NYMPH_LOG_ERROR("Calling slave_buffer_reset failed.");
				}
				
				delete returnValue;
			}	
		}
		
		// Send message to client indicating that we're seeking in the file.
		std::vector<NymphType*> values;
		values.push_back(new NymphType((uint64_t) offset));
		values.push_back(new NymphType(readBlockSize));
		std::string result;
		NymphType* resVal = 0;
		if (!NymphRemoteClient::callCallback(session, "MediaSeekCallback", values, result)) {
			NYMPH_LOG_ERROR("Calling media seek callback failed: " + result);
			return;
		}
				
		return; 
	}
}


// --- FINISH PLAYBACK ---
// Called at the end of playback of a stream or file.
// If a stream is queued, play it, otherwise end playback.
void finishPlayback() {
	// Send message to client indicating that we're done.
	uint32_t handle = DataBuffer::getSessionHandle();
	std::vector<NymphType*> values;
	std::string result;
	if (!NymphRemoteClient::callCallback(handle, "MediaStopCallback", values, result)) {
		NYMPH_LOG_ERROR("Calling media stop callback failed: " + result);
		return;
	}
	
	// Update the LCDProc daemon if enabled.
	/* if (lcdproc_enabled) {
		// TODO: Clear the screen?
	} */
	
	// Start the Screensaver here for now.
	/* if (!display_disable) {
		if (gui_enable) {
			// Return to GUI. Hide window.
			SDL_Event event;
			event.type = SDL_KEYDOWN;
			event.key.keysym.sym = SDLK_UNDERSCORE;
			SDL_PushEvent(&event);
			
			Gui::active = true;
			Gui::resumeCv.notify_one();
		}
		else if (screensaver_enable) {
			// Start screensaver.
			ScreenSaver::start(15);
		}
		else {
			// Hide window.
			SDL_Event event;
			event.type = SDL_KEYDOWN;
			event.key.keysym.sym = SDLK_UNDERSCORE;
			SDL_PushEvent(&event);
		}
	} */
}


// --- STREAM TRACK ---
// Attempt to stream from the indicated URL.
bool streamTrack(std::string url) {
	// TODO: Check that we're not still streaming, otherwise queue the URL.
	// TODO: allow to cancel any currently playing track/empty queue?
	if (ffplay.playbackActive()) {
		// Add to queue.
		DataBuffer::addStreamTrack(url);
		
		return true;
	}
	
	// Schedule next track URL.
	ffplay.streamTrack(url);
	
	// Send status update to client.
	sendStatusUpdate(DataBuffer::getSessionHandle());
	
	return true;
}


// Callback for the connect function.
NymphMessage* connectClient(int session, NymphMessage* msg, void* data) {
	NYMPH_LOG_INFORMATION("Received message for session: " + Poco::NumberFormatter::format(session)
							+ ", msg ID: " + Poco::NumberFormatter::format(msg->getMessageId()));
	
	std::string clientStr = msg->parameters()[0]->getString();
	NYMPH_LOG_INFORMATION("Client string: " + clientStr);
	
	// TODO: check whether we're not operating in slave or master mode already.
	NYMPH_LOG_INFORMATION("Switching to stand-alone server mode.");
	serverMode = NCS_MODE_STANDALONE;
	
	// Register this client with its ID. Return error if the client ID already exists.
	NymphMessage* returnMsg = msg->getReplyMessage();
	std::map<int, CastClient>::iterator it;
	it = clients.find(session);
	NymphType* retVal = 0;
	if (it != clients.end()) {
		// Client ID already exists, abort.
		retVal = new NymphType(false);
	}
	else {
		CastClient c;
		c.name = clientStr;
		c.handle = session;
		c.sessionActive = false;
		c.filesize = 0;
		clients.insert(std::pair<int, CastClient>(session, c));
		retVal = new NymphType(true);
	}
	
	// Send the client the current playback status.
	std::vector<NymphType*> values;
	std::map<std::string, NymphPair>* status = getPlaybackStatus();
	values.push_back(new NymphType(status, true));
	std::string result;
	if (!NymphRemoteClient::callCallback(session, "MediaStatusCallback", values, result)) {
		NYMPH_LOG_ERROR("Calling media status callback failed: " + result);
	}
	
	returnMsg->setResultValue(retVal);
	msg->discard();
	
	return returnMsg;
}


// --- CONNECT MASTER ---
// Master server calls this to turn this server instance into a slave.
// This disables the regular client connection functionality for the duration of the master/slave
// session.
// Returns the timestamp when the message was received.
// sint64 connectMaster(sint64)
NymphMessage* connectMaster(int session, NymphMessage* msg, void* data) {
	NYMPH_LOG_INFORMATION("Received master connect request, slave mode initiation requested.");
	
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	// Switch to slave mode, if possible.
	// Return error if we're currently playing content in stand-alone mode.
	if (ffplay.playbackActive()) {
		returnMsg->setResultValue(new NymphType((int64_t) 0));
	}
	else {
		// FIXME: for now we just return the current time.
		NYMPH_LOG_INFORMATION("Switching to slave server mode.");
		serverMode = NCS_MODE_SLAVE;
		//DataBuffer::setFileSize(it->second.filesize);
		DataBuffer::setSessionHandle(session);
		
		Poco::Timestamp ts;
		int64_t now = (int64_t) ts.epochMicroseconds();
		returnMsg->setResultValue(new NymphType(now));
	}
	
	// TODO: Obtain timestamp, compare with current time.
	//time_t then = ((NymphSint64*) msg->parameters()[0])->getValue();
	
	// TODO: Send delay request to master.
	
	// TODO: Determine final latency and share with master.
	
	msg->discard();
	
	return returnMsg;
}


// --- RECEIVE DATA MASTER ---
// Receives data chunks for playback from a master receiver. (Slave-only)
// uint8 receiveDataMaster(blob data, bool done, sint64 when)
NymphMessage* receiveDataMaster(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	// Extract data blob and add it to the buffer.
	NymphType* mediaData = msg->parameters()[0];
	bool done = msg->parameters()[1]->getBool();
	
	// Write string into buffer.
	DataBuffer::write(mediaData->getChar(), mediaData->string_length());
	
	// Playback is started in its own function, which is called by the master when it's ready.
	int64_t then = 0;
	if (!ffplay.playbackActive()) {
		int64_t when = msg->parameters()[2]->getInt64();
		
		// Start the player when the delay in 'when' has been reached.
		/* std::condition_variable cv;
		std::mutex cv_m;
		std::unique_lock<std::mutex> lk(cv_m);
		//std::chrono::system_clock::time_point then = std::chrono::system_clock::from_time_t(when);
		std::chrono::microseconds dur(when);
		std::chrono::time_point<std::chrono::system_clock> then(dur);
		//while (cv.wait_until(lk, then) != std::cv_status::timeout) { }
		while (cv.wait_for(lk, dur) != std::cv_status::timeout) { } */
		
		// Start player.
		ffplay.playTrack(when);
	}
	
	if (done) {
		DataBuffer::setEof(done);
	}
	
	msg->discard();
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	return returnMsg;
}


// --- SLAVE START ---
// Receives data chunks for playback from a master receiver. (Slave-only)
// uint8 slave_start(int64 when)
NymphMessage* slave_start(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	// Extract data blob and add it to the buffer.
	//NymphType* mediaData = msg->parameters()[0];
	//bool done = msg->parameters()[1]->getBool();
	int64_t when = msg->parameters()[0]->getInt64();
	
	if (!ffplay.playbackActive()) {
		// Start the player when the delay in 'when' has been reached.
		std::condition_variable cv;
		std::mutex cv_m;
		std::unique_lock<std::mutex> lk(cv_m);
		//std::chrono::system_clock::time_point then = std::chrono::system_clock::from_time_t(when);
		std::chrono::microseconds dur(when);
		std::chrono::time_point<std::chrono::system_clock> then(dur);
		//while (cv.wait_until(lk, then) != std::cv_status::timeout) { }
		while (cv.wait_for(lk, dur) != std::cv_status::timeout) { }
	}
	
	// Trigger the playback start condition variable that will resume the read_thread of this slave
	// receiver's ffplay module.
	if (serverMode == NCS_MODE_SLAVE) {
		slavePlayCon.signal();
	}
	
	msg->discard();
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	return returnMsg;
}


// Client disconnects from server.
// bool disconnect()
NymphMessage* disconnect(int session, NymphMessage* msg, void* data) {
	
	// Remove the client ID from the list.
	std::map<int, CastClient>::iterator it;
	it = clients.find(session);
	if (it != clients.end()) {
		clients.erase(it);
	}
	
	NYMPH_LOG_INFORMATION("Current server mode: " + Poco::NumberFormatter::format(serverMode));
	
	// Disconnect any slave remotes if we're connected.
	if (serverMode == NCS_MODE_MASTER) {
		NYMPH_LOG_DEBUG("# of slave remotes: " + 
								Poco::NumberFormatter::format(slave_remotes.size()));
		for (int i = 0; i < slave_remotes.size(); ++i) {
			// Disconnect from slave remote.
			NymphCastSlaveRemote& rm = slave_remotes[i];
			NYMPH_LOG_DEBUG("Disconnecting slave: " + rm.name);
			std::string result;
			if (!NymphRemoteServer::disconnect(rm.handle, result)) {
				// Failed to connect, error out. Disconnect from any already connected slaves.
				NYMPH_LOG_ERROR("Slave disconnect error: " + result);
			}
		}
		
		slave_remotes.clear();
	}
	
	NYMPH_LOG_INFORMATION("Switching to stand-alone server mode.");
	serverMode = NCS_MODE_STANDALONE;
	
	NymphMessage* returnMsg = msg->getReplyMessage();
	returnMsg->setResultValue(new NymphType(true));
	msg->discard();
	
	return returnMsg;
}


// Client starts a session.
// Return value: OK (0), ERROR (1).
// int session_start(struct fileInfo)
NymphMessage* session_start(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	// Set up a new session instance for the client.
	std::map<int, CastClient>::iterator it;
	it = clients.find(session);
	if (it == clients.end()) {
		returnMsg->setResultValue(new NymphType((uint8_t) 1));
		msg->discard();
		
		return returnMsg;
	}
	
	// Obtain the filesize from the client, which we use with the buffer management.
	NymphType* fileInfo = msg->parameters()[0];
	NymphType* num = 0;
	if (!fileInfo->getStructValue("filesize", num)) {
		NYMPH_LOG_FATAL("Didn't find entry 'filesize'. Aborting...");
		returnMsg->setResultValue(new NymphType((uint8_t) 1));
		msg->discard();
		
		return returnMsg;
	}
	
	it->second.filesize = num->getUint32();
	
	// Check whether we're already playing or not. If we continue here, this will forcefully 
	// end current playback.
	//	FIXME:	=> this likely happens due to a status update glitch. Fix by sending back status update
	// 			along with error?
	if (ffplay.playbackActive()) {
		NYMPH_LOG_ERROR("Trying to start a new session with session already active. Abort.");
		returnMsg->setResultValue(new NymphType((uint8_t) 1));
		msg->discard();
		
		return returnMsg;
	}
	
	// If the AV thread is currently running, we wait until it's quit.
	//if (avThread.joinable()) { avThread.join(); } // FIXME: C++11 version
	/* if (avThread.isRunning()) {
		std::cout << "AV thread active: waiting for join..." << std::endl;
		bool ret = avThread.tryJoin(100);
		if (!ret) {
			// Player thread is still running, meaning we cannot proceed. Error out.
			std::cerr << "Joining failed: aborting new session..." << std::endl;
			returnMsg->setResultValue(new NymphType((uint8_t) 1));
			msg->discard();

			return returnMsg;
		}
	} */
	
	NYMPH_LOG_INFORMATION("Starting new session for file with size: " +
							Poco::NumberFormatter::format(it->second.filesize));
	
	DataBuffer::setFileSize(it->second.filesize);
	DataBuffer::setSessionHandle(session);
	
	// Start calling the client's read callback method to obtain data. Once the data buffer
	// has been filled sufficiently, start the playback.
	if (!DataBuffer::start()) {
		NYMPH_LOG_ERROR("Failed to start buffering. Abort.");
		returnMsg->setResultValue(new NymphType((uint8_t) 1));
		msg->discard();
		
		return returnMsg;
	}
		
	it->second.sessionActive = true;
	
	// Stop screensaver.
	/* if (!video_disable) {
		if (gui_enable) {
			// Show window.
			SDL_Event event;
			event.type = SDL_KEYDOWN;
			event.key.keysym.sym = SDLK_MINUS;
			SDL_PushEvent(&event);
		}
		else if (screensaver_enable) {
			ScreenSaver::stop();
		}
		else {
			// Show window.
			SDL_Event event;
			event.type = SDL_KEYDOWN;
			event.key.keysym.sym = SDLK_MINUS;
			SDL_PushEvent(&event);
		}
	} */
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- SESSION ADD SLAVE ---
// Client sends list of slave server which this server instance should control.
// Returns: OK (0), ERROR (1).
// int session_add_slave(array servers);
NymphMessage* session_add_slave(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	// Disconnect slaves and clear array.
	// TODO: Maybe merge this with proper session management.
	for (uint32_t i = 0; i < slave_remotes.size(); ++i) {
		NymphCastSlaveRemote& rm = slave_remotes[i];
		std::string result;
		if (!NymphRemoteServer::disconnect(rm.handle, result)) {
			// Failed to connect, error out. Disconnect from any already connected slaves.
			NYMPH_LOG_ERROR("Slave disconnection error: " + result);
			
			returnMsg->setResultValue(new NymphType((uint8_t) 1));
			msg->discard();
			
			return returnMsg;
		}
	}
	
	slave_remotes.clear();
	slaveLatencyMax = 0;
	
	// Extract the array.
	std::vector<NymphType*>* remotes = msg->parameters()[0]->getArray();
	for (int i = 0; i < (*remotes).size(); ++i) {
		std::map<std::string, NymphPair>* pairs = (*remotes)[i]->getStruct();
		NymphCastSlaveRemote remote;
		NymphType* value = 0;
		(*remotes)[i]->getStructValue("name", value);
		remote.name = std::string(value->getChar(), value->string_length());
		
		(*remotes)[i]->getStructValue("ipv4", value);
		remote.ipv4 = std::string(value->getChar(), value->string_length());
		
		(*remotes)[i]->getStructValue("ipv6", value);
		remote.ipv6 = std::string(value->getChar(), value->string_length());
		
		(*remotes)[i]->getStructValue("port", value);
		remote.port = value->getUint16();
		remote.handle = 0;
		remote.delay = 0;
	
		slave_remotes.push_back(remote);
	}
	
	// Validate that each slave remote is accessible and determine latency.
	for (int i = 0; i < slave_remotes.size(); ++i) {
		// Establish RPC connection to remote. Starts the PTP-like handshake.
		NymphCastSlaveRemote& rm = slave_remotes[i];
		std::string result;
		if (!NymphRemoteServer::connect(rm.ipv4, 4004, rm.handle, 0, result)) {
			// Failed to connect, error out. Disconnect from any already connected slaves.
			NYMPH_LOG_ERROR("Slave connection error: " + result);
			for (; i >= 0; --i) {
				NymphCastSlaveRemote& drm = slave_remotes[i];
				NymphRemoteServer::disconnect(drm.handle, result);
			}
			
			returnMsg->setResultValue(new NymphType((uint8_t) 1));
			msg->discard();
			
			return returnMsg;
		}
		
		// Attempt to start slave mode on the remote.
		// Send the current timestamp to the slave remote as part of the latency determination.
		Poco::Timestamp ts;
		int64_t now = (int64_t) ts.epochMicroseconds();
		std::vector<NymphType*> values;
		values.push_back(new NymphType(now));
		NymphType* returnValue = 0;
		if (!NymphRemoteServer::callMethod(rm.handle, "connectMaster", values, returnValue, result)) {
			NYMPH_LOG_ERROR("Slave connect master failed: " + result);
			// TODO: disconnect from slave remotes.
			returnMsg->setResultValue(new NymphType((uint8_t) 1));
			msg->discard();
			
			return returnMsg;
		}
		
		// Get new time. This should be roughly twice the latency to the slave remote.
		ts.update();
		int64_t pong = ts.epochMicroseconds();
		time_t theirs = returnValue->getInt64();
		delete returnValue;
		if (theirs == 0) {
			NYMPH_LOG_ERROR("Configuring remote as slave failed.");
			// TODO: disconnect from slave remotes.
			returnMsg->setResultValue(new NymphType((uint8_t) 1));
			msg->discard();
			
			return returnMsg;
		}
		
		// Use returned time stamp to calculate the delay.
		// FIXME: using stopwatch-style local time to determine latency for now.
		
		//rm.delay = theirs - now;
		rm.delay = pong - now;
		NYMPH_LOG_DEBUG("Slave delay: " + Poco::NumberFormatter::format(rm.delay) + 
							" microseconds.");
		NYMPH_LOG_DEBUG("Current max slave delay: " + 
							Poco::NumberFormatter::format(slaveLatencyMax));
		if (rm.delay > slaveLatencyMax) { 
			slaveLatencyMax = rm.delay;
			NYMPH_LOG_DEBUG("Max slave latency increased to: " + 
								Poco::NumberFormatter::format(slaveLatencyMax) + " microseconds.");
		}
	}
	
	NYMPH_LOG_INFORMATION("Switching to master server mode.");
	serverMode = NCS_MODE_MASTER;
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// Client sends a chunk of track data.
// Returns: OK (0), ERROR (1).
// int session_data(string buffer, boolean done)
NymphMessage* session_data(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	// Get iterator to the session instance for the client.
	std::map<int, CastClient>::iterator it;
	it = clients.find(session);
	if (it == clients.end()) {
		returnMsg->setResultValue(new NymphType((uint8_t) 1));
		msg->discard();
		return returnMsg;
	}
	
	// Safely write the data for this session to the buffer.
	NymphType* mediaData = msg->parameters()[0];
	bool done = msg->parameters()[1]->getBool();
	
	// Update EOF status.
	DataBuffer::setEof(done);
	
	// Write string into buffer.
	DataBuffer::write(mediaData->getChar(), mediaData->string_length());
	
	// If passing the message through to slave remotes, add the timestamp to the message.
	// This timestamp is the current time plus the largest master-slave latency times 2.
	// Timing: 	Multiply the max slave latency by the number of slaves. After sending this delay
	// 			to the first slave (minus half its recorded latency), 
	// 			subtract the time it took to send to this slave from the
	//			first delay, then send this new delay to the second slave, and so on.
	int64_t then = 0;
	if (serverMode == NCS_MODE_MASTER) {
		Poco::Timestamp ts;
		int64_t now = 0;
		int64_t countdown = 0;
		if (!ffplay.playbackActive()) {
			now = (int64_t) ts.epochMicroseconds();
			//then = now + (slaveLatencyMax * 2);
			countdown = slaveLatencyMax * slave_remotes.size();
		}
		
		for (int i = 0; i < slave_remotes.size(); ++i) {
			NymphCastSlaveRemote& rm = slave_remotes[i];
			
			int64_t send = 0;
			if (!ffplay.playbackActive()) {
				//then = slaveLatencyMax - rm.delay;
				then = countdown - (rm.delay / 2);
				send = (int64_t) ts.epochMicroseconds();
			}
		
			// Prepare data vector.
			NymphType* media = new NymphType((char*) mediaData->getChar(), mediaData->string_length());
			NymphType* doneBool = new NymphType(done);
			std::vector<NymphType*> values;
			values.push_back(media);
			values.push_back(doneBool);
			values.push_back(new NymphType(then));
			
			std::string result;
			NymphType* returnValue = 0;
			if (!NymphRemoteServer::callMethod(rm.handle, "receiveDataMaster", values, returnValue, result)) {
				// TODO:
			}
			
			delete returnValue;
			
			if (!ffplay.playbackActive()) {
				int64_t receive = (int64_t) ts.epochMicroseconds();
			
				countdown -= (receive - send);
			}
		}
		
		if (!ffplay.playbackActive()) {
			// Wait out the countdown.
			std::condition_variable cv;
			std::mutex cv_m;
			std::unique_lock<std::mutex> lk(cv_m);
			std::chrono::microseconds dur(countdown);
			while (cv.wait_for(lk, dur) != std::cv_status::timeout) { }
		}
	}
	
	// Start the player if it hasn't yet. This ensures we have a buffer ready.
	// TODO: take into account delay of slave remotes before starting local playback.
	if (!ffplay.playbackActive()) {
		// if we're in master mode, only start after the slaves are starting as well.
		// In slave mode, we execute time-critical commands like playback start when
		/* if (serverMode == NCS_MODE_MASTER) {
			// We use the calculated latency to the slave to determine when to send the play
			// command to the slave.
			// TODO:
		} */
		
		/* if (serverMode == NCS_MODE_MASTER) {
			// Start the player when the delay in 'then' has been reached.
			std::condition_variable cv;
			std::mutex cv_m;
			std::unique_lock<std::mutex> lk(cv_m);
			std::chrono::microseconds dur(slaveLatencyMax);
			//std::chrono::time_point<std::chrono::system_clock> when(dur);
			while (cv.wait_for(lk, dur) != std::cv_status::timeout) { }
		} */
		
		// Start playback locally.
		ffplay.playTrack();
		
		playerStopped = false;
	}
	else {
		// Send status update to clients.
		sendGlobalStatusUpdate();
	}
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// Client ends the session.
// Returns: OK (0), ERROR (1).
// int session_end()
NymphMessage* session_end(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	// Mark session as inactive.
	std::map<int, CastClient>::iterator it;
	it = clients.find(session);
	if (it == clients.end()) {
		returnMsg->setResultValue(new NymphType((uint8_t) 1));
		msg->discard();
		
		return returnMsg;
	}
	
	it->second.sessionActive = false;
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- SLAVE BUFFER RESET ---
// Called to reset the slave's local data buffer.
// Returns: OK (0), ERROR (1).
// int slave_buffer_reset()
NymphMessage* slave_buffer_reset(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	// Call DataBuffer's reset function.
	if (!DataBuffer::reset()) {
		NYMPH_LOG_ERROR("Resetting data buffer failed.");
		returnMsg->setResultValue(new NymphType((uint8_t) 1));
		msg->discard();
		
		return returnMsg;
	}
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- VOLUME SET ---
// uint8 volume_set(uint8 volume)
NymphMessage* volume_set(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	uint8_t volume = msg->parameters()[0]->getUint8();
	
	std::vector<NymphType*> values;
	values.push_back(new NymphType(volume));
	if (serverMode == NCS_MODE_MASTER) {
		for (int i = 0; i < slave_remotes.size(); ++i) {
			NymphCastSlaveRemote& rm = slave_remotes[i];
			std::string result;
			NymphType* returnValue = 0;
			if (!NymphRemoteServer::callMethod(rm.handle, "volume_set", values, returnValue, result)) {
				// TODO:
			}
			
			delete returnValue;
		}
	}
	
	audio_volume = volume;
	ffplay.setVolume(volume);
	
	// Inform all clients of this update.
	sendGlobalStatusUpdate();
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- VOLUME UP ---
// uint8 volume_up()
NymphMessage* volume_up(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	if (serverMode == NCS_MODE_MASTER) {
		for (int i = 0; i < slave_remotes.size(); ++i) {
			NymphCastSlaveRemote& rm = slave_remotes[i];
			std::vector<NymphType*> values;
			std::string result;
			NymphType* returnValue = 0;
			if (!NymphRemoteServer::callMethod(rm.handle, "volume_up", values, returnValue, result)) {
				// TODO:
			}
			
			delete returnValue;
		}
	}
	
	SDL_Event event;
	event.type = SDL_KEYDOWN;
	event.key.keysym.sym = SDLK_0;
	SDL_PushEvent(&event);
	
	// TODO: update global audio_volume variable.
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- VOLUME DOWN ---
// uint8 volume_down()
NymphMessage* volume_down(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	if (serverMode == NCS_MODE_MASTER) {
		for (int i = 0; i < slave_remotes.size(); ++i) {
			NymphCastSlaveRemote& rm = slave_remotes[i];
			std::vector<NymphType*> values;
			std::string result;
			NymphType* returnValue = 0;
			if (!NymphRemoteServer::callMethod(rm.handle, "volume_down", values, returnValue, result)) {
				// TODO:
			}
			
			delete returnValue;
		}
	}
	
	SDL_Event event;
	event.type = SDL_KEYDOWN;
	event.key.keysym.sym = SDLK_9;
	SDL_PushEvent(&event);
	
	// TODO: update global audio_volume variable.
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- VOLUME MUTE ---
// uint8 volume_mute()
NymphMessage* volume_mute(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	if (serverMode == NCS_MODE_MASTER) {
		for (int i = 0; i < slave_remotes.size(); ++i) {
			NymphCastSlaveRemote& rm = slave_remotes[i];
			std::vector<NymphType*> values;
			std::string result;
			NymphType* returnValue = 0;
			if (!NymphRemoteServer::callMethod(rm.handle, "volume_mute", values, returnValue, result)) {
				// TODO:
			}
			
			delete returnValue;
		}
	}
	
	// Update muted audio state.
	muted = !muted;
	if (muted) {
		muted_volume = audio_volume.load();
		audio_volume = 0;
	}
	else {
		audio_volume = muted_volume.load();
	}
	
	SDL_Event event;
	event.type = SDL_KEYDOWN;
	event.key.keysym.sym = SDLK_m;
	SDL_PushEvent(&event);
	
	sendGlobalStatusUpdate();
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- PLAYBACK START ---
// uint8 playback_start()
NymphMessage* playback_start(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	if (serverMode == NCS_MODE_MASTER) {
		for (int i = 0; i < slave_remotes.size(); ++i) {
			NymphCastSlaveRemote& rm = slave_remotes[i];
			std::vector<NymphType*> values;
			std::string result;
			NymphType* returnValue = 0;
			if (!NymphRemoteServer::callMethod(rm.handle, "playback_start", values, returnValue, result)) {
				// TODO:
			}
			
			delete returnValue;
		}
	}
	
	SDL_Event event;
	event.type = SDL_KEYDOWN;
	event.key.keysym.sym = SDLK_SPACE;
	SDL_PushEvent(&event);
	
	playerPaused = false;
	
	// Send status update to clients.
	sendGlobalStatusUpdate();
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- PLAYBACK STOP ---
// uint8 playback_stop()
NymphMessage* playback_stop(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	if (serverMode == NCS_MODE_MASTER) {
		for (int i = 0; i < slave_remotes.size(); ++i) {
			NymphCastSlaveRemote& rm = slave_remotes[i];
			std::vector<NymphType*> values;
			std::string result;
			NymphType* returnValue = 0;
			if (!NymphRemoteServer::callMethod(rm.handle, "playback_stop", values, returnValue, result)) {
				// TODO:
			}
			
			delete returnValue;
		}
	}
	
	SDL_Event event;
	event.type = SDL_KEYDOWN;
	event.key.keysym.sym = SDLK_ESCAPE;
	SDL_PushEvent(&event);
	
	playerPaused = false;
	playerStopped = true;
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- PLAYBACK PAUSE ---
// uint8 playback_pause()
NymphMessage* playback_pause(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	if (serverMode == NCS_MODE_MASTER) {
		for (int i = 0; i < slave_remotes.size(); ++i) {
			NymphCastSlaveRemote& rm = slave_remotes[i];
			std::vector<NymphType*> values;
			std::string result;
			NymphType* returnValue = 0;
			if (!NymphRemoteServer::callMethod(rm.handle, "playback_pause", values, returnValue, result)) {
				// TODO:
			}
			
			delete returnValue;
		}
	}
	
	SDL_Event event;
	event.type = SDL_KEYDOWN;
	event.key.keysym.sym = SDLK_p;
	SDL_PushEvent(&event);
	
	playerPaused = !playerPaused;
	
	// Send status update to clients.
	sendGlobalStatusUpdate();
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- PLAYBACK REWIND ---
// uint8 playback_rewind()
// TODO:
NymphMessage* playback_rewind(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- PLAYBACK FORWARD ---
// uint8 playback_forward()
// TODO:
NymphMessage* playback_forward(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- CYCLE AUDIO ---
// uint8 cycle_audio()
NymphMessage* cycle_audio(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	if (serverMode == NCS_MODE_MASTER) {
		for (int i = 0; i < slave_remotes.size(); ++i) {
			NymphCastSlaveRemote& rm = slave_remotes[i];
			std::vector<NymphType*> values;
			std::string result;
			NymphType* returnValue = 0;
			if (!NymphRemoteServer::callMethod(rm.handle, "cycle_audio", values, returnValue, result)) {
				// TODO:
			}
			
			delete returnValue;
		}
	}
	
	SDL_Event event;
	event.type = SDL_KEYDOWN;
	event.key.keysym.sym = SDLK_a;
	SDL_PushEvent(&event);
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- PLAYBACK SEEK ---
// uint8 playback_seek(uint64)
NymphMessage* playback_seek(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	std::vector<NymphType*>* valArray = msg->parameters()[0]->getArray();
	NymphSeekType type = static_cast<NymphSeekType>((*valArray)[0]->getUint8());
	if (type == NYMPH_SEEK_TYPE_PERCENTAGE) {
		uint8_t percentage = (*valArray)[1]->getUint8();
		
		// Sanity check.
		// We accept a value from 0 - 100.
		if (percentage > 100) { percentage = 100; }
	
		// Create mouse event structure.
		SDL_Event event;
		event.type = nymph_seek_event;
		event.user.code = NYMPH_SEEK_EVENT;
		event.user.code = percentage;
		SDL_PushEvent(&event);
	}
	else if (type == NYMPH_SEEK_TYPE_BYTES) {
		NYMPH_LOG_FATAL("Error: Seeking by byte offset is not implemented.");
		returnMsg->setResultValue(new NymphType((uint8_t) 1));
		msg->discard();
		return returnMsg;
	}
	else {
		NYMPH_LOG_FATAL("Error: Unknown seeking type requested: " + 
										Poco::NumberFormatter::format(type));
		returnMsg->setResultValue(new NymphType((uint8_t) 1));
		msg->discard();
		return returnMsg;
	}
	
	returnMsg->setResultValue(new NymphType((uint8_t) 0));
	msg->discard();
	
	return returnMsg;
}


// --- PLAYBACK URL ---
// uint8 playback_url(string)
NymphMessage* playback_url(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
	
	std::string url = msg->parameters()[0]->getString();
	bool ret = streamTrack(url);
	
	NymphType* retval = new NymphType((uint8_t) 1);
	if (ret) {
		if (serverMode == NCS_MODE_MASTER) {
			std::vector<NymphType*> values;
			std::string* tUrl = new std::string(url);
			values.push_back(new NymphType(tUrl, true));
			for (int i = 0; i < slave_remotes.size(); ++i) {
				NymphCastSlaveRemote& rm = slave_remotes[i];
				std::string result;
				NymphType* returnValue = 0;
				if (!NymphRemoteServer::callMethod(rm.handle, "playback_url", values, returnValue, result)) {
					// TODO:
				}
				
				delete returnValue;
			}
		}
		
		retval->setValue((uint8_t) 0);
	}
	
	// Send status update to client.
	sendStatusUpdate(DataBuffer::getSessionHandle());
	
	returnMsg->setResultValue(retval);
	msg->discard();
	
	return returnMsg;
}
	


// --- PLAYBACK STATUS ---
// struct playback_status()
NymphMessage* playback_status(int session, NymphMessage* msg, void* data) {
	NymphMessage* returnMsg = msg->getReplyMessage();
		
	returnMsg->setResultValue(new NymphType(getPlaybackStatus(), true));
	msg->discard();
	
	return returnMsg;
}
// --- END Callbacks ---


// --- LOG FUNCTION ---
void logFunction(int level, std::string logStr) {
#ifdef ESP_PLATFORM
	if (level == NYMPH_LOG_LEVEL_FATAL || level == NYMPH_LOG_LEVEL_CRITICAL || 
		level == NYMPH_LOG_LEVEL_ERROR) {
		ESP_LOGE(TAG, "%s", logStr.c_str());
	}
	else if (level == NYMPH_LOG_LEVEL_WARNING) {
		ESP_LOGW(TAG, "%s", logStr.c_str());
	}
	else if (level == NYMPH_LOG_LEVEL_NOTICE || level == NYMPH_LOG_LEVEL_INFO) {
		ESP_LOGI(TAG, "%s", logStr.c_str());
	}
	else if (level == NYMPH_LOG_LEVEL_DEBUG) {
		ESP_LOGD(TAG, "%s", logStr.c_str());
	}
	else if (level == NYMPH_LOG_LEVEL_TRACE) {
		ESP_LOGV(TAG, "%s", logStr.c_str());
	}
#else
	//
	std::cout << level << " - " << logStr << std::endl;
#endif
}


// --- READ LINE ---
// Read a UART line, with configurable timeout in milliseconds.
char* readLine(uart_port_t uart, uint32_t timeout) {
	static char line[256];
	int size;
	char *ptr = line;
	const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
	int counter = 0; // For timeout.
	while(1) {
		size = uart_read_bytes(uart, (unsigned char*) ptr, 1, xDelay);
		if (size == 1) {
			if (*ptr == '\r' || *ptr == '\n') {
				*ptr = 0;
				return line;
			}
			
			counter = 0;
			ptr++;
		}
		else {
			// Timeout was reached, add to counter
			counter += 100;
			if (counter >= timeout) {
				// Return with null pointer to indicate timeout.
				return 0;
			}
		}
			
		vTaskDelay(1);
	} // End of loop
} // End of readLine


// --- UART TASK ---
// The UART console task is started at boot. After 3 seconds it'll begin to broadcast a 
// 'Press key for config' message on stdout (UART0).
// If no key is pressed, after 10 seconds a timeout is reached and the task exits.
// If a key is pressed, enter the console:
// - Print available commands:
// 		- ssid	=> display SSID stored in NVS.
//		- pass	=> display stored WiFi password.
//		- name	=> display stored hostname.
//		- ssidS <ssid>	=> set new SSID (provide SSID following command).
//		- passS <pass>	=> set new WiFi password.
//		- nameS <name>	=> set new hostname.
//		- help	=> Show the command list.
//		- quit	=> exit the console.
//
nvs_handle_t nvsHandle;
Poco::Thread uartTask("uartConsole");
void uartConsole(void* /*arg*/) {
	// Enter timeout loop after 3 second pause.
	const TickType_t xDelay = 3000 / portTICK_PERIOD_MS;
	vTaskDelay(xDelay);
	
	const uart_port_t uart_num = UART_NUM_0;
	int length = 128;
	uint8_t data[length];
	int msec = 0;
	int sec = 0;
	int sec_last = -1;

	struct scoped_cleanup {
		~scoped_cleanup() {
			ESP_ERROR_CHECK(uart_driver_delete(uart_num));
			setvbuf(stdout, NULL, _IOLBF, 0);	// restore line-buffered output
		}
	};

	// Install UART driver not using an event queue:
	ESP_ERROR_CHECK(uart_driver_install(uart_num, 130, 0, 0, NULL, 0));

	scoped_cleanup cleanup;

	while (1) {
		if (sec > sec_last) {
			sec_last = sec;
			printf("[%d/10] Press key for shell...\n", sec);
		}
		
		// Wait for key press for 10 seconds.
		// If key press, break loop and continue, else exit task.
		/* while (length == 0) {
			ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*) &length));
		} */
		
		// Read a single character, wait for 100 ms.
		length = uart_read_bytes(uart_num, data, 1, 100 / portTICK_PERIOD_MS);

		if (length == -1) {
			printf("uartConsole(): Error reading from console '%d'\n", uart_num);
		}
		
		if (length > 0) { break; }
		
		msec += 100;
		if (msec >= 1000) {
			sec++;
			msec = 0;
			if (sec >= 10) { return; } // Time-out.
		}
		
		vTaskDelay(1);
	}
	
	// Enter console loop.
	// A command is a string of characters followed by a newline character (\n) or carriage return (\r).
	setvbuf(stdout, NULL, _IONBF, 0);	// non-buffered output, to allow for `printf("> ");` below
	bool changed = false;
	while (1) {
		// Print command list.
		printf("For command list, type help.\n");
		printf("> ");
		
		// Read from UART until newline. Enter timeout after 30 seconds.
		const char* line = readLine(uart_num, 30000);
		if (line == 0) {
			// Timeout on reading line. Exit task.
			printf("Timeout with console. Exiting task.\n");
			return;
		}
		
		vTaskDelay(1);
		
		// Echo command.
		printf("%s\n", line);
		
		// Process command.
		if (strncmp(line, "help", 4) == 0) {
			printf("Command list:\n");
			printf("quit, ssid, pass, name\n");
			printf("ssidS <ssid>, passS <pass>, nameS <name>\n");
		}
		else if (strncmp(line, "ssidS ", 6) == 0) {
			// Split on space, set SSID to trailing string.
			char* data = strstr(line, " ");
			nvs_set_str(nvsHandle, "wifi_ssid", data);
			changed = true;
		}
		else if (strncmp(line, "passS ", 6) == 0) {
			// Split on space, set password to trailing string.
			char* data = strstr(line, " ");
			nvs_set_str(nvsHandle, "wifi_pass", data);
			changed = true;
		}
		else if (strncmp(line, "nameS ", 6) == 0) {
			// Split on space, set hostname to trailing string.
			char* data = strstr(line, " ");
			nvs_set_str(nvsHandle, "hostname", data);
			changed = true;
		}
		else if (strncmp(line, "ssid", 4) == 0) {
			// Read SSID.
			printf("SSID: %s\n", wifi_ssid.c_str());
		}
		else if (strncmp(line, "pass", 4) == 0) {
			printf("WiFi pass: %s\n", wifi_pass.c_str());
		}
		else if (strncmp(line, "name", 4) == 0) {
			printf("Hostname: %s\n", hostName.c_str());
		}
		else if (strncmp(line, "quit", 4) == 0) {
			// If we made changes to the configuration, reboot, otherwise exit task.
			if (changed) {
				// Reboot.
				printf("Settings changed. Restarting system...\n");
				esp_restart();
			}
			
			printf("Exiting console...\n");
			return;
		}
		else {
			printf("Unknown command. Ignoring...\n");
		}
	}
}


#ifdef ESP_PLATFORM
extern "C" {
	void app_main(void);
}


void app_main() {
	//Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
	  ESP_ERROR_CHECK(nvs_flash_erase());
	  ret = nvs_flash_init();
	}
	
	ESP_ERROR_CHECK(ret);
	
	// Open the WiFi namespace.
	esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvsHandle);
	if (err != ESP_OK) {
		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	}
	
	// Check NVS for WiFi SSID & password, as well as host name.
	bool nvs_saved = true;
	bool nvs_name = true;
	size_t required_size;
	err = nvs_get_str(nvsHandle, "wifi_ssid", NULL, &required_size);
	switch (err) {
		case ESP_OK: {
			char* ssid = (char*) malloc(required_size);
			nvs_get_str(nvsHandle, "wifi_ssid", ssid, &required_size);
			wifi_ssid = std::string(ssid, required_size);
			free(ssid);
			break;
		}
		case ESP_ERR_NVS_NOT_FOUND: {
			// No SSID set in NVS. Set to hardcoded wifi_ssid if found.
			if (wifi_ssid.size() > 1) {
				// Save to NVS.
				nvs_set_str(nvsHandle, "wifi_ssid", wifi_ssid.c_str());
				nvs_saved = true;
			}
			else {
				nvs_saved = false;
			}
		
			break;
		}
		default :
			printf("Error (%s) reading!\n", esp_err_to_name(err));
	}
	
	err = nvs_get_str(nvsHandle, "wifi_pass", NULL, &required_size);
	switch (err) {
		case ESP_OK: {
			char* pass = (char*) malloc(required_size);
			nvs_get_str(nvsHandle, "wifi_pass", pass, &required_size);
			wifi_pass = std::string(pass, required_size);
			free(pass);
			break;
		}
		case ESP_ERR_NVS_NOT_FOUND: {
			// No password set, save to NVS later.
			if (wifi_pass.size() > 1) {
				// Save to NVS.
				nvs_set_str(nvsHandle, "wifi_pass", wifi_pass.c_str());
				nvs_saved = true;
			}
			else {
				nvs_saved = false;
			}
			
			break;
		}
		default :
			printf("Error (%s) reading!\n", esp_err_to_name(err));
	}
	
	err = nvs_get_str(nvsHandle, "hostname", NULL, &required_size);
	switch (err) {
		case ESP_OK: {
			char* name = (char*) malloc(required_size);
			nvs_get_str(nvsHandle, "hostname", name, &required_size);
			hostName = std::string(name, required_size);
			free(name);
			break;
		}
		case ESP_ERR_NVS_NOT_FOUND:{
			// No hostname set, save to NVS later.
			if (hostName.size() > 1) {
				// Save to NVS.
				nvs_set_str(nvsHandle, "hostname", hostName.c_str());
				nvs_name = true;
			}
			else {
				hostName = "NCA-ESP32";
				nvs_name = false;
			}
			
			break;
		}
		default :
			printf("Error (%s) reading!\n", esp_err_to_name(err));
	}
	
	// Start the console module. If no WiFi details were defined or found in NVS, skip connecting.
	// TODO:
	uartTask.start(uartConsole, 0);
	
	if (!nvs_saved) {
		// No WiFi credentials available, skip setting up NCA.
		// We just exit here and allow the user to set credentials in the console.
		printf("Error: no WiFi credentials. Please set via console or header.\n");
		return;
	}

	// Set up WiFi.
	ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
	if (!wifi_init_sta()) {
		ESP_LOGE(TAG, "");
		
		// Connecting to WiFi failed, wait for user to connect to interactive shell.
		// This allows for the user to set the WiFi details & host name before retrying.
		// TODO:
		
		
		return;
	}
	
	// Start the NTP task to update the RTC clock.
	esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
	esp_netif_sntp_init(&config);
#else
int main() {
#endif
	// This embedded version of NymphCast Audio has no configuration settings, but runs as follows:
	// - Audio-only.
	// - Fixed 1 MB in-RAM buffer.
	// - Playback supported of audio formats which the ESP SDK supports.
	// - RTSP audio stream support (?)
	// - AngelScript-based NC Apps are desirable => check RAM usage.
	//
	// Specific hardware target is the ESP32 with 4+ MB PSRAM expansion (e.g. ESP32-WROVER-B).
	// For ESP32 boards without in-module PSRAM (e.g. ESP-WROOM-32), external PSRAM must be
	// connected.
	video_disable = 1;
	display_disable = 1;
	audio_disable = 0;
	
#ifdef ESP_PLATFORM
	// Debug
	uint32_t free_size = (uint32_t) heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	uint32_t free_block = (uint32_t) heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	NYMPH_LOG_INFORMATION("HEAP Free Size: " + Poco::NumberFormatter::format(free_size) 
							+ ", Free Block: " + Poco::NumberFormatter::format(free_block));
	// ---
#endif
	
	// Initialise server.
	std::cout << "Initialising server...\n";
	long timeout = 5000; // 5 seconds.
	NymphRemoteClient::init(logFunction, NYMPH_LOG_LEVEL_WARNING, timeout);
	//NymphRemoteClient::init(logFunction, NYMPH_LOG_LEVEL_INFO, timeout);
	//NymphRemoteClient::init(logFunction, NYMPH_LOG_LEVEL_DEBUG, timeout);
	
#ifdef ESP_PLATFORM
	// Debug
	free_size = (uint32_t) heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	free_block = (uint32_t) heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	NYMPH_LOG_INFORMATION("HEAP Free Size: " + Poco::NumberFormatter::format(free_size) 
							+ ", Free Block: " + Poco::NumberFormatter::format(free_block));
	// ---
#endif
	
	// Set up NymphCast RPC calls.
	std::vector<NymphTypes> parameters;
	
	// Client connects to server.
	// bool connect(string client_id)
	parameters.push_back(NYMPH_STRING);
	NymphMethod connectFunction("connect", parameters, NYMPH_BOOL, connectClient);
	NymphRemoteClient::registerMethod("connect", connectFunction);
	
	// Master server calls this to turn this server instance into a slave.
	// uint8 connectMaster(sint64)
	parameters.clear();
	parameters.push_back(NYMPH_SINT64);
	NymphMethod connectMasterFunction("connectMaster", parameters, NYMPH_SINT64, connectMaster);
	NymphRemoteClient::registerMethod("connectMaster", connectMasterFunction);
	
	// Receives data chunks for playback.
	// uint8 receiveDataMaster(blob data, bool done, sint64)
	parameters.clear();
	parameters.push_back(NYMPH_STRING);
	parameters.push_back(NYMPH_BOOL);
	parameters.push_back(NYMPH_SINT64);
	NymphMethod receivedataMasterFunction("receiveDataMaster", parameters, NYMPH_UINT8, receiveDataMaster);
	NymphRemoteClient::registerMethod("receiveDataMaster", receivedataMasterFunction);
	
	// Receives data chunks for playback.
	// uint8 slave_start(sint64 when)
	parameters.clear();
	parameters.push_back(NYMPH_SINT64);
	NymphMethod slaveStartFunction("slave_start", parameters, NYMPH_UINT8, slave_start);
	NymphRemoteClient::registerMethod("slave_start", slaveStartFunction);
	
	// Client disconnects from server.
	// bool disconnect()
	parameters.clear();
	NymphMethod disconnectFunction("disconnect", parameters, NYMPH_BOOL, disconnect);
	NymphRemoteClient::registerMethod("disconnect", disconnectFunction);
	
	// Client starts a session.
	// Return value: OK (0), ERROR (1).
	// int session_start()
	parameters.clear();
	parameters.push_back(NYMPH_STRUCT);
	NymphMethod sessionStartFunction("session_start", parameters, NYMPH_UINT8, session_start);
	NymphRemoteClient::registerMethod("session_start", sessionStartFunction);
	
	// Client adds slave NymphCast servers to the session.
	// Any slaves will follow the master server exactly when it comes to playback.
	// Returns: OK (0), ERROR (1).
	// int session_add_slave(array servers);
	parameters.clear();
	parameters.push_back(NYMPH_ARRAY);
	NymphMethod sessionAddSlaveFunction("session_add_slave", parameters, NYMPH_UINT8, session_add_slave);
	NymphRemoteClient::registerMethod("session_add_slave", sessionAddSlaveFunction);
	
	// Client sends a chunk of track data.
	// Returns: OK (0), ERROR (1).
	// int session_data(string buffer)
	parameters.clear();
	parameters.push_back(NYMPH_STRING);
	parameters.push_back(NYMPH_BOOL);
	NymphMethod sessionDataFunction("session_data", parameters, NYMPH_UINT8, session_data);
	NymphRemoteClient::registerMethod("session_data", sessionDataFunction);
	
	// Client ends the session.
	// Returns: OK (0), ERROR (1).
	// int session_end()
	parameters.clear();
	NymphMethod sessionEndFunction("session_end", parameters, NYMPH_UINT8, session_end);
	NymphRemoteClient::registerMethod("session_end", sessionEndFunction);
	
	// Reset slave data buffer.
	// Returns: OK (0), ERROR (1).
	// int slave_buffer_reset()
	parameters.clear();
	NymphMethod slaveBufferResetFunction("slave_buffer_reset", parameters, NYMPH_UINT8, slave_buffer_reset);
	NymphRemoteClient::registerMethod("slave_buffer_reset", slaveBufferResetFunction);
	
	// Playback control methods.
	//
	// VolumeSet.
	// uint8 volume_set(uint8 volume)
	// Set volume to between 0 - 100.
	// Returns new volume setting or >100 if failed.
	parameters.clear();
	parameters.push_back(NYMPH_UINT8);
	NymphMethod volumeSetFunction("volume_set", parameters, NYMPH_UINT8, volume_set);
	NymphRemoteClient::registerMethod("volume_set", volumeSetFunction);
	
	// VolumeUp.
	// uint8 volume_up()
	// Increase volume by 10 up to 100.
	// Returns new volume setting or >100 if failed.
	parameters.clear();
	NymphMethod volumeUpFunction("volume_up", parameters, NYMPH_UINT8, volume_up);
	NymphRemoteClient::registerMethod("volume_up", volumeUpFunction);
		
	// VolumeDown.
	// uint8 volume_down()
	// Decrease volume by 10 up to 100.
	// Returns new volume setting or >100 if failed.
	parameters.clear();
	NymphMethod volumeDownFunction("volume_down", parameters, NYMPH_UINT8, volume_down);
	NymphRemoteClient::registerMethod("volume_down", volumeDownFunction);
		
	// VolumeMute.
	// uint8 volume_mute()
	// Toggle muting audio volume.
	// Returns 0 if succeeded.
	parameters.clear();
	NymphMethod volumeMuteFunction("volume_mute", parameters, NYMPH_UINT8, volume_mute);
	NymphRemoteClient::registerMethod("volume_mute", volumeMuteFunction);
	
	// PlaybackStart.
	// uint8 playback_start()
	// Start playback.
	// Returns success or error number.
	parameters.clear();
	NymphMethod playbackStartFunction("playback_start", parameters, NYMPH_UINT8, playback_start);
	NymphRemoteClient::registerMethod("playback_start", playbackStartFunction);
	
	// PlaybackStop.
	// uint8 playback_stop()
	// Stop playback.
	// Returns success or error number.
	parameters.clear();
	NymphMethod playbackStopFunction("playback_stop", parameters, NYMPH_UINT8, playback_stop);
	NymphRemoteClient::registerMethod("playback_stop", playbackStopFunction);
	
	// PlaybackPause.
	// uint8 playback_pause()
	// Pause playback.
	// Returns success or error number.
	parameters.clear();
	NymphMethod playbackPauseFunction("playback_pause", parameters, NYMPH_UINT8, playback_pause);
	NymphRemoteClient::registerMethod("playback_pause", playbackPauseFunction);
	
	// PlaybackRewind.
	// uint8 playback_rewind()
	// Rewind the current file to the beginning.
	// Returns success or error number.
	parameters.clear();
	NymphMethod playbackRewindFunction("playback_rewind", parameters, NYMPH_UINT8, playback_rewind);
	NymphRemoteClient::registerMethod("playback_rewind", playbackRewindFunction);
	
	// PlaybackForward
	// uint8 playback_forward()
	// Forward the current file to the end.
	// Returns success or error number.
	parameters.clear();
	NymphMethod playbackForwardFunction("playback_forward", parameters, NYMPH_UINT8, playback_forward);
	NymphRemoteClient::registerMethod("playback_forward", playbackForwardFunction);
	
	// uint8 cycle_audio()
	// Cycle audio channel.
	// Returns success or error number.
	parameters.clear();
	NymphMethod cycleAudioFunction("cycle_audio", parameters, NYMPH_UINT8, cycle_audio);
	NymphRemoteClient::registerMethod("cycle_audio", cycleAudioFunction);
	
	// PlaybackSeek
	// uint8 playback_seek(uint64)
	// Seek to the indicated position.
	// Returns success or error number.
	parameters.clear();
	parameters.push_back(NYMPH_ARRAY);
	NymphMethod playbackSeekFunction("playback_seek", parameters, NYMPH_UINT8, playback_seek);
	NymphRemoteClient::registerMethod("playback_seek", playbackSeekFunction);
	
	// PlaybackUrl.
	// uint8 playback_url(string)
	// Try to the play the media file indicated by the provided URL.
	// Returns success or error number.
	parameters.clear();
	parameters.push_back(NYMPH_STRING);
	NymphMethod playbackUrlFunction("playback_url", parameters, NYMPH_UINT8, playback_url);
	NymphRemoteClient::registerMethod("playback_url", playbackUrlFunction);
	
	// PlaybackStatus
	// struct playback_status()
	// The current state of the NymphCast server.
	// Return struct with information:
	// ["playing"] => boolean (true/false)
	// 
	parameters.clear();
	NymphMethod playbackStatusFunction("playback_status", parameters, NYMPH_STRUCT, playback_status);
	NymphRemoteClient::registerMethod("playback_status", playbackStatusFunction);
	
	// Register client callbacks
	//
	// void MediaReadCallback(uint32 blockSize)
	parameters.clear();
	parameters.push_back(NYMPH_UINT32);
	NymphMethod mediaReadCallback("MediaReadCallback", parameters, NYMPH_NULL);
	mediaReadCallback.enableCallback();
	NymphRemoteClient::registerCallback("MediaReadCallback", mediaReadCallback);
	
	// MediaStopCallback
	parameters.clear();
	//parameters.push_back(NYMPH_STRING);
	NymphMethod mediaStopCallback("MediaStopCallback", parameters, NYMPH_NULL);
	mediaStopCallback.enableCallback();
	NymphRemoteClient::registerCallback("MediaStopCallback", mediaStopCallback);
	
	// MediaSeekCallback
	// Sends the desired byte position in the open file to seek to.
	// void MediaSeekCallback(uint64 position, uint32 blockSize)
	parameters.clear();
	parameters.push_back(NYMPH_UINT64);
	parameters.push_back(NYMPH_UINT32);
	NymphMethod mediaSeekCallback("MediaSeekCallback", parameters, NYMPH_NULL);
	mediaSeekCallback.enableCallback();
	NymphRemoteClient::registerCallback("MediaSeekCallback", mediaSeekCallback);
	
	// MediaStatusCallback
	// Sends a struct with this remote's playback status to the client.
	// void MediaStatusCallback(struct)
	parameters.clear();
	parameters.push_back(NYMPH_STRUCT);
	NymphMethod mediaStatusCallback("MediaStatusCallback", parameters, NYMPH_NULL);
	mediaStatusCallback.enableCallback();
	NymphRemoteClient::registerCallback("MediaStatusCallback", mediaStatusCallback);
	
	// Master-Slave registrations.
	using namespace std::placeholders; 
	NymphRemoteServer::registerCallback("MediaReadCallback", MediaReadCallback, 0);
	NymphRemoteServer::registerCallback("MediaSeekCallback", MediaSeekCallback, 0);
	NymphRemoteServer::registerCallback("MediaStopCallback", MediaStopCallback, 0);
	NymphRemoteServer::registerCallback("MediaStatusCallback", MediaStatusCallback, 0);
	
#ifdef ESP_PLATFORM
	// Debug
	free_size = (uint32_t) heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	free_block = (uint32_t) heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	NYMPH_LOG_INFORMATION("HEAP Free Size: " + Poco::NumberFormatter::format(free_size) 
							+ ", Free Block: " + Poco::NumberFormatter::format(free_block));
	// ---
#endif
	
	// Initialise buffer.
	NYMPH_LOG_INFORMATION("Setting up DataBuffer...");
	uint32_t buffer_size = 1048576; // 1 MB
	DataBuffer::init(buffer_size);
	DataBuffer::setSeekRequestCallback(seekingHandler);
	DataBuffer::setDataRequestCallback(dataRequestHandler);
	
	// Initialise SDL.
	if (!SdlRenderer::init()) {
		NYMPH_LOG_FATAL("Failed to init SDL. Aborting...");
#ifdef ESP_PLATFORM
		return;
#else
		return 1;
#endif
	}
	
#ifdef ESP_PLATFORM
	// Debug
	free_size = (uint32_t) heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	free_block = (uint32_t) heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	NYMPH_LOG_INFORMATION("HEAP Free Size: " + Poco::NumberFormatter::format(free_size) 
							+ ", Free Block: " + Poco::NumberFormatter::format(free_block));
	// ---
#endif
	
	// Start NymphRPC server in its own thread.
	NYMPH_LOG_INFORMATION("Starting NymphRPC server...");
	NymphRemoteClient::start(4004);
	
#ifdef ESP_PLATFORM
	// Debug
	free_size = (uint32_t) heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	free_block = (uint32_t) heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	NYMPH_LOG_INFORMATION("HEAP Free Size: " + Poco::NumberFormatter::format(free_size) 
							+ ", Free Block: " + Poco::NumberFormatter::format(free_block));
	// ---
#endif
	
	// Start NyanSD server.
	NYSD_service sv;
	sv.port = 4004;
	sv.protocol = NYSD_PROTOCOL_TCP;
	sv.service = "nymphcast-audio";
	NyanSD::addService(sv);
	
	sv.port = 4004;
	sv.protocol = NYSD_PROTOCOL_TCP;
	sv.service = "nymphcast";
	NyanSD::addService(sv);
	
#ifdef ESP_PLATFORM
	// Debug
	free_size = (uint32_t) heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	free_block = (uint32_t) heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	NYMPH_LOG_INFORMATION("HEAP Free Size: " + Poco::NumberFormatter::format(free_size) 
							+ ", Free Block: " + Poco::NumberFormatter::format(free_block));
	// ---
#endif
	
	NYMPH_LOG_INFORMATION("Starting NyanSD on port 4004 UDP...");
	NyanSD::startListener(4004);
	
#ifdef ESP_PLATFORM
	// Debug
	free_size = (uint32_t) heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	free_block = (uint32_t) heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	NYMPH_LOG_INFORMATION("HEAP Free Size: " + Poco::NumberFormatter::format(free_size) 
							+ ", Free Block: " + Poco::NumberFormatter::format(free_block));
	// ---
#endif
	
	// Start AV thread.
	NYMPH_LOG_INFORMATION("Starting ffplay...");
#ifdef ESP_PLATFORM
	avThread.setStackSize(12288); 	// 12 kB.
	avThread.useExternalRAM(true); 	// Use external RAM for stack.
#endif
	avThread.start(ffplay);
	
#ifdef ESP_PLATFORM
	// Debug
	free_size = (uint32_t) heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	free_block = (uint32_t) heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	NYMPH_LOG_INFORMATION("HEAP Free Size: " + Poco::NumberFormatter::format(free_size) 
							+ ", Free Block: " + Poco::NumberFormatter::format(free_block));
	// ---
#endif
	
	// Start SDL event loop in separate threan on ESP32.
	NYMPH_LOG_INFORMATION("Starting SDL event loop...");
#ifdef ESP_PLATFORM
	//sdlThread.setStackSize(1024);	// 1 kB
	sdlThread.setStackSize(7168);	// 7 kB
	sdlThread.start(SdlRenderer::run_event_loop);
#else
	SdlRenderer::run_event_loop();
#endif
	
	NYMPH_LOG_INFORMATION("Main thread: Shutting down...");
	
	// On ESP32 here the main thread exits while the other tasks continue.
#ifndef ESP_PLATFORM
	// Clean-up
	DataBuffer::cleanup();
	running = false;
 
	// Close window and clean up libSDL.
	ffplay.quit();
	avThread.join();
	SdlRenderer::quit();
	
	NYMPH_LOG_INFORMATION("Stopped SDL loop. Shutting down server threads.");
	
	NyanSD::stopListener();
	NymphRemoteClient::shutdown();
	
	// Wait before exiting, giving threads time to exit.
	Poco::Thread::sleep(2000); // 2 seconds.
	return 1;
#endif
}
