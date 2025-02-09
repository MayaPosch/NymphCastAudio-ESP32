#ifndef ESP_PLATFORM
// Do absolutely nothing.

#else
//#include "SDL.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <unistd.h>

#include <queue>

// ESP-IDF 5.x version
//#include "driver/i2s_std.h"

//i2s_chan_handle_t tx_chan = nullptr;

// ESP-IDF 4.4.x version.
#include "driver/i2s.h"

#include <ffplay/audio_renderer.h>
#include <ffplay/sdl_renderer.h>
#include <ffplay/player.h>

#include "clock.h"

extern std::string hostName;

std::queue<SDL_Event> sdl_events;

std::atomic<bool> SdlRenderer::run_events;
std::atomic<bool> SdlRenderer::playerEventsActive = { false };

SDL_RendererInfo renderer_info = {0};

/* 
#define CONFIG_EXAMPLE_I2S_LRCK_PIN 22
#define CONFIG_EXAMPLE_I2S_DATA_PIN 25
#define CONFIG_EXAMPLE_I2S_BCK_PIN 26 */
#define CONFIG_EXAMPLE_I2S_LRCK_PIN 15
#define CONFIG_EXAMPLE_I2S_DATA_PIN 13
#define CONFIG_EXAMPLE_I2S_BCK_PIN 14

static bool i2s_started = false;

/* #include "esp_log.h"
#define I2S_TAG       "I2S_AV" */


// -- INIT ---
bool SdlRenderer::init() {
	// Set up I2S interface.
	
	return true;
}


// --- QUIT ---
void SdlRenderer::quit() {
	// Shutdown I2S interface.
}


// --- 
void SdlRenderer::showWindow() {
	// Do nothing.
}

// --- 
void SdlRenderer::hideWindow() {
	// Do nothing.
}

// --- 
void SdlRenderer::setShowWindow(bool show) {
	// Do nothing.
}

// --- 
void SdlRenderer::resizeWindow(int width, int height) {
	// Do nothing.
}

// --- 
void SdlRenderer::set_default_window_size(int width, int height, AVRational sar) {
	// Do nothing.
}

// --- 
void SdlRenderer::set_fullscreen(bool fullscreen) {
	// Do nothing.
}

// --- 
void SdlRenderer::video_display(VideoState *is) {
	// Do nothing.
}

// --- 
void SdlRenderer::image_display(std::string image) {
	// Do nothing.
}

// --- 
void SdlRenderer::run_event_loop(void* arg) {
	run_events = true;
	SDL_Event event;
	const TickType_t xDelay = 10 / portTICK_PERIOD_MS;
	while (run_events) {
		while (SDL_PollEvent(&event)) {
			if (playerEventsActive) {
				// Pass through player processor.
				Player::process_event(event);
			}
		}
			
		if (playerEventsActive) {
			// Trigger player refresh.
			Player::run_updates();
		}
		
		// Wait for 10 ms.
		vTaskDelay(xDelay);
		//usleep(10);
	}
}

// --- 
void SdlRenderer::stop_event_loop() {
	run_events = false;
}

// --- 
void SdlRenderer::playerEvents(bool active) {
	playerEventsActive = active;
}

// --- 
void SdlRenderer::guiEvents(bool active) {
	// Do nothing.
}

// --- 
void SdlRenderer::screensaverUpdate(std::string path) {
	// Do nothing.
}

// --- 
void SdlRenderer::video_audio_display(VideoState *s) {
	// Do nothing.
}

// --- 
void SdlRenderer::video_image_display(VideoState *is) {
	// Do nothing.
}


