
# A set of useful functions for cmake
#    \file ccmake.cmake
#    \author   Igor Sandler
#
#    \version 1.0
#
#


# 	NEED_BOOST - list of boost packages that we need
#    ourPlatform  - destination platform ID
#    ourLibSuffix  -  where to put everything (like install-prefix)

macro (setPlatformName)
	include(CMakeParseArguments)
	if (UNIX)
		if (CMAKE_SIZEOF_VOID_P EQUAL 8) 
			set(ourPlatform linux-x64)
			set (ourLibSuffix lib)
		else()
			set(ourPlatform linux-x32)
			set (ourLibSuffix lib-x32)
		endif()
		
	elseif(WIN32)
		message(STATUS "MSVC_VERSION = " ${MSVC_VERSION})
		if (MSVC_VERSION STREQUAL 1600) # VS 2010 ?
			if (CMAKE_CL_64) # Using the 64 bit compiler from Microsoft
				set(ourPlatform vs2010-x64)
				set (ourLibSuffix lib-vs2010-x64)
			else() 
				set(ourPlatform vs2010-x32)
				set (ourLibSuffix lib-vs2010-x32)
			endif()
		elseif(MSVC_VERSION STREQUAL 1800) # VS2013 ?
			if (CMAKE_CL_64) # Using the 64 bit compiler from Microsoft
				set(ourPlatform vs2013-x64)
				set (ourLibSuffix lib)
			else()
				set(ourPlatform vs2013-x32)
				set (ourLibSuffix lib-vs2013-x32)
			endif()
		else()
			message(FATAL_ERROR " this Visual Studio version look like not supported yet")
		endif()
	else()
		message ( FATAL_ERROR " platform not supported " )
	endif()
	
	# look for buildsys folder:
	find_path(OUR_BUILDFOLDER buildsys PATHS ../ ../../ ../../../ ../../../../  NO_DEFAULT_PATH)
	if (NOT OUR_BUILDFOLDER)
		message(STATUS " warning: can not find buildsys directory")
	else()
		message(STATUS " found buildsys in ${OUR_BUILDFOLDER}")	
	endif()
	message(STATUS  " detected platform: " ${ourPlatform}; " output dir:  " ${ourLibSuffix})
endmacro()

# add boost libs and headers
macro (addBoost)	
	set(Boost_USE_STATIC_LIBS OFF) 
	set(Boost_USE_MULTITHREADED ON)  
	set (BOOST_ROOT $ENV{BOOST_ROOT})
	set (BOOST_LIBRARYDIR $ENV{BOOST_LIBRARYDIR})
	add_definitions(-DBOOST_ALL_DYN_LINK)
	add_definitions(-DUSING_BOOST_LIBRARY)
	message(STATUS  " adding BOOST: root = " ${BOOST_ROOT} ; " lib dir = " ${BOOST_LIBRARYDIR} )
	find_package(Boost ${boost_version} REQUIRED COMPONENTS ${ARGN})
	list(APPEND INC_DIR_LIST ${Boost_INCLUDE_DIRS}) 
	list(APPEND LIB_LIST ${Boost_LIBRARIES})
	#message(STATUS boost libs:  ${Boost_LIBRARIES})
endmacro()

macro (addQT)
	# Find includes in corresponding build directories
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	# Instruct CMake to run moc automatically when needed.
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTORCC ON)
	set (QTDIR $ENV{QTDIR})  #  ? do we need this really?
	list(APPEND CMAKE_PREFIX_PATH ${QTDIR})
	# Find the Qt library
	find_package(Qt5Core)
	find_package(Qt5Gui)
	find_package(Qt5Widgets)
	list(APPEND LIB_LIST "Qt5::Core" "Qt5::Widgets" "Qt5::Gui")
	set(AUTOGEN_TARGETS_FOLDER automoc)
endmacro()

