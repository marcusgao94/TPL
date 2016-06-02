#executables
#1.NodeParser
set(BOOKSHELF_NODE_PARSER_TEST_SRC bookshelf_node_parser_test.cpp)
add_executable(test_bk_node_parser ${BOOKSHELF_NODE_PARSER_TEST_SRC})
target_link_libraries(test_bk_node_parser bookshelf)

#2.PlParser
set(BOOKSHELF_PL_PARSER_TEST_SRC bookshelf_pl_parser_test.cpp)
add_executable(test_bk_pl_parser   ${BOOKSHELF_PL_PARSER_TEST_SRC})
target_link_libraries(test_bk_pl_parser bookshelf)

#3.PlGenerator
set(BOOKSHELF_PL_GENERATOR_TEST_SRC bookshelf_pl_generator_test.cpp)
add_executable(test_bk_pl_generator ${BOOKSHELF_PL_GENERATOR_TEST_SRC})
target_link_libraries(test_bk_pl_generator bookshelf)

#4.NetParser
set(BOOKSHELF_NET_PARSER_TEST_SRC bookshelf_net_parser_test.cpp)
add_executable(test_bk_net_parser   ${BOOKSHELF_NET_PARSER_TEST_SRC})
target_link_libraries(test_bk_net_parser bookshelf)