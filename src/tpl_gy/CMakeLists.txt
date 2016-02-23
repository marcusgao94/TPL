cmake_minimum_required(VERSION 3.3)
project(tpl)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRECTORY})

set(ENV{EIGEN3} "/Users/user/Downloads/eigen")
include_directories($ENV{EIGEN3})

set(bookshelf
	../bookshelf/bookshelf_net.h ../bookshelf/bookshelf_net.cpp ../bookshelf/bookshelf_net_parser.hpp
	../bookshelf/bookshelf_node.h ../bookshelf/bookshelf_node.cpp ../bookshelf/bookshelf_node_parser.hpp
	../bookshelf/bookshelf_pl.h ../bookshelf/bookshelf_pl.cpp ../bookshelf/bookshelf_pl_parser.hpp
)
file(GLOB_RECURSE sf *.cpp *.h)

set(SOURCE_FILES ${sf} ${bookshelf})
add_executable(tpl ${SOURCE_FILES})
target_link_libraries(tpl ${Boost_LIBRARIES})
