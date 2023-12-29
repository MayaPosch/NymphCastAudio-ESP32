# idf_component_register(SRCS "foo.c" "bar.c"
                       # INCLUDE_DIRS "include"
                       # REQUIRES mbedtls)
idf_component_register(SRCS "libswresample/audioconvert.c" "libswresample/dither.c" "libswresample/options.c" "libswresample/rematrix.c" "libswresample/resample.c" "libswresample/resample_dsp.c" "libswresample/swresample.c" "libswresample/swresample_frame.c" "libswresample/version.c"
						INCLUDE_DIRS "libswresample" "."
						REQUIRES libavutil)
						
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format-truncation)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-pointer-sign)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-parentheses)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-switch)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-maybe-uninitialized)