macro (addQWTLinux) 
	FIND_PATH(QWT_INCLUDE_DIR NAMES qwt.h PATHS  
		/usr/include/qt5/qwt
		/usr/include
		/usr/include/qt5
		"ENV{INCLUDE}"
		PATH_SUFFIXES qwt-qt5 qwt
	)
	FIND_LIBRARY(QWT_LIBRARY NAMES qwt-qt5 qwt PATHS
		/usr/lib64
		/user/local/lib64
		/usr/lib
		/usr/local/lib
		"$ENV{LIB}"
	)
	IF (QWT_INCLUDE_DIR AND QWT_LIBRARY)
		SET(QWT_FOUND TRUE)
		list(APPEND LIB_LIST ${QWT_LIBRARY})
		list(APPEND INC_DIR_LIST ${QWT_INCLUDE_DIR})

	else()
		message(FATAL_ERROR "cannot find QWT library")
	ENDIF()
	

	
endmacro()

macro (findOurLibs)
	find_path(OUR_LIBRARY_DIR ${ourLibSuffix} PATHS ../ ../../ ../../../ ../../../../  NO_DEFAULT_PATH)
	if (NOT OUR_LIBRARY_DIR)
		#message(FATAL_ERROR " error: can not find " ${ourLibSuffix} " directory")
		#lets create it!
		file(MAKE_DIRECTORY ${OUR_BUILDFOLDER}/${ourLibSuffix})
		set(OUR_LIBRARY_DIR ${OUR_BUILDFOLDER})
	endif()
	set(OUR_LIBRARY_DIR ${OUR_LIBRARY_DIR}/${ourLibSuffix})
	message(STATUS "our libs are in " ${OUR_LIBRARY_DIR})
endmacro()

# add build information to the source files list (curSrcList)
macro (addVersionInfo curSrcList)
	find_file(VERSION_INFO_FILE version.txt PATHS . ../ ../../ ../../../ ../../../../ NO_DEFAULT_PATH) # where is version.txt file?
	find_path(BUILD_SYSTEM_PATH buildsys PATHS . ../ ../../ ../../../ ../../../../ NO_DEFAULT_PATH) 
	find_file(BUILDINFO_PY_SCRIPT build_info.py PATHS ${BUILD_SYSTEM_PATH}/buildsys  NO_DEFAULT_PATH)
	if (NOT VERSION_INFO_FILE) # at first, try to create one:
		file(WRITE "../version.txt" "1.0")
		find_file(VERSION_INFO_FILE version.txt PATHS . ../ ../../ ../../../   NO_DEFAULT_PATH)
		if (NOT VERSION_INFO_FILE) 
			message(FATAL_ERROR "can not find version info file (version.txt); CMAKE_CURRENT_SOURCE_DIR=" ${CMAKE_CURRENT_SOURCE_DIR})
		endif()
	else()
		#logMessage("got version file:  ${VERSION_INFO_FILE}")
		message(STATUS  "got version file:  ${VERSION_INFO_FILE}")
	endif()
	if (NOT BUILDINFO_PY_SCRIPT) 
		message(FATAL_ERROR "can not find BUILDINFO_PY_SCRIPT (BUILD_SYSTEM_PATH = ${BUILD_SYSTEM_PATH}")
	endif()
	
	set(BUILD_INFO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/build_info.cpp) # We are going to create this file after version.txt will change, at compile time
	list(APPEND ${curSrcList} ${BUILD_INFO_FILE}) # append 'build_info.cpp' to the project
	
	set(BN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/build_number.cpp) 
	list(APPEND ${curSrcList} ${BN_FILE}) # append 'build_number.cpp' to the project
	if (NOT EXISTS ${BN_FILE}) 
		file(WRITE ${BN_FILE}  "0")
	endif()
	
	# recreate build info:
	add_custom_command(OUTPUT ${BUILD_INFO_FILE} COMMAND python ${BUILDINFO_PY_SCRIPT} ${VERSION_INFO_FILE} ${BUILD_INFO_FILE} ${ourPlatform} DEPENDS ${VERSION_INFO_FILE} COMMENT "creating ${BUILD_INFO_FILE}")
	
	file(STRINGS  ${VERSION_INFO_FILE} VERSION_INFO_STRING)
	message(STATUS "version:   ${VERSION_INFO_STRING}")
   
