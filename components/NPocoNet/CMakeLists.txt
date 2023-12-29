# idf_component_register(SRCS "foo.c" "bar.c"
                       # INCLUDE_DIRS "include"
                       # REQUIRES mbedtls)
idf_component_register(SRC_DIRS "src"
						INCLUDE_DIRS "include" "include/npoco/net"
						REQUIRES NPocoCore)