# idf_component_register(SRCS "foo.c" "bar.c"
                       # INCLUDE_DIRS "include"
                       # REQUIRES mbedtls)
idf_component_register(SRCS "libavfilter/af_aformat.c" "libavfilter/af_anull.c" "libavfilter/af_aresample.c" "libavfilter/allfilters.c" "libavfilter/audio.c" "libavfilter/avfilter.c" "libavfilter/avfiltergraph.c" "libavfilter/buffersink.c" "libavfilter/buffersrc.c" "libavfilter/colorspace.c" "libavfilter/drawutils.c" "libavfilter/fifo.c" "libavfilter/formats.c" "libavfilter/framepool.c" "libavfilter/framequeue.c" "libavfilter/graphdump.c" "libavfilter/graphparser.c" "libavfilter/pthread.c" "libavfilter/setpts.c" "libavfilter/trim.c" "libavfilter/version.c" "libavfilter/vf_format.c" "libavfilter/vf_null.c" "libavfilter/video.c"
						INCLUDE_DIRS "libavfilter" "."
						REQUIRES libavutil)
						
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format-truncation)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-pointer-sign)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-parentheses)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-switch)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-maybe-uninitialized)