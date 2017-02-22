cmake_minimum_required(VERSION 2.8)
project (FOLLY-LITE)

# initialize variables
set(USE_JEMALLOC FALSE)
set (PRIVATE_INCLUDE_DIRS "")

# set complie flags
set(CMAKE_VERBOSE_MAKEFILE on)
set(PRIVATE_FOLLY_CXX_FLAGS
	"-pthread"
	"-std=c++11"
	)

# set output path
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(BUILD_OUT_PATH ${CMAKE_CURRENT_BINARY_DIR})


# decide use boost or not
# in win32 platform use local boost, in unix and linux use host boost as default.
if (WIN32)
	message("================= WINDOWS 32 =======================")
	set(PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/boost/)
	set(PRIVATE_FOLLY_CXX_FLAGS ${PRIVATE_FOLLY_CXX_FLAGS} -DWIN32)
elseif (APPLE)
	message("================= UNIX ==============================")
	set(PRIVATE_FOLLY_CXX_FLAGS ${PRIVATE_FOLLY_CXX_FLAGS} -DUNIX)
else()
	message("================== LINUX ============================")
	set(PRIVATE_FOLLY_CXX_FLAGS ${PRIVATE_FOLLY_CXX_FLAGS} -DLINUX)
endif()


# set incoude directories
set (PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/folly
	${PROJECT_SOURCE_DIR}/jemalloc/include
	)

# set compile flags
add_compile_options(${PRIVATE_FOLLY_CXX_FLAGS})


set (USE_JEMALLOC true)
# decide use jemalloc or not
if (USE_JEMALLOC)
	message("================ USE jemalloc =======================")
	set (PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS}
		${PROJECT_SOURCE_DIR}/jemalloc/jemalloc-4.2.0/include
	)

	# compile jemalloc
	message("compile jemalloc begain....")
	unset (PRIVATE_LIB_JEMALLOC CACHE)
	find_library (
		PRIVATE_LIB_JEMALLOC
		NAMES "jemalloc"
	)
	if ("PRIVATE_LIB_JEMALLOC-NOTFOUND" STREQUAL "${PRIVATE_LIB_JEMALLOC}" AND UNIX)
	if (APPLE AND (NOT (CMAKE_OSX_ARCHITECTURES STREQUAL "")))
	    set (PRIVATE_JEMALLOC_FLAGS "-arch ${CMAKE_OSX_ARCHITECTURES}")
	else ()
	    set (PRIVATE_JEMALLOC_FLAGS "")
	endif ()
	if (NOT MSVC)
	    # suppose all compilers except MSVC support the "-std" argument.
	    set (PRIVATE_JEMALLOC_FLAGS "${PRIVATE_JEMALLOC_FLAGS} -std=gnu99")
	endif ()

	# to compilde jemalloc, you should first run ./autogen.sh first in jemalloc source file folder
	add_custom_command(
			OUTPUT  ${PROJECT_SOURCE_DIR}/jemalloc/jemalloc-4.2.0/configure
			COMMAND sh ${PROJECT_SOURCE_DIR}/jemalloc/jemalloc-4.2.0/autogen.sh
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/jemalloc/jemalloc-4.2.0
			COMMENT "run autogen to generate configure"
			)
	add_custom_target(T1 ALL DEPENDS ${PROJECT_SOURCE_DIR}/jemalloc/jemalloc-4.2.0/configure)

	include(ExternalProject)
	ExternalProject_Add (libjemalloc-target
	    SOURCE_DIR "${PROJECT_SOURCE_DIR}/jemalloc/jemalloc-4.2.0/src"
	    PREFIX "${PROJECT_SOURCE_DIR}/jemalloc"
	    BINARY_DIR "${BUILD_OUT_PATH}/CMakeFiles/CMakeTmp/jemalloc/build"
	    STAMP_DIR "${BUILD_OUT_PATH}/CMakeFiles/CMakeTmp/jemalloc/timestamp"
	    TMP_DIR "${BUILD_OUT_PATH}/CMakeFiles/CMakeTmp/jemalloc/tmp"
	    CONFIGURE_COMMAND "${PROJECT_SOURCE_DIR}/jemalloc/jemalloc-4.2.0/configure"
	        "--with-jemalloc-prefix=je_"
	        "--disable-stats"
	        "--enable-fill"
	        "--prefix=${BUILD_OUT_PATH}"
	        "je_cv_static_page_shift=12"
	        "CC=${CMAKE_C_COMPILER} ${CMAKE_C_COMPILER_ARG1}"
	        "CPPFLAGS=${CMAKE_C_FLAGS} ${PRIVATE_JEMALLOC_FLAGS}"
	        "CFLAGS=${CMAKE_C_FLAGS} ${PRIVATE_JEMALLOC_FLAGS}"
	        "LDFLAGS=${CMAKE_C_FLAGS} ${PRIVATE_JEMALLOC_FLAGS}"
	    BUILD_COMMAND "make" "build_lib"
	    INSTALL_DIR "${LIBRARY_OUTPUT_PATH}"
	    INSTALL_COMMAND  "make"  install_include install_lib  install_bin
	    )
	# add dependencies for jemalloc
	add_dependencies(libjemalloc-target ${PROJECT_SOURCE_DIR}/jemalloc/jemalloc-4.2.0/configure)
	add_library(libjemalloc SHARED IMPORTED)
	add_dependencies (libjemalloc libjemalloc-target)

	elseif (NOT "PRIVATE_LIB_JEMALLOC-NOTFOUND" STREQUAL "${PRIVATE_LIB_JEMALLOC}")
	# add library link
	add_library(libjemalloc SHARED IMPORTED)
	set_target_properties(libjemalloc PROPERTIES
	    IMPORTED_LOCATION "${PRIVATE_LIB_JEMALLOC}"
	    )
	endif ()

  link_libraries(${CMAKE_CURRENT_BINARY_DIR}/lib/libjemalloc.so)
  set (SRC_LIST ${PRIVATE_SRC_LIST})
	include_directories( ${PRIVATE_INCLUDE_DIRS} )
	# build targets
	add_executable(testFBString ${PROJECT_SOURCE_DIR}/test/testFBString.cpp)
	add_executable(testMain ${PROJECT_SOURCE_DIR}/test/main.cpp)
	add_dependencies(testMain libjemalloc)
	add_dependencies(testFBString libjemalloc)

else()
	# build targets
	set (SRC_LIST ${PRIVATE_SRC_LIST})
	include_directories( ${PRIVATE_INCLUDE_DIRS} )
	add_executable(testFBString ${PRIVATE_SRC_LIST} ${PROJECT_SOURCE_DIR}/test/testFBString.cpp)
	add_executable(testMain ${PRIVATE_SRC_LIST} ${PROJECT_SOURCE_DIR}/test/main.cpp)
endif()
