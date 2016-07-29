#
# 	test for qwtwc library
#

cmake_minimum_required (VERSION 2.8.6)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../buildsys/")
include(ccmake)

set(PROJECT_NAME qwtwctest)

#set(USE_MARBLE YES) # use marble-qt for 'top-view' data

commonStart()
if (USE_MARBLE)
	add_definitions(-DUSEMARBLE)
endif()
add_definitions(-DUNICODE -D_UNICODE)


list(APPEND INC_DIR_LIST "../" "../../"  "../../common" "../c_lib")
list(APPEND SRC_LIST "qwtwtest.cpp")
addSourceFiles("common" "../../common"
	xstdef  xmutils )

list(APPEND OUR_LIB_LIST qwtwc)

#message("our platform: ${our_platform}")

#addExtLibrary(bspline  BSpline BSplined)
if(WIN32)
	
else()
 #   addExtLibrary(qwt qwt  qwtd)
    list(APPEND LIB_LIST ncurses qwt)
	link_directories(/home/igor/space/qt541/5.4/gcc_64/bin)

endif()


programEnd()

