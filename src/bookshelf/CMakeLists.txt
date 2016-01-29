#build flags
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

set(CMAKE_CXX_FLAGS_DEBUG   "$ENV{CXXFLAGS} -O0 -Wall -std=c++11 -g -ggdb ")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c++11")

#targets
#1.NodeParser
set(BOOKSHELF_NODE_PARSER_TEST_SRC bookshelf_node_parser_test.cpp bookshelf_node_parser.hpp bookshelf_node.cpp)
add_executable(node_parser_test ${BOOKSHELF_NODE_PARSER_TEST_SRC})

#2.PlParser
set(BOOKSHELF_PL_PARSER_TEST_SRC bookshelf_pl_parser_test.cpp bookshelf_pl_parser.hpp bookshelf_pl.cpp)
add_executable(pl_parser_test   ${BOOKSHELF_PL_PARSER_TEST_SRC})

#3.PlGenerator
set(BOOKSHELF_PL_GENERATOR_TEST_SRC bookshelf_pl_generator_test.cpp bookshelf_pl_generator.hpp bookshelf_pl.cpp)
add_executable(pl_generator_test   ${BOOKSHELF_PL_GENERATOR_TEST_SRC})


#4.NetParser
set(BOOKSHELF_NET_PARSER_TEST_SRC bookshelf_net_parser_test.cpp bookshelf_net_parser.hpp   bookshelf_net.cpp)
add_executable(net_parser_test   ${BOOKSHELF_NET_PARSER_TEST_SRC})
