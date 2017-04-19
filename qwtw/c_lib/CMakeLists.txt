#
# 	# qwtwc library project
#

cmake_minimum_required (VERSION 2.8.11)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../buildsys/")
set(USE_QT YES)
set(USE_MARBLE YES) # use marble-qt for 'top-view' data
set(USE_QT3D YES) 
include(ccmake)

set(PROJECT_NAME qwtwc)
set (NEED_BOOST  thread system filesystem date_time chrono)
commonStart()


set(INC_DIR_LIST "../"   "../../common" "../../xmatrix2" )

list(APPEND HEADERS_LIST  "../figure2.h")
	
addSourceFiles("source" "." 	qtint qwtw_c )	
addSourceFiles("qwtw" "../" sfigure figure2 justaplot plib1.qrc)	
addSourceFiles("matrix" "../../xmatrix2" xmatrix2 	xmatrixplatform xmatrix_imp xmcoords )

addSourceFiles("common" "../../common"
	xstdef xqbytebuffer xmutils queue_block qtswidget xmroundbuf
	sqwlinetree lrbuf coordtypes
)

addSourceFiles("klifft" "../../common/klifft"	complex fft psd )

#list(APPEND SRC_LIST  ../plib1.qrc)

add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DQT_DLL -DQWT_DLL -DqqwtwEXPORTS -DqwtwcEXPORTS)

if (USE_MARBLE)
	add_definitions(-DUSEMARBLE)
	addSourceFiles("marble" "../wmarble" marbleview)
	
	add_definitions(-DQT_USE_QSTRINGBUILDER -DQT_NO_URL_CAST_FROM_STRING -DQT_NO_CAST_TO_ASCII -DQT_NO_CAST_FROM_BYTEARRAY -DQT_STRICT_ITERATORS)
	#add_definitions(-DQT_USE_QSTRINGBUILDER -DQT_NO_URL_CAST_FROM_STRING -DQT_NO_CAST_TO_ASCII -DQT_NO_CAST_FROM_BYTEARRAY -DQT_STRICT_ITERATORS)
	
	if (WIN32)
		addExtLibrary(marble VSTRING 16.12 RLNAME marblewidget-qt5 DLNAME marblewidget-qt5d  )
	else()
		FIND_PACKAGE (Marble REQUIRED)	
		list(APPEND INC_DIR_LIST ${MARBLE_INCLUDE_DIR}) 	
	 	list(APPEND LIB_LIST ${MARBLE_LIBRARIES})	
	endif()
endif()


if (USE_QT3D)
	add_definitions(-DUSE_QT3D)
	addSourceFiles("qt-3d" "../qt-3d"   qt-3d)
	

endif()

if(WIN32)
	addExtLibrary(qwt VSTRING trunk RLNAME qwt)
	#list(APPEND LIB_LIST "ws2_32.lib")
else()
	add_definitions(-DLIN_UX)
	addQWTLinux()
	message("LIB_LIST: "  ${LIB_LIST})

endif()
libraryEnd(SHARED)

if (UNIX)
	install(FILES qwtw_c.h   DESTINATION  include)
endif()

#add_subdirectory(test)

# ========================================================
#addSourceFilesSimple(testSrc "source" "."  qwtwtest)
#ADD_EXECUTABLE(qwtwtest ${testSrc})
#target_link_libraries(qwtwtest  qwtwc )
#install(TARGETS qwtwtest   DESTINATION bin)