endmacro ()

macro (addBuildNumber)
	find_file(BN_INFO_FILE buildnumber.txt PATHS ./ ../ ../../ ../../../ ../../../../ NO_DEFAULT_PATH) # where is buildnumber.txt file?
	find_path(BUILD_SYSTEM_PATH buildsys PATHS . ../ ../../ ../../../ ../../../../ NO_DEFAULT_PATH) 
	find_file(BN_PY_SCRIPT incbuildnumber.py PATHS ${BUILD_SYSTEM_PATH}/buildsys  NO_DEFAULT_PATH)
	if (NOT BN_PY_SCRIPT) # cannot find one useful python script
		message(STATUS "script: ${BN_PY_SCRIPT}")
		message(FATAL_ERROR "ERROR in addBuildNumber: (BUILD_SYSTEM_PATH = ${BUILD_SYSTEM_PATH}; cannot find incbuildnumber.py")
	endif()	
		
	if (NOT BN_INFO_FILE)
		message(STATUS " WARNING: cannot find build info file... creating the new one")
		file(WRITE "../buildnumber.txt" "1")
		find_file(BN_INFO_FILE buildnumber.txt PATHS ./ ../ ../../   NO_DEFAULT_PATH)
		if (NOT BN_INFO_FILE)
			message(FATAL_ERROR "ERROR in addBuildNumber: cannot create buildnumber.txt")
		endif()
	endif()
	
	set(BN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/build_number.cpp) 
	add_custom_target(INC_BN_TARGET ALL  python ${BN_PY_SCRIPT} ${BN_INFO_FILE} ${BN_FILE} )
	
	message(STATUS "PROJECT_NAME = ${PROJECT_NAME}; INC_BN_TARGET = ${INC_BN_TARGET}")
	add_dependencies(${PROJECT_NAME} INC_BN_TARGET)
endmacro()


# ============= addExtLibrary   macro =========
# params: 
# 1.  library dir
# 2. ".lib" file name (release)
# 3. ".lib" file name (debug) - may be absent
macro (addExtLibrary libNickname)

	# 1. find ext library dir:
	unset(EXT_LIBRARY_DIR CACHE)
	set(EXT_LIBRARY_DIR EXT_LIBRARY_DIR-NOTFOUND)
	find_path(EXT_LIBRARY_DIR extlib PATHS ../ ../../ ../../../ ../../../../ ../../../../../ ../../../../../../   NO_DEFAULT_PATH)
	if (NOT EXT_LIBRARY_DIR)
		message(FATAL_ERROR "addExtLibrary::error: can not find 'extlib' directory")
	else()
		set(EXT_LIBRARY_DIR "${EXT_LIBRARY_DIR}/extlib")
	endif()
	#message(STATUS "EXT_LIBRARY_DIR =  ${EXT_LIBRARY_DIR} ")
	
	cmake_parse_arguments(earg "" "VSTRING;RLNAME;DLNAME" "" ${ARGN})
	set(extLibPath ${EXT_LIBRARY_DIR}/${libNickname})
	if (earg_VSTRING)   # use version string, if needed
		set(extLibPath ${extLibPath}/${earg_VSTRING})
	endif()
	if (NOT EXISTS ${extLibPath})
		message(FATAL_ERROR "addExtLibrary ( ${libNickname})::error: can not find ${extLibPath} directory")
	endif()
	set(extLibPlatformPath ${extLibPath}/${ourPlatform})
	
	if (earg_RLNAME)
		set(lName "${extLibPlatformPath}/lib/${earg_RLNAME}") 
	else()
		set(lName "${extLibPlatformPath}/lib/${libNickname}") 
	endif()
	
	if (earg_DLNAME) # we have a very special name for debug version
		set(dName "${extLibPlatformPath}/lib/${earg_DLNAME}${CMAKE_LINK_LIBRARY_SUFFIX}") 
		# this dName MUST exist, since we specify it here:
		if (NOT EXISTS "${dName}") 
			message(FATAL_ERROR "addExtLibrary: cannot locate ${dName} file") 
		endif()
	else() # use name for 'release'.d; 
		set(dName "${lName}d${CMAKE_LINK_LIBRARY_SUFFIX}") 
	endif()
	set(lName "${lName}${CMAKE_LINK_LIBRARY_SUFFIX}")
	
	if (EXISTS ${dName}) # we have got separate debug and release
		list(APPEND releaseLibList optimized ${lName})
		list(APPEND debugLibList debug ${dName})
		message(STATUS "${libNickname} added (release and debug)")
	else()	# everything is in one place
		list(APPEND LIB_LIST ${lName})
		message(STATUS "${libNickname} added (release only)")
	endif()
	
	list(APPEND INC_DIR_LIST "${extLibPath}/include")

