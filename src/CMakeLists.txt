cmake_minimum_required (VERSION 3.2.2)
project (TPL)

#build flags setting
set(CMAKE_CXX_FLAGS         "$ENV{CXXFLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")

#output directory setting
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_AR x86_64-apple-darwin1X-ar)

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/bookshelf)

#boost setting
set(Boost_USE_MULTITHREDED OFF)
find_package(Boost COMPONENTS system filesystem REQUIRED)
if(Boost_FOUND)
    include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
else(Boost_FOUND)
    message("Boost not found!")
endif(Boost_FOUND)

#subdirs
add_subdirectory(bookshelf)
add_subdirectory(tpl)
add_subdirectory(unittest)

