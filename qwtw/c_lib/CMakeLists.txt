#
# 	# qwtwc library project
#

cmake_minimum_required (VERSION 2.8.11)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../buildsys/")
set(USE_QT YES)
set(USE_MARBLE YES) # use marble-qt for 'top-view' data
include(ccmake)

set(PROJECT_NAME qwtwc)
set (NEED_BOOST  thread system filesystem date_time chrono)
commonStart()


set(INC_DIR_LIST "../"   "../../common" "../../xmatrix2" )

list(APPEND HEADERS_LIST  "../figure2.h")
	
addSourceFiles("source" "." 	qtint qwtw_c )	
addSourceFiles("qwtw" "../" sfigure figure2 justaplot )	
addSourceFiles("matrix" "../../xmatrix2" xmatrix2 	xmatrixplatform xmatrix_imp xmcoords )

addSourceFiles("common" "../../common"
	xstdef xqbytebuffer xmutils queue_block qtswidget xmroundbuf
	sqwlinetree lrbuf coordtypes
)

addSourceFiles("klifft" "../../common/klifft"	complex fft psd )

list(APPEND SRC_LIST  ../plib1.qrc)

add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DQT_DLL -DQWT_DLL -DqqwtwEXPORTS -DqwtwcEXPORTS)

if (USE_MARBLE)
	add_definitions(-DUSEMARBLE)
	addSourceFiles("marble" "../wmarble" marbleview)
	if (WIN32)
		addExtLibrary(marble VSTRING 1.11.3 RLNAME marblewidget-qt5 DLNAME marblewidget-qt5d  )
	else()
		FIND_PACKAGE (Marble REQUIRED)	
		list(APPEND INC_DIR_LIST ${MARBLE_INCLUDE_DIR}) 	
	 	list(APPEND LIB_LIST ${MARBLE_LIBRARIES})	
	endif()
endif()

if(WIN32)
	addExtLibrary(qwt VSTRING 6.1.2)
	#list(APPEND LIB_LIST "ws2_32.lib")
else()
	add_definitions(-DLIN_UX)
	addQWTLinux()
	message("LIB_LIST: "  ${LIB_LIST})

endif()
libraryEnd(SHARED)

if (UNIX)
	install(FILES qwtw_c.h   DESTINATION include)
endif()

# ========================================================
addSourceFilesSimple(testSrc "source" "."  qwtwtest)
ADD_EXECUTABLE(qwtwtest ${testSrc})
target_link_libraries(qwtwtest  qwtwc )


