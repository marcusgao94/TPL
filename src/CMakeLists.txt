cmake_minimum_required (VERSION 3.2.2)
project (TPL)

#system specific setting
if( CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message("OS is Mac OS X!!!")
    set(CMAKE_AR x86_64-apple-darwin1X-ar)
elseif( CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("OS is Linux!!!")
endif()

#build flags setting
set(CMAKE_CXX_FLAGS         "$ENV{CXXFLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")

#output directory setting
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/bookshelf)

#boost setting
set(Boost_USE_MULTITHREDED OFF)
find_package(Boost COMPONENTS system filesystem REQUIRED)
if(Boost_FOUND)
    include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
    message(STATUS "Boost LIB : ${Boost_LIBRARIES}")
else(Boost_FOUND)
    message("Boost not found!")
endif(Boost_FOUND)

#stxxl setting
find_package(STXXL REQUIRED)
if(STXXL_FOUND)
    message(STATUS "STXXL LIB : ${STXXL_LIBRARIES}")
    include_directories(SYSTEM ${STXXL_INCLUDE_DIRS})
else(STXXL_FOUND)
    message("stxxl not found!")
endif(STXXL_FOUND)


#subdirs
add_subdirectory(bookshelf)
add_subdirectory(tpl)
add_subdirectory(unittest)