// --- Audio Task ---
Poco::Thread audio_thread("audio_task");
bool audio_thread_active;
void audio_task(void* arg) {
	// Writing to the I2S buffer is a blocking operation, so we read from the internal buffer
	// whenever we can.
	VideoState* is = (VideoState*) arg;
	
	audio_thread_active = true;
	int audio_size, len1;
	size_t bytes_written = 0;
	while (audio_thread_active) {
		audio_callback_time = av_gettime_relative();
		
		//if (is->audio_buf_index >= is->audio_buf_size) {
			audio_size = AudioRenderer::audio_decode_frame(is);
			if (audio_size < 0) {
				// No data available: just output silence.
				// Since the DMA buffers are zeroed by ESP-IDF (since ESP-IDF issue #1789, 2018),
				// we only need to write to I2S when there is data to send.
				//is->audio_buf = NULL;
				//is->audio_buf_size = SDL_AUDIO_MIN_BUFFER_SIZE / is->audio_tgt.frame_size * is->audio_tgt.frame_size;
		
				// Briefly yield.
				vTaskDelay(1);
				
				continue;
		   } 
		   else {
			   /*if (is->show_mode != SHOW_MODE_VIDEO)
				   VideoRenderer::update_sample_display(is, (int16_t *)is->audio_buf, audio_size);*/
			   is->audio_buf_size = audio_size;
		   }
			
			is->audio_buf_index = 0;
		//}
		
        len1 = is->audio_buf_size; // - is->audio_buf_index;
        //if (len1 > len) { len1 = len; }
        is->audio_buf_index += len1;
		
		uint8_t* data = is->audio_buf;
		size_t item_size = audio_size;
		
		// Debug
		//av_log(NULL, AV_LOG_INFO, "audio_callback_time: %d\n", (int) audio_callback_time);
		
		// ESP-IDF 5.x version.
		//i2s_channel_write(tx_chan, data, item_size, &bytes_written, portMAX_DELAY);
		
		// ESP-IDF 4.4.x version.
		i2s_write((i2s_port_t) 0, data, item_size, &bytes_written, portMAX_DELAY);
	
		is->audio_write_buf_size = is->audio_buf_size - is->audio_buf_index;
		
		if (!isnan(is->audio_clock)) {
			ClockC::set_clock_at(&is->audclk, 
				is->audio_clock - (double)(2 * is->audio_hw_buf_size + 
				is->audio_write_buf_size) / is->audio_tgt.bytes_per_sec, 
				is->audio_clock_serial, audio_callback_time / 1000000.0);
			ClockC::sync_clock_to_slave(&is->extclk, &is->audclk);
		}
		
		// Wait for 1 ms.
		vTaskDelay(xDelay);
	}
}


// --- 
int SdlRenderer::audio_open(void *opaque, int64_t wanted_channel_layout, int wanted_nb_channels, 
		int wanted_sample_rate, struct AudioParams* audio_hw_params) {
	// Open I2S interface.
	// ESP-IDF 5.x version.
	/* i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
	chan_cfg.auto_clear = true;
	i2s_std_config_t std_cfg = {
		.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),
		.slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
		.gpio_cfg = {
			.mclk = I2S_GPIO_UNUSED,
			.bclk = CONFIG_EXAMPLE_I2S_BCK_PIN,
			.ws = CONFIG_EXAMPLE_I2S_LRCK_PIN,
			.dout = CONFIG_EXAMPLE_I2S_DATA_PIN,
			.din = I2S_GPIO_UNUSED,
			.invert_flags = {
				.mclk_inv = false,
				.bclk_inv = false,
				.ws_inv = false,
			},
		},
	};
	
	// enable I2S.
	ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_chan, NULL));
	ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_chan, &std_cfg));
	ESP_ERROR_CHECK(i2s_channel_enable(tx_chan)); */
	
	// ESP-IDF 4.4.x version.
	i2s_config_t i2s_config;// = {
        i2s_config.mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX); //,              /* only TX */
        //i2s_config.sample_rate = 44100; //,
        i2s_config.sample_rate = wanted_sample_rate; //,
        i2s_config.bits_per_sample = (i2s_bits_per_sample_t) 16; //,
        i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT; //,       /* 2-channels */
        i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S; //,
        i2s_config.dma_buf_count = 6; //,
        i2s_config.dma_buf_len = 60; //,
		i2s_config.use_apll = false;
        i2s_config.intr_alloc_flags = 0; //,                              /* default interrupt priority */
        i2s_config.tx_desc_auto_clear = true; //                          /* auto clear tx descriptor on underflow */
		i2s_config.fixed_mclk = 0;
		i2s_config.mclk_multiple = I2S_MCLK_MULTIPLE_512;
		i2s_config.bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT;
    //};

    /// enable I2S.
    i2s_driver_install((i2s_port_t) 0, &i2s_config, 0, NULL);
	
    i2s_pin_config_t pin_config; // = {
		pin_config.mck_io_num = I2S_PIN_NO_CHANGE,
        pin_config.bck_io_num = CONFIG_EXAMPLE_I2S_BCK_PIN; //,
        pin_config.ws_io_num = CONFIG_EXAMPLE_I2S_LRCK_PIN; //,
        pin_config.data_out_num = CONFIG_EXAMPLE_I2S_DATA_PIN; //,
        pin_config.data_in_num = I2S_PIN_NO_CHANGE; //                                   /* not used */
    //};
	
    i2s_set_pin((i2s_port_t) 0, &pin_config);
	
	// Config vars.
	i2s_bits_per_sample_t codec_bps = (i2s_bits_per_sample_t) 0;
	i2s_bits_per_sample_t dac_bps = (i2s_bits_per_sample_t) 0;
	i2s_channel_t i2s_channels = I2S_CHANNEL_STEREO;
	
	// setup sample rate and channels
	//uint32_t sample_rate = wanted_sample_rate;
	i2s_bits_per_sample_t bits_per_sample = (i2s_bits_per_sample_t) 0;
	bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT; // TODO: may need to adjust.
	//i2s_config_t i2s_config;
	i2s_port_t i2s_port = (i2s_port_t) 0;
	i2s_config.sample_rate = (uint32_t) wanted_sample_rate;
	codec_bps = bits_per_sample;
	i2s_config.bits_per_sample = (dac_bps != 0) ? dac_bps : codec_bps;
	//xSemaphoreTake(i2s_lock, portMAX_DELAY);
	if (!i2s_started) {
		// i2s_set_clk stops i2s, sets clk, then starts i2s.
		// start it now if it isn't already
		i2s_start(i2s_port);
	}
	
	esp_err_t err = i2s_set_clk(i2s_port, i2s_config.sample_rate, 
								i2s_config.bits_per_sample, i2s_channels);
	if (err == ESP_OK) {
		i2s_started = true;
		/* ESP_LOGI(I2S_TAG, "audio player configured, samplerate=%d, bits_per_sample=%d",
				 i2s_config.sample_rate, i2s_config.bits_per_sample); */
	} 
	else {
		i2s_started = false;
		//ESP_LOGE(I2S_TAG, "i2s_set_clk failed with samplerate=%d", i2s_config.sample_rate);
		return -1;
	}
	
	// Note: experimental if section.
	if (!wanted_channel_layout || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)) {
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
		wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
	}
	// -- End experimental.
	
	audio_hw_params->fmt = AV_SAMPLE_FMT_S16;
    audio_hw_params->freq = i2s_config.sample_rate;
    audio_hw_params->channel_layout = wanted_channel_layout;
    audio_hw_params->channels = wanted_nb_channels; // spec.channels;
    audio_hw_params->frame_size = av_samples_get_buffer_size(NULL, audio_hw_params->channels, 1, audio_hw_params->fmt, 1);
    audio_hw_params->bytes_per_sec = av_samples_get_buffer_size(NULL, audio_hw_params->channels, audio_hw_params->freq, audio_hw_params->fmt, 1);
    if (audio_hw_params->bytes_per_sec <= 0 || audio_hw_params->frame_size <= 0) {
        av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size failed\n");
        return -1;
    }
	
	// Start I2S write task.
	// Make sure it's pinned to Core 1.
	audio_thread.setCoreId(1);
	audio_thread.start(audio_task, opaque);
	
	return 0;
	//return spec.size;
}