endmacro()

# usage: addSourceFiles(path_to_our_files file1 file2 ...   file_n)
# file names without extension
macro (addSourceFiles groupName sourceFilesPath)
	#GET_FILENAME_COMPONENT(ourGroupName ${sourceFilesPath} NAME)
	#message(STATUS "source group ${ourGroupName} was added")
	set(extList "cpp" "cc" "c" "h" "hpp" "hh")
	set(sFileList)
	list(APPEND INC_DIR_LIST ${sourceFilesPath})
	foreach(sfName ${ARGN})
		set(currentFileList) # clear this list
		foreach(ext ${extList})
			set(aFile aFile-NOTFOUND)
			set(aFileName "${sfName}.${ext}")
			#message(STATUS "    looking for ${aFileName} in a ${sourceFilesPath}")
			find_file(aFile ${aFileName} PATHS ${sourceFilesPath} NO_DEFAULT_PATH)
			if(aFile) 
				list(APPEND currentFileList ${aFile})
				#message(STATUS "     file ${aFile} added")
			endif()			
			set(aFile aFile-NOTFOUND)
		endforeach()
		if (NOT currentFileList) 
			message(STATUS "error: in path ${sourceFilesPath}")
			message(FATAL_ERROR "error: can not add file ${sfName}")
		else()
			list(APPEND SRC_LIST  ${currentFileList})
			list(APPEND sFileList  ${currentFileList})
		endif()
		
	endforeach()
	#message(STATUS "adding source_group ${groupName}")
	#source_group(${groupName} FILES ${sFileList})
	
	#set(group_${groupName} ${groupName})
	if (sFileList)
		set(group_${groupName}_files ${sFileList})
		list(APPEND ourGroupNames ${groupName})
	endif()
endmacro()

macro (addSourceFilesSimple dstVar groupName sourceFilesPath)
	set(extList "cpp" "cc" "c" "h" "hpp" "hh")
	set(sFileList)
	list(APPEND INC_DIR_LIST ${sourceFilesPath})
	foreach(sfName ${ARGN})
		#message(STATUS "    " ${sfName} ":")
		set(currentFileList) # clear this list
		foreach(ext ${extList})
			set(aFile aFile-NOTFOUND)
			set(aFileName "${sfName}.${ext}")
			#message(STATUS "    looking for ${aFileName} in a ${sourceFilesPath}")
			find_file(aFile ${aFileName} PATHS ${sourceFilesPath} NO_DEFAULT_PATH)
			if(aFile) 
				list(APPEND currentFileList ${aFile})
				#message(STATUS "     file ${aFile} added")
			endif()			
			set(aFile aFile-NOTFOUND)
		endforeach()
		if (NOT currentFileList) 
			message("error: in path ${sourceFilesPath}")
			message(FATAL_ERROR "error: can not add file ${sfName}")
		else()
			list(APPEND ${dstVar}  ${currentFileList})
			#message(STATUS "     files ${currentFileList} added")
		endif()
		
	endforeach()
