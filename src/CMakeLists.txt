CMAKE_MINIMUM_REQUIRED (VERSION 3.0)
PROJECT (TPL)

#build flags
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS}       -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}       -Wall -std=c++11 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -std=c++11 -DNDEBUG")


#global settings
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)

#find package
find_package(Boost COMPONENTS system filesystem REQUIRED)
#if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIR})
    #set(CMAKE_INCLUDE_PATH "${CMAKE_INCLUDE_PATH} ${Boost_INCLUDE_PATH}")
    #set(CMAKE_LIBRARY_PATH "${CMAKE_LIBRARY_PATH} ${PROJECT_BINARY_DIR}/lib ${Boost_LIBRARY_PATH}")
	set(Boost_USE_MULTITHREDED OFF)
	#message("${Boost_LIBRARIES}")
#else(Boost_FOUND)
    #message("Boost not found!")
#endif(Boost_FOUND)


#sub dirs
#ADD_SUBDIRECTORY(bookshelf)
set(bookshelf bookshelf/bookshelf_net.h bookshelf/bookshelf_net.cpp bookshelf/bookshelf_net_parser.hpp
		bookshelf/bookshelf_node.h bookshelf/bookshelf_node.cpp bookshelf/bookshelf_node_parser.hpp
		bookshelf/bookshelf_pl.h bookshelf/bookshelf_pl.cpp bookshelf/bookshelf_pl_parser.hpp)

set(sourcefile tpl/tpl_circuit.cpp tpl/tpl_circuit_test.cpp)


# ADD_SUBDIRECTORY(tpl)
#libraries
#set(TPL_LIB_SRC tpl/tpl_circuit.cpp)
#add_library(tpl SHARED ${TPL_LIB_SRC})
#set_target_properties(tpl PROPERTIES VERSION 0.1 SOVERSION 1)

#executables
#1.TplCircuit
#set(TPL_CIRCUIT_TEST_SRC tpl/tpl_circuit_test.cpp)
add_executable(tpl_circuit_test ${sourcefile} ${bookshelf})
target_link_libraries(tpl_circuit_test ${Boost_LIBRARIES})





#ADD_SUBDIRECTORY(tpl_pf)
#ADD_SUBDIRECTORY(tpl_gy)

