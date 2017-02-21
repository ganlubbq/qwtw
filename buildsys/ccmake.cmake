
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
		message(STATUS "MSVC_CXX_ARCHITECTURE_ID = ${MSVC_CXX_ARCHITECTURE_ID}")
		message(STATUS "CMAKE_CL_64 = ${CMAKE_CL_64}")
	
		if (MSVC_VERSION STREQUAL 1600) # VS 2010 
			if (CMAKE_CL_64) # Using the 64 bit compiler from Microsoft
				set(ourPlatform vs2010-x64)
				set (ourLibSuffix lib-vs2010-x64)
			else() 
				set(ourPlatform vs2010-x32)
				set (ourLibSuffix lib-vs2010-x32)
			endif()
		elseif(MSVC_VERSION STREQUAL 1800) # VS2013 
			if (CMAKE_CL_64) # Using the 64 bit compiler from Microsoft
				set(ourPlatform vs2013-x64)
				set (ourLibSuffix lib-vs2013-x64)
			else()
				set(ourPlatform vs2013-x32)
				set (ourLibSuffix lib-vs2013-x32)
			endif()
		elseif(MSVC_VERSION STREQUAL 1900) # VS2015 
			if (CMAKE_CL_64) # Using the 64 bit compiler from Microsoft
				set(ourPlatform vs2015-x64)
				set (ourLibSuffix lib-vs2015-x64)
			else()
				set(ourPlatform vs2015-x32)
				set (ourLibSuffix lib-vs2015-x32)
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
		message(STATUS "[${PROJECT_NAME}]:  warning: can not find buildsys directory")
	else()
		message(STATUS "[${PROJECT_NAME}]:  found buildsys in ${OUR_BUILDFOLDER}")	
	endif()
	message(STATUS  "[${PROJECT_NAME}]:  detected platform: " ${ourPlatform}; " output dir:  " ${ourLibSuffix})
endmacro()



# substruct list2 from list1
# on the output, list1 = list1 - list2;    and after that list2 = list2 + list1
#
macro (sub_list list1 list2)
	#message(STATUS "[${PROJECT_NAME}]: sub_list begin. (list1 =  ${${list1}}    list2 = ${${list2}};     argn = ${ARGN}")
	foreach (list2_item ${${list2}})
		list(FIND ${list1} ${list2_item} find_result)
		if (${find_result} EQUAL -1)
			#message(STATUS "[${PROJECT_NAME}]: sub_list: got new item ( ${list2_item} ) in 'list1' ")
		else()
			#message(STATUS "[${PROJECT_NAME}]: sub_list: removing item ( ${list2_item} ) from 'list1' ")
			list(REMOVE_AT  ${list1} find_result)
		endif()
		list(APPEND ${list2} ${${list1}})
		list(REMOVE_DUPLICATES ${list2})
	endforeach()
	#message(STATUS "[${PROJECT_NAME}]: sub_list end. (list1 =  ${${list1}}    list2 = ${${list2}};     argn = ${ARGN}")
endmacro()