// --- 
void SdlRenderer::audio_pause() {
	// Pause I2S interface?
}

// --- 
void SdlRenderer::audio_close() {
	// Stop I2S write task.
	audio_thread_active = false;
	audio_thread.join();
	
	// Close I2S interface.
	// ESP-IDF 5.x version.
	/* ESP_ERROR_CHECK(i2s_channel_disable(tx_chan));
	ESP_ERROR_CHECK(i2s_del_channel(tx_chan)); */
	
	// ESP-IDF 4.4.x version.
	i2s_driver_uninstall((i2s_port_t) 0);
}

// --- 
void SdlRenderer::texture_destroy(SDL_Texture* texture) {
	// Do nothing.
}


// --- SDL implementations ---
int SDL_PollEvent(SDL_Event* event) {
	// If there's an event available, make it available.
	if (sdl_events.empty()) { return 0; }
	
	*event = sdl_events.front();
	sdl_events.pop();
	
	return 1;
}



void SDL_PumpEvents() {
	// Do nothing.
}


/* int  SDL_PeepEvents(SDL_Event* events, int numevents, SDL_eventaction action,
												Uint32 minType, Uint32 maxType) {
	// Unimplemented.
	return 0;
} */
												
												
int SDL_PushEvent(SDL_Event* event) {
	// Put the event into the queue.
	sdl_events.push(*event);
	
	return 1;
}


int SDL_ShowCursor(int toggle) {
	// Do nothing.
	return toggle;
}


int SDL_LockTexture(SDL_Texture * texture, const SDL_Rect * rect, void **pixels, int *pitch) {
	// Do nothing.
	return 0;
}


void SDL_UnlockTexture(SDL_Texture * texture) {
	// Do nothing.
}


uint32_t SDL_RegisterEvents(int numevents) {
	return 42000;
}


// FIXME: LibAV hacks
void avdevice_register_all() {
	// Nothing.
}


void sws_freeContext(struct SwsContext *swsContext) {
	// Nothing.
}


// FIXME: another hack due to undefined reference error on ESP-IDF.
int gethostname(char *name, size_t len) {
	// TODO: Get ESP-IDF specific identifier (MAC).
	//strncpy(name, "NCA-ESP32", len - 1);
	strncpy(name, hostName.c_str(), len - 1);
	if (len > 9) {
		name[9] = '\0';
	}
	else {
		name[len - 1] = '\0';
	}
	
	return 0;
}

#endif
