CPPFLAGS += __FREERTOS__
CFLAGS += -O0 -Wno-format-truncation -Wno-pointer-sign -Wno-parentheses -Wno-switch -Wno-maybe-uninitialized
COMPONENT_ADD_LDFLAGS += --gc-sections
COMPONENT_ADD_INCLUDEDIRS += .
COMPONENT_OBJS := audioconvert.o \
			dither.o \
			options.o \
			rematrix.o \
			resample.o \
			resample_dsp.o \
			swresample.o \
			swresample_frame.o \
			version.o
COMPONENT_SRCDIRS := libswresample
COMPONENT_ADD_INCLUDEDIRS := libswresample