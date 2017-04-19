#
# 	# qwtwc library test project
#

cmake_minimum_required (VERSION 2.8.11)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../buildsys/")
set(USE_QT YES)
include(ccmake)

set(PROJECT_NAME qwtwctest)
set (NEED_BOOST  thread system filesystem date_time chrono)
commonStart()


set(INC_DIR_LIST "../" "../c_lib"  "../../../common" "../../../xmatrix2" )
	
addSourceFiles("source" "../" qwtwtest)	

add_definitions(-DUNICODE -D_UNICODE -DUSE_QT3D )
	add_definitions(-DUSEMARBLE)



#addOurLib(qwtwc)
addOurLib2(qwtwc)

programEnd()
install(TARGETS qwtwctest   DESTINATION bin)

# ========================================================


