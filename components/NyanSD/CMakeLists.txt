idf_component_register(SRC_DIRS "."
						INCLUDE_DIRS "."
						REQUIRES NPocoCore NPocoNet esp_netif)
						
target_compile_options(${COMPONENT_LIB} PRIVATE -frtti)
