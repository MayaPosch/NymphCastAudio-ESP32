CPPFLAGS += __FREERTOS__
CFLAGS += -O0 -Wno-format-truncation -Wno-pointer-sign -Wno-parentheses -Wno-switch -Wno-maybe-uninitialized
COMPONENT_ADD_LDFLAGS += --gc-sections
COMPONENT_ADD_INCLUDEDIRS += .
COMPONENT_SRCDIRS := libswscale
COMPONENT_ADD_INCLUDEDIRS := libswscale