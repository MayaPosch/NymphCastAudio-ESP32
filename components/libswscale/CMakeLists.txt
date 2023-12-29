# idf_component_register(SRCS "foo.c" "bar.c"
                       # INCLUDE_DIRS "include"
                       # REQUIRES mbedtls)
idf_component_register(SRCS "libswscale/alphablend.c"
						 "libswscale/gamma.c"
						 "libswscale/hscale.c"
						 "libswscale/hscale_fast_bilinear.c"
						 "libswscale/input.c"
						 "libswscale/log2_tab.c"
						 "libswscale/options.c"
						 "libswscale/output.c"
						 "libswscale/rgb2rgb.c"
						 "libswscale/slice.c"
						 "libswscale/swscale.c"
						 "libswscale/swscale_unscaled.c"
						 "libswscale/utils.c"
						 "libswscale/version.c"
						 "libswscale/vscale.c"
						 "libswscale/yuv2rgb.c"
						INCLUDE_DIRS "." "libswscale"
						REQUIRES libavutil)
						
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format-truncation)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-pointer-sign)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-parentheses)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-switch)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-maybe-uninitialized)