# add boost libs and headers
macro (addBoost)	
	set(Boost_USE_STATIC_LIBS OFF) 
	set(Boost_USE_MULTITHREADED ON)  
	set (BOOST_ROOT $ENV{BOOST_ROOT})
	set (BOOST_LIBRARYDIR $ENV{BOOST_LIBRARYDIR})
	add_definitions(-DBOOST_ALL_DYN_LINK)
	add_definitions(-DUSING_BOOST_LIBRARY)
	message(STATUS  "[${PROJECT_NAME}]: adding BOOST: root = " ${BOOST_ROOT} ; " lib dir = " ${BOOST_LIBRARYDIR} )
	
	if (NOT OUR_BOOST_COMPONENTS_LIST)
		set (OUR_BOOST_COMPONENTS_LIST ${ARGN})
		find_package(Boost ${boost_version} REQUIRED COMPONENTS ${OUR_BOOST_COMPONENTS_LIST})
		set (OUR_BOOST_LIBRARIES_LIST ${Boost_LIBRARIES})
		set (OUR_BOOST_INCLUDE_LIST ${Boost_INCLUDE_DIRS})
	else()
		#message(STATUS "[${PROJECT_NAME}]: add-boost-begin: OUR_BOOST_COMPONENTS_LIST = ${OUR_BOOST_COMPONENTS_LIST}")
		set (tmplist1 ${ARGN})
		#message(STATUS "[${PROJECT_NAME}]: addBoost: (list1 =  ${tmplist1};    list2 = ${OUR_BOOST_COMPONENTS_LIST}")
		sub_list(tmplist1 OUR_BOOST_COMPONENTS_LIST)
		if (tmplist1)
			message(STATUS "[${PROJECT_NAME}]: updating BOOST components list; new components: ${tmplist1}")
			set (OUR_BOOST_COMPONENTS_LIST ${OUR_BOOST_COMPONENTS_LIST} PARENT_SCOPE)
			
			find_package(Boost ${boost_version} REQUIRED COMPONENTS ${tmplist1})
			
			list(APPEND OUR_BOOST_INCLUDE_LIST ${Boost_INCLUDE_DIRS}) 
			list(APPEND OUR_BOOST_LIBRARIES_LIST ${Boost_LIBRARIES})  
			
			list(REMOVE_DUPLICATES OUR_BOOST_INCLUDE_LIST)
			list(REMOVE_DUPLICATES OUR_BOOST_LIBRARIES_LIST)
			
			set (OUR_BOOST_INCLUDE_LIST ${OUR_BOOST_INCLUDE_LIST} PARENT_SCOPE)
			set (OUR_BOOST_LIBRARIES_LIST ${OUR_BOOST_LIBRARIES_LIST} PARENT_SCOPE)
		endif()
		
	endif()
	
	list(APPEND INC_DIR_LIST ${OUR_BOOST_INCLUDE_LIST}) 
	list(APPEND ${L_LIST} ${OUR_BOOST_LIBRARIES_LIST})  
	
	#message(STATUS boost libs:  ${Boost_LIBRARIES})
	message(STATUS "[${PROJECT_NAME}]: add-boost-end: OUR_BOOST_COMPONENTS_LIST = ${OUR_BOOST_COMPONENTS_LIST}")
endmacro()

# add QT5 macro
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
	if (NOT QT5_LIBRARIES_MINE)
		message(STATUS "adding QT5 library for ${PROJECT_NAME} (platform ${ourPlatform})")
		find_package(Qt5Core REQUIRED)
		find_package(Qt5Gui REQUIRED)
		find_package(Qt5Widgets REQUIRED)
		set (QT5_LIBRARIES_MINE YES)
	endif()
	list(APPEND ${L_LIST} "Qt5::Core" "Qt5::Widgets" "Qt5::Gui")
	set(AUTOGEN_TARGETS_FOLDER automoc)
	
	set(now_using_QT YES)
	set(now_using_QT5 YES)
endmacro()

# add QT4 macro
macro (addQT4)
	# Find includes in corresponding build directories
	
	# Instruct CMake to run moc automatically when needed.
	set(AUTOGEN_TARGETS_FOLDER automoc)
	
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTORCC ON)
	
	set (QTDIR $ENV{QTDIR})  #  ? do we need this really?
	list(APPEND CMAKE_PREFIX_PATH ${QTDIR})
	
	# Find the Qt library
	if (NOT QT4_LIBRARIES_MINE)
		find_package(Qt4 4.8.7 REQUIRED)
		message(STATUS "got QT libs for [${PROJECT_NAME}]  (platform ${ourPlatform}): ${QT_LIBRARIES}  ${QT_QTMAIN_LIBRARY}; files: ${QT_USE_FILE}; defs: ${QT_DEFINITIONS}")
		include(${QT_USE_FILE})
		set (QT4_LIBRARIES_MINE YES)
	endif()
	
	
	add_definitions(${QT_DEFINITIONS})
	list(APPEND ${L_LIST}  ${QT_LIBRARIES}   ${QT_QTMAIN_LIBRARY})
	#list(APPEND ${L_LIST} "Qt4::Core" "Qt4::Widgets" "Qt4::Gui")
	
	
	
	set(now_using_QT YES)
	set(now_using_QT4 YES)
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
		list(APPEND ${L_LIST} ${QWT_LIBRARY})
		list(APPEND INC_DIR_LIST ${QWT_INCLUDE_DIR})

	else()
		message(FATAL_ERROR "cannot find QWT library")
	ENDIF()

	
endmacro()


