CPPFLAGS += __FREERTOS__
CFLAGS += -O0 -Wno-pointer-sign -Wno-parentheses -Wno-switch -Wno-maybe-uninitialized
COMPONENT_ADD_LDFLAGS += --gc-sections
COMPONENT_ADD_INCLUDEDIRS += .
COMPONENT_OBJS := af_aformat.o \
			af_anull.o \
			af_aresample.o \
			allfilters.o \
			audio.o \
			avfilter.o \
			avfiltergraph.o \
			buffersink.o \
			buffersrc.o \
			colorspace.o \
			drawutils.o \
			fifo.o \
			formats.o \
			framepool.o \
			framequeue.o \
			graphdump.o \
			graphparser.o \
			pthread.o \
			setpts.o \
			trim.o \
			version.o \
			vf_format.o \
			vf_null.o \
			video.o
COMPONENT_SRCDIRS := libavfilter
COMPONENT_ADD_INCLUDEDIRS := libavfilter