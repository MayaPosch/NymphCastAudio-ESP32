

#ifndef STREAM_HANDLER_H
#define STREAM_HANDLER_H


#include "types.h"
#include "ffplay.h"

#include <atomic>


class StreamHandler {
	static std::string loggerName;
	VideoState* vstate;
	static std::atomic_bool run;
	static std::atomic<bool> eof;
	static FileMetaInfo file_meta;
	
	static void read_thread(void *arg);
	
	static void setPosition(double p);
	static double getPosition() { return file_meta.position; }
	static uint64_t getDuration() { return file_meta.duration; }
	static void setDuration(uint64_t d) { file_meta.setDuration(d); }
	static std::string getTitle() { return file_meta.getTitle(); }
	static std::string getArtist() { return file_meta.getArtist(); }
	static std::string getAlbum() { return file_meta.getAlbum(); }
	
public:
	static VideoState *stream_open(const char *filename, AVInputFormat *iformat, AVFormatContext* context);
	static int stream_component_open(VideoState *is, int stream_index);
	static void stream_close(VideoState *is);
	static int get_master_sync_type(VideoState *is);
	static void stream_toggle_pause(VideoState *is);
	static void stream_seek(VideoState *is, int64_t pos, int64_t rel, int seek_by_bytes);
	static void step_to_next_frame(VideoState *is);
	static void stream_cycle_channel(VideoState *is, int codec_type);
#if CONFIG_AVFILTER
	static int opt_add_vfilter(void *optctx, const char *opt, const char *arg);
#endif
	
	static void quit();
};


#endif