endmacro()


macro (commonStart)
	set(CMAKE_COLOR_MAKEFILE TRUE)
	set(CMAKE_VERBOSE_MAKEFILE OFF)
	setPlatformName()
	findOurLibs()
endmacro()

# add libs from argn to release and debug lists;   libs are from our "lib" folder
macro(fillOurLibList  releaseLList debugLList)
	set(ourRLib ourRLib-NOTFOUND)
	set(ourDLib ourDLib-NOTFOUND)

	foreach(ourLib ${ARGN})
	    find_library(ourRLib "${ourLib}" PATHS ${OUR_LIBRARY_DIR}/release NO_DEFAULT_PATH)
	    if (NOT ourRLib)
	        message(FATAL_ERROR "error: can not find ${ourLib} (release) lib in ${OUR_LIBRARY_DIR}/release")
	    endif()
	    find_library(ourDLib "${ourLib}d" PATHS ${OUR_LIBRARY_DIR}/debug NO_DEFAULT_PATH)
	    if (NOT ourDLib)
	        message(FATAL_ERROR "error: can not find ${ourLib} (debug) lib ")
	    endif()
	    #message(STATUS "....adding " ${ourLib} " libraries " ${ourRLib} " and " ${ourDLib})

	    list(APPEND ${releaseLList}  optimized ${ourRLib})
	    list(APPEND ${debugLList}  debug ${ourDLib})
	    
	    set(ourRLib ourRLib-NOTFOUND)
	    set(ourDLib ourDLib-NOTFOUND)
	endforeach()
endmacro()