macro (findOurLibs)
	if (NOT OUR_LIBRARY_DIR)
		find_file(OUR_LIBRARY_DIR ${ourLibSuffix} PATHS ../ ../../ ../../../ ../../../../  NO_DEFAULT_PATH)
		if (NOT OUR_LIBRARY_DIR)#lets create it!
			set(OUR_LIBRARY_DIR ${OUR_BUILDFOLDER}/${ourLibSuffix})
			IF (NOT EXISTS ${OUR_LIBRARY_DIR})
				file(MAKE_DIRECTORY ${OUR_LIBRARY_DIR})
			endif()
			IF (NOT EXISTS ${OUR_LIBRARY_DIR})
				message(FATAL_ERROR "project ${PROJECT_NAME}: cannot create   ${OUR_LIBRARY_DIR} folder")
			endif()
		endif()
	endif()	
	message(STATUS "project ${PROJECT_NAME}: our libs are in " ${OUR_LIBRARY_DIR})
endmacro()

# add build information to the source files list (curSrcList)
macro (addVersionInfo curSrcList)
	find_file(VERSION_INFO_FILE version.txt PATHS . ../ ../../ ../../../ ../../../../  NO_DEFAULT_PATH) # where is version.txt file?
	find_path(BUILD_SYSTEM_PATH buildsys PATHS . ../ ../../ ../../../ ../../../../  NO_DEFAULT_PATH) 
	find_file(BUILDINFO_PY_SCRIPT build_info.py PATHS ${BUILD_SYSTEM_PATH}/buildsys  NO_DEFAULT_PATH)
	if (NOT VERSION_INFO_FILE) # at first, try to create one:
		file(WRITE "../version.txt" "1.0")
		find_file(VERSION_INFO_FILE version.txt PATHS . ../ ../../ ../../../  NO_DEFAULT_PATH)
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
	
	set(BUILD_INFO_FILE "${CMAKE_BINARY_DIR}/build_info.cpp") # We are going to create this file after version.txt will change, at compile time
	list(APPEND ${curSrcList} ${BUILD_INFO_FILE}) # append 'build_info.cpp' to the project
	
	set(BN_FILE "${CMAKE_BINARY_DIR}/build_number.cpp") 
	list(APPEND ${curSrcList} ${BN_FILE}) # append 'build_number.cpp' to the project
	if (NOT EXISTS ${BN_FILE})  # we need to create something, or project will not be created
		file(WRITE ${BN_FILE}  " ")
	endif()
	
	# recreate build info:
	add_custom_command(OUTPUT ${BUILD_INFO_FILE} COMMAND python ${BUILDINFO_PY_SCRIPT} ${VERSION_INFO_FILE} ${BUILD_INFO_FILE} ${ourPlatform} DEPENDS ${VERSION_INFO_FILE} COMMENT "creating ${BUILD_INFO_FILE}")

	file(STRINGS  ${VERSION_INFO_FILE} VERSION_INFO_STRING)
	message(STATUS "version:   ${VERSION_INFO_STRING}")
endmacro ()

