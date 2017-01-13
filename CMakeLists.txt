project (FOLLY-LITE)

# initialize variables
set(USE_JEMALLOC FALSE)
set (PRIVATE_INCLUDE_DIRS "")
set (PRIVATE_SRC_LIST "")

# set complie flags
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-Wall")
add_compile_options(-std=c++11)

# set output path
set(EXECUTABLE_OUTPUT_PATH ./bin)
set(LIBRARY_OUTPUT_PATH ./lib)

# set folly source list
set(FOLLY_SRC_LIST
	${PROJECT_SOURCE_DIR}/folly/folly/SpookyHashV1.cpp
	${PROJECT_SOURCE_DIR}/folly/folly/SpookyHashV2.cpp
	${PROJECT_SOURCE_DIR}/folly/folly/detail/MallocImpl.cpp
	${PROJECT_SOURCE_DIR}/folly/folly/portability/BitsFunctexcept.cpp
	)

# set test file list
set(TEST_SRC_LIST
	${PROJECT_SOURCE_DIR}/test/testFBVector.cpp
)

# decide use boost or not
if (WIN32)
	message("================= WINDOWS 32 =======================")
	aux_source_directory(${PROJECT_SOURCE_DIR}/boost BOOST_SRC_LIST)	
  	set(PRIVATE_SRC_LIST ${PRIVATE_SRC_LIST} ${BOOST_SRC_LIST})
  	set(PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/boost)
elseif (UNIX)
	message("================= UNIX ==============================")
	aux_source_directory(${PROJECT_SOURCE_DIR}/boost BOOST_SRC_LIST)
  	set(PRIVATE_SRC_LIST ${PRIVATE_SRC_LIST} ${BOOST_SRC_LIST})
  	set(PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/boost)
endif()

# decide use jemalloc or not
if (NOT USE_JEMALLOC)
	message("================ USE jemalloc =======================")
	aux_source_directory(${PROJECT_SOURCE_DIR}/jemalloc/src JEMALLOC_SRC_LIST)
	set(PRIVATE_SRC_LIST ${PRIVATE_SRC_LIST} ${JEMALLOC_SRC_LIST})
	set(PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/jemalloc/include)
endif()


# set source files and include directories
set (PRIVATE_SRC_LIST ${PRIVATE_SRC_LIST}
	${FOLLY_SRC_LIST}
	)

set (PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/folly
	)

set (SRC_LIST ${PRIVATE_SRC_LIST})
include_directories( ${PRIVATE_INCLUDE_DIRS} )

# build target
add_library(folly-lite SHARED ${FOLLY_SRC_LIST})
add_executable(testMain MACOSX_BUNDLE ${TEST_SRC_LIST})