# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(NymphCastAudio)
						
idf_build_set_property(CXX_COMPILE_OPTIONS "-std=gnu++14" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-frtti" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-D__FREERTOS__" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-DNPOCO" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-DPOCO_THREAD_STACK_SIZE=7168" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-DCONFIG_AVFILTER=1" APPEND)
#idf_build_set_property(COMPILE_OPTIONS "-ffunction-sections" APPEND)
#idf_build_set_property(COMPILE_OPTIONS "-fdata-sections" APPEND)
#idf_build_set_property(COMPILE_OPTIONS "-Wl,--gc-sections" APPEND)
idf_build_set_property(LINK_OPTIONS "-frtti" APPEND)
