cmake_minimum_required(VERSION 2.8)
project (FOLLY-LITE)

# initialize variables
set(USE_JEMALLOC FALSE)
set (PRIVATE_INCLUDE_DIRS "")
set (PRIVATE_SRC_LIST "")

# set complie flags
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-g -std=c++11 -pthread")

# set output path
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(BUILD_OUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# set folly source list
set(FOLLY_SRC_LIST
	${PROJECT_SOURCE_DIR}/folly/folly/SpookyHashV1.cpp
	${PROJECT_SOURCE_DIR}/folly/folly/SpookyHashV2.cpp
	${PROJECT_SOURCE_DIR}/folly/folly/detail/MallocImpl.cpp
	${PROJECT_SOURCE_DIR}/folly/folly/portability/BitsFunctexcept.cpp
	#${PROJECT_SOURCE_DIR}/folly/folly/Malloc.cpp
	)

# set test file list
set(TEST_SRC_LIST
	${PROJECT_SOURCE_DIR}/test/testFBVector.cpp
)

# decide use boost or not
if (WIN32)
	message("================= WINDOWS 32 =======================")

elseif (APPLE)
	message("================= UNIX ==============================")

endif()


# set source files and include directories
set (PRIVATE_SRC_LIST ${PRIVATE_SRC_LIST}
	${FOLLY_SRC_LIST}
	${TEST_SRC_LIST}
	)

set (PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/folly
	${PROJECT_SOURCE_DIR}/jemalloc/include
	)


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

	include (ExternalProject)
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
	add_library(libjemalloc SHARED IMPORTED)
	add_dependencies (libjemalloc libjemalloc-target)

	elseif (NOT "PRIVATE_LIB_JEMALLOC-NOTFOUND" STREQUAL "${PRIVATE_LIB_JEMALLOC}")
	add_library(libjemalloc SHARED IMPORTED)
	set_target_properties(libjemalloc PROPERTIES
	    IMPORTED_LOCATION "${PRIVATE_LIB_JEMALLOC}"
	    )
	endif ()
	message("compile jemalloc end.")
	
    link_libraries(${CMAKE_CURRENT_BINARY_DIR}/lib/libjemalloc.so)
    set (SRC_LIST ${PRIVATE_SRC_LIST})
	include_directories( ${PRIVATE_INCLUDE_DIRS} )
	add_executable(testMain ${PRIVATE_SRC_LIST})
	add_dependencies(testMain libjemalloc)
else()
	set (SRC_LIST ${PRIVATE_SRC_LIST})
	include_directories( ${PRIVATE_INCLUDE_DIRS} )
	add_executable(testMain ${PRIVATE_SRC_LIST})
endif()

# gcc ex_stats_print.c -o ex_stats_print -I/home/hqx/code/folly-lite/folly-lite/jemalloc/include -L/home/hqx/code/folly-lite/folly-lite/jemalloc/lib -Wl,-rpath,/home/hqx/code/folly-lite/folly-lite/jemalloc/lib -ljemalloc-navi
# gcc ex_stats_print.c -o ex_stats_print -I/home/hqx/Downloads/jemalloc-cmake-4.3.0/include -L/home/hqx/Downloads/jemalloc-cmake-4.3.0/lib -Wl,-rpath,/home/hqx/Downloads/jemalloc-cmake-4.3.0/lib -ljemalloc
