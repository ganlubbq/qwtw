#
# 	# qwtwc library project
#

cmake_minimum_required (VERSION 2.8.11)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../buildsys/")
include(ccmake)

add_subdirectory(c_lib)
add_subdirectory(c_lib_test)

