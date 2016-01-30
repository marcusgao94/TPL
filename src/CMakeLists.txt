#global settings
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

find_package(Boost REQUIRED COMPONENTS
    filesystem
    system
    )

#build flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -O0 -Wall -std=c++11 -g -ggdb")
    message("${CMAKE_CXX_FLAGS}")
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -std=c++11 -DNDEBUG")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

#sub dirs
ADD_SUBDIRECTORY(bookshelf)
ADD_SUBDIRECTORY(tpl_fpeng)
