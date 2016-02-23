#library
set(BOOKSHELF_LIB_SRC bookshelf_node.cpp bookshelf_node_parser.hpp
                      bookshelf_pl.cpp   bookshelf_pl_parser.hpp
                                         bookshelf_pl_generator.hpp
                      bookshelf_net.cpp  bookshelf_net_parser.hpp)
add_library(bookshelf SHARED ${BOOKSHELF_LIB_SRC})
set_target_properties(bookshelf PROPERTIES VERSION 0.1 SOVERSION 1)

#executables
#include_directories(${PROJECT_SOURCE_DIR}/src/bookshelf)
#1.NodeParser
set(BOOKSHELF_NODE_PARSER_TEST_SRC bookshelf_node_parser_test.cpp)
add_executable(node_parser_test ${BOOKSHELF_NODE_PARSER_TEST_SRC})
target_link_libraries(node_parser_test bookshelf)
#2.PlParser
set(BOOKSHELF_PL_PARSER_TEST_SRC bookshelf_pl_parser_test.cpp)
add_executable(pl_parser_test   ${BOOKSHELF_PL_PARSER_TEST_SRC})
target_link_libraries(pl_parser_test bookshelf)
#3.PlGenerator
set(BOOKSHELF_PL_GENERATOR_TEST_SRC bookshelf_pl_generator_test.cpp)
add_executable(pl_generator_test   ${BOOKSHELF_PL_GENERATOR_TEST_SRC})
target_link_libraries(pl_generator_test bookshelf)
#4.NetParser
set(BOOKSHELF_NET_PARSER_TEST_SRC bookshelf_net_parser_test.cpp)
add_executable(net_parser_test   ${BOOKSHELF_NET_PARSER_TEST_SRC})
target_link_libraries(net_parser_test bookshelf)