macro (commonEnd   libType)
	if(${libType} STREQUAL SHARED) #  DLL special case:
		set(BUILD_SHARED_LIBS ON)
	endif()
	if(${libType} STREQUAL STATIC)
		set(BUILD_SHARED_LIBS OFF)
	endif()
	list(APPEND INC_DIR_LIST ".")
	
	PROJECT(${PROJECT_NAME})
	
	#set(CMAKE_DEBUG_POSTFIX "d") # this works only for libs
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUR_LIBRARY_DIR}/debug")
	set(ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUR_LIBRARY_DIR}/debug")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUR_LIBRARY_DIR}/debug")

	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUR_LIBRARY_DIR}/release")
	set(ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUR_LIBRARY_DIR}/release")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUR_LIBRARY_DIR}/release")

	if (USE_QT)
		if (${USE_QT} STREQUAL YES )
			#http://qt-project.org/wiki/toStdWStringAndBuiltInWchar : 
			# not work with boost and sometimes with std set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-")
			addQT()
		endif()
	endif()
	
	add_definitions(-DNOMINMAX)
	add_definitions(-DOURPROJECTNAME=${PROJECT_NAME}) # this can be used in 'version()' function
	list(APPEND CMAKE_C_FLAGS_DEBUG	-D_DEBUG -DBUILD123=1)
	list(APPEND CMAKE_C_FLAGS_RELEASE	-D_SCL_SECURE_NO_WARNINGS -DNDEBUG -DBUILD123=2  -D_ITERATOR_DEBUG_LEVEL=0 )
	
	addVersionInfo(SRC_LIST)
			
	if(${libType} STREQUAL EXE)
	
		if (WIN32 AND USE_QT)
			if (${USE_QT} STREQUAL YES )
				ADD_EXECUTABLE(${PROJECT_NAME}  WIN32 ${SRC_LIST})
			else()
				message(FATAL_ERROR "error: WIN32 = ${WIN32} and USE_QT=${USE_QT}")
			endif()
		else()
			message(STATUS "creating EXE without QT support")
			ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})
		endif()
		
	else()
		add_library(${PROJECT_NAME}  ${libType} ${SRC_LIST})
	endif()
	
	addBuildNumber()
	if (NEED_BOOST)
		addBoost(${NEED_BOOST})
	endif()
	
	INCLUDE_DIRECTORIES(${INC_DIR_LIST}) 
	
	SET_TARGET_PROPERTIES( ${PROJECT_NAME}
		PROPERTIES    DEBUG_OUTPUT_NAME ${PROJECT_NAME}d    RELEASE_OUTPUT_NAME ${PROJECT_NAME}
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUR_LIBRARY_DIR}/debug"  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUR_LIBRARY_DIR}/release"
		VERSION ${VERSION_INFO_STRING}
	)
	
	if(WIN32 AND MSVC) 
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /fp:fast /O2  /Ob2 /Oi /Ot /Qpar /openmp")
		
		# 4101 unreferenced local variable
		# 4290 C++ exception specification ignored
		# C4018  signed/unsigned mismatch
		#
		
		set(MSWARNINGS 4267  4996 4290 4101 4018)
		foreach(wwarning ${MSWARNINGS})
			#set(thisWD "/wd \\\"${wwarning}\\\"")
			#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd\\\"${wwarning}\\\" ")
			#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${thisWD}")
			#add_compile_options("/wd\\\"${wwarning}\\\" ")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd${wwarning} ")
		endforeach()
		#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  /wd \\\"4267\\\" /wd\\\"4996\\\"     ")
	endif()
	if (UNIX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
		add_definitions(-DLIN_UX -DUNIX)
	endif()
	
	#SET_TARGET_PROPERTIES( ${PROJECT_NAME}
	#	PROPERTIES    COMPILE_DEFINITIONS_DEBUG _DEBUG
	#	COMPILE_DEFINITIONS_RELEASE 
	
	#set(CMAKE_CXX_FLAGS_RELEASE  ${CMAKE_CXX_FLAGS_RELEASE} -D_SCL_SECURE_NO_WARNINGS -DNDEBUG -DBUILD123=2  -D_ITERATOR_DEBUG_LEVEL=0 )
	
	foreach(ourGroup  ${ourGroupNames})
		source_group(${ourGroup} FILES ${group_${ourGroup}_files})
		#message(" group ${ourGroup} created; ")
		#message(${group_${ourGroup}_files})
		#message("")
	endforeach()
	
	if (OUR_LIB_LIST) # if we have some libs to add
		fillOurLibList(releaseLibList  debugLibList  ${OUR_LIB_LIST})
	endif()
	if(NOT(${libType} STREQUAL STATIC))
	
		target_link_libraries(${PROJECT_NAME}  ${LIB_LIST}   ${debugLibList}   ${releaseLibList} )
		
		message(STATUS "debug lib list: " )
		foreach(ourLib123 ${debugLibList})
			message(STATUS "      " ${ourLib123})
		endforeach()
		#message("release lib list: " )
		#foreach(ourLib123 ${releaseLibList})
		#	message("      " ${ourLib123})
		#endforeach()
		#message(" _common_ lib list: " )
		#foreach(ourLib123 ${LIB_LIST})
		#	message("      " ${ourLib123})
		#endforeach()


	endif()
	
	if (UNIX)
		if(${libType} STREQUAL SHARED) #  DLL special case:
			if( CMAKE_SIZEOF_VOID_P MATCHES 8 )
				install(TARGETS ${PROJECT_NAME}   DESTINATION lib64)
			else()
				install(TARGETS ${PROJECT_NAME}   DESTINATION lib)
			endif()
		endif()
	endif()
	
	
endmacro()

macro (programEnd)
	commonEnd(EXE)
endmacro()

macro (libraryEnd libType)
	commonEnd(${libType})
endmacro()



