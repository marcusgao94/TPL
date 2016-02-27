#global settings
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)

#find package
find_package(Boost REQUIRED COMPONENTS system filesystem)
if(Boost_FOUND)
	set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${Boost_INCLUDE_PATH})
	set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${Boost_LIBRARY_PATH})
else(Boost_FOUND)
	message("Boost not found!")
endif(Boost_FOUND)

#build flags
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS_DEBUG}       -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}       -Wall -std=c++11 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -std=c++11 -DNDEBUG")

#sub dirs
ADD_SUBDIRECTORY(bookshelf)
ADD_SUBDIRECTORY(tpl)
#ADD_SUBDIRECTORY(tpl_pf)
#ADD_SUBDIRECTORY(tpl_gy)