macro (addBuildNumber)
	find_file(BN_INFO_FILE buildnumber.txt PATHS ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/../  NO_DEFAULT_PATH) # where is buildnumber.txt file?
	find_path(BUILD_SYSTEM_PATH buildsys PATHS . ../ ../../ ../../../ ../../../../  NO_DEFAULT_PATH) 
	find_file(BN_PY_SCRIPT incbuildnumber.py PATHS ${BUILD_SYSTEM_PATH}/buildsys  NO_DEFAULT_PATH)
	if (NOT BN_PY_SCRIPT) # cannot find one useful python script
		message(STATUS "script: ${BN_PY_SCRIPT}")
		message(FATAL_ERROR "ERROR in addBuildNumber: (BUILD_SYSTEM_PATH = ${BUILD_SYSTEM_PATH}; cannot find incbuildnumber.py")
	endif()	
		
	if (NOT BN_INFO_FILE)
		message(STATUS " WARNING: cannot find build info file... creating the new one")
		file(WRITE "${CMAKE_BINARY_DIR}/buildnumber.txt" "1")
		find_file(BN_INFO_FILE buildnumber.txt PATHS ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/../  NO_DEFAULT_PATH) 
		if (NOT BN_INFO_FILE)
			message(FATAL_ERROR "ERROR in addBuildNumber: cannot create buildnumber.txt")
		endif()
	endif()
	
	set(BN_FILE ${CMAKE_BINARY_DIR}/build_number.cpp) 
	add_custom_target(INC_BN_${PROJECT_NAME} ALL  python ${BN_PY_SCRIPT} ${BN_INFO_FILE} ${BN_FILE} WORKING_DIRECTORY  ${CMAKE_BINARY_DIR}  COMMENT "creating new build number" VERBATIM)
	#
	#message(STATUS "PROJECT_NAME = ${PROJECT_NAME}; INC_BN_TARGET = ${INC_BN_TARGET}")
	add_dependencies(${PROJECT_NAME} INC_BN_${PROJECT_NAME})
	
	#add_custom_command(TARGET ${PROJECT_NAME}PRE_BUILD COMMAND  python ARGS ${BN_PY_SCRIPT} ${BN_INFO_FILE} ${BN_FILE} WORKING_DIRECTORY  ${CMAKE_BINARY_DIR} COMMENT "creating new build number" VERBATIM)
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
	
	#if RLNAME was provided, 'lName' and 'dName' vars should be OK
	if (earg_RLNAME) # choose particular library with this name:
		set(lName "${extLibPlatformPath}/lib/${earg_RLNAME}") 
		if (earg_DLNAME) # we have a very special name for debug version
			set(dName "${extLibPlatformPath}/lib/${earg_DLNAME}${CMAKE_LINK_LIBRARY_SUFFIX}") 
			# this dName MUST exist, since we specify it here:
			if (NOT EXISTS "${dName}") 
				message(FATAL_ERROR "addExtLibrary: cannot locate ${dName} file") 
			endif()
		else() # use name for 'release'.d; 
			set(dName "${lName}d${CMAKE_LINK_LIBRARY_SUFFIX}") 
		endif()
		set(lName "${lName}${CMAKE_LINK_LIBRARY_SUFFIX}") # finalize release library name
		if (NOT EXISTS ${lName}) 
			message(FATAL_ERROR "addExtLibrary: cannot locate ${lName} file") 
		endif()
					
		if (EXISTS ${dName}) # we have got separate debug and release
			list(APPEND ${LR_LIST} optimized ${lName})
			list(APPEND ${LD_LIST} debug ${dName})
			message(STATUS "${libNickname} added (release and debug)")
		else()	# everything is in one place
			list(APPEND ${L_LIST} ${lName})
			message(STATUS "${libNickname} added (release only)")
		endif()
	
	else() # lets grab everything in this case:
		#	set(lName "${extLibPlatformPath}/lib/${libNickname}") 
		#
		#	lets grab all the files:
		#
		if (EXISTS "${extLibPlatformPath}/lib/release")
			set (thisLibFilesRelease)
			file(GLOB thisLibFilesRelease "${extLibPlatformPath}/lib/release/*")
			foreach (item ${thisLibFilesRelease})
				list(APPEND ${LR_LIST} optimized ${item})
			endforeach()
		endif()
		if (EXISTS "${extLibPlatformPath}/lib/debug")
			set (thisLibFilesDebug)
			file(GLOB thisLibFilesDebug "${extLibPlatformPath}/lib/debug/*")
			foreach (item ${thisLibFilesDebug})
				list(APPEND ${LD_LIST} debug ${item})
			endforeach()
		endif()
		
		set(thisLibFiles)
		file(GLOB thisLibFiles "${extLibPlatformPath}/lib/*.lib")
		if (thisLibFiles)
			list(APPEND ${L_LIST} ${thisLibFiles})
		endif()
		
		
		#message(STATUS "following libs were added: ")
		#message(STATUS ${thisLibFiles})
	endif()

	list(APPEND INC_DIR_LIST "${extLibPath}/include")
	
	#lets collect all the subfolders from 'include' here:
	FILE(GLOB incSubDirs LIST_DIRECTORIES true  "${extLibPath}/include/*")
	SET(incSubDirList "")
	FOREACH(incSubDir ${incSubDirs})
		IF(IS_DIRECTORY ${incSubDir})
			LIST(APPEND incSubDirList ${incSubDir})
		ENDIF()
	ENDFOREACH()
	list(APPEND INC_DIR_LIST ${incSubDirList})

endmacro()

# just add some library
macro (addOtherLib)
	list(APPEND ${L_LIST} ${ARGN})
endmacro()

# usage: addSourceFiles(path_to_our_files file1 file2 ...   file_n)
# file names without extension
macro (addSourceFiles groupName sourceFilesPath)
	#GET_FILENAME_COMPONENT(ourGroupName ${sourceFilesPath} NAME)
	#message(STATUS "source group ${ourGroupName} was added")
	set(extList ".cpp" ".cc" ".c" ".h" ".hpp" ".hh")
	set(sFileList)
	list(APPEND INC_DIR_LIST ${sourceFilesPath})
	foreach(sfName ${ARGN})
		set(currentFileList) # clear this list
		foreach(ext ${extList})
			set(aFile aFile-NOTFOUND)
			set(aFileName "${sfName}${ext}")
			#message(STATUS "    looking for ${aFileName} in a ${sourceFilesPath}")
			find_file(aFile ${aFileName} PATHS ${sourceFilesPath} NO_DEFAULT_PATH)
			if(aFile) 
				list(APPEND currentFileList ${aFile})
				#message(STATUS "     file ${aFile} added")
			endif()			
			set(aFile aFile-NOTFOUND)
		endforeach()
		if (NOT currentFileList) # test without extencion
			find_file(aFile ${sfName} PATHS ${sourceFilesPath} NO_DEFAULT_PATH)
			if(aFile) 
				list(APPEND currentFileList ${aFile})
			endif()			
			set(aFile aFile-NOTFOUND)
		endif()
		
		if (NOT currentFileList) 
			message(STATUS "tested following files: ")
			foreach(ext ${extList})
				message(STATUS "	${sfName}${ext}")
			
			endforeach()
			message(FATAL_ERROR "error: in path ${sourceFilesPath} can not add file ${sfName}")
		else()
			list(APPEND ${PROJ_SRC_FILES}  ${currentFileList})
			list(APPEND sFileList  ${currentFileList})
			
			#message(STATUS "file ${sfName}: added as ${currentFileList}")
			#message(STATUS "${PROJ_SRC_FILES} =  ${${PROJ_SRC_FILES}}")
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
	
	set(PROJ_SRC_FILES  SL_${PROJECT_NAME})
	set(L_LIST LIBS_${PROJECT_NAME})
	set(LR_LIST LIBS_RELEASE_${PROJECT_NAME})
	set(LD_LIST LIBS_DEBUG_${PROJECT_NAME})
endmacro()

macro (addOurLib)
	set(ourRLib ourRLib-NOTFOUND)
	set(ourDLib ourDLib-NOTFOUND)

	foreach(ourLib ${ARGN})
	    find_library(ourRLib "${ourLib}" PATHS ${OUR_LIBRARY_DIR}/release NO_DEFAULT_PATH)
	    if (NOT ourRLib)
	        message(FATAL_ERROR "error: project ${PROJECT_NAME}: can not find ${ourLib} (release) lib in ${OUR_LIBRARY_DIR}/release")
	    endif()
	    find_library(ourDLib "${ourLib}d" PATHS ${OUR_LIBRARY_DIR}/debug NO_DEFAULT_PATH)
	    if (NOT ourDLib)
	        message(FATAL_ERROR "error: can not find ${ourLib} (debug) lib ")
	    endif()
	    #message(STATUS "....adding " ${ourLib} " libraries " ${ourRLib} " and " ${ourDLib})

	    list(APPEND ${LR_LIST}  optimized ${ourRLib})
	    list(APPEND ${LD_LIST}  debug ${ourDLib})
	    
	    set(ourRLib ourRLib-NOTFOUND)
	    set(ourDLib ourDLib-NOTFOUND)
	endforeach()
endmacro()

# add libs from argn to release and debug lists;   libs are from our "lib" folder
macro(fillOurLibList  releaseLList debugLList)
	set(ourRLib ourRLib-NOTFOUND)
	set(ourDLib ourDLib-NOTFOUND)

	foreach(ourLib ${ARGN})
	    find_library(ourRLib "${ourLib}" PATHS ${OUR_LIBRARY_DIR}/release NO_DEFAULT_PATH)
	    if (NOT ourRLib)
	        message(FATAL_ERROR "error: project ${PROJECT_NAME}: can not find ${ourLib} (release) lib in ${OUR_LIBRARY_DIR}/release")
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
		if (${USE_QT})
			#http://qt-project.org/wiki/toStdWStringAndBuiltInWchar : 
			# not work with boost and sometimes with std set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-")
			addQT()
		endif()
	endif()
	
	add_definitions(-DNOMINMAX)
	add_definitions(-DOURPROJECTNAME=${PROJECT_NAME}) # this can be used in 'version()' function
	IF(WIN32)
		add_definitions(-DWIN32)
	endif()
	
	list(APPEND CMAKE_C_FLAGS_DEBUG	-D_DEBUG -DBUILD123=1 )
	list(APPEND CMAKE_C_FLAGS_RELEASE	-D_SCL_SECURE_NO_WARNINGS -DNDEBUG -DBUILD123=2 )
	
	addVersionInfo(${PROJ_SRC_FILES})
			
	if(${libType} STREQUAL EXE)
	
		if (WIN32 AND now_using_QT)
			if (${now_using_QT} STREQUAL YES )
				
				ADD_EXECUTABLE(${PROJECT_NAME}  WIN32 ${${PROJ_SRC_FILES}})
			else()
				message(FATAL_ERROR "error: WIN32 = ${WIN32} and USE_QT=${USE_QT}")
			endif()
		else()
			message(STATUS "creating EXE without QT support")
			ADD_EXECUTABLE(${PROJECT_NAME} ${${PROJ_SRC_FILES}})
		endif()
		
	else()
		add_library(${PROJECT_NAME}  ${libType} ${${PROJ_SRC_FILES}})
		
		#message(STATUS "files: ${PROJ_SRC_FILES} = ${${PROJ_SRC_FILES}}")
	endif()
	
	addBuildNumber()
	if (NEED_BOOST)
		addBoost(${NEED_BOOST})
	endif()
	
	INCLUDE_DIRECTORIES(${INC_DIR_LIST}) 
	SET_TARGET_PROPERTIES( ${PROJECT_NAME}
		PROPERTIES    DEBUG_OUTPUT_NAME ${PROJECT_NAME}d    RELEASE_OUTPUT_NAME ${PROJECT_NAME}
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUR_LIBRARY_DIR}/debug"  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUR_LIBRARY_DIR}/release"
	)
	
	if (${now_using_QT5})
		set_property(TARGET ${PROJECT_NAME} PROPERTY QT5_NO_LINK_QTMAIN ON)
	endif()
	
	if (${now_using_QT4})
		IF(WIN32)
			if(MSVC) 
				set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
			endif()
		endif()	
		#QT4_AUTOMOC(${PROJ_SRC_FILES})
		set_property(TARGET ${PROJECT_NAME} PROPERTY QT4_NO_LINK_QTMAIN ON)
	endif()

	# add version info to the project properties:
	if(${libType} STREQUAL SHARED) #  DLL special case:
		SET_TARGET_PROPERTIES( ${PROJECT_NAME} 	PROPERTIES    SOVERSION ${VERSION_INFO_STRING})
	else()
		SET_TARGET_PROPERTIES( ${PROJECT_NAME}  PROPERTIES     VERSION ${VERSION_INFO_STRING})
	endif()


	if(WIN32 AND MSVC) 
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHsc")
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
	
	#set(CMAKE_CXX_FLAGS_RELEASE  ${CMAKE_CXX_FLAGS_RELEASE} -D_SCL_SECURE_NO_WARNINGS -DNDEBUG -DBUILD123=2 )
	
	foreach(ourGroup  ${ourGroupNames})
		source_group(${ourGroup} FILES ${group_${ourGroup}_files})
		#message(" group ${ourGroup} created; ")
		#message(${group_${ourGroup}_files})
		#message("")
	endforeach()
	
	if (OUR_LIB_LIST) # if we have some libs to add
		fillOurLibList(${LR_LIST}  ${LD_LIST}  ${OUR_LIB_LIST})
	endif()
	if(NOT(${libType} STREQUAL STATIC))
	
		target_link_libraries(${PROJECT_NAME}  ${${L_LIST}}   ${${LD_LIST}}   ${${LR_LIST}} )
		
		#message(STATUS "debug lib list: " )
		#foreach(ourLib123 ${${LD_LIST}})
		#	message(STATUS "      " ${ourLib123})
		#endforeach()
		
		#message("release lib list: " )
		#foreach(ourLib123 ${${LR_LIST}})
		#	message("      " ${ourLib123})
		#endforeach()
		#message(" _common_ lib list: " )
		#foreach(ourLib123 ${${L_LIST}})
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



