#libraries
set(BOOKSHELF_LIB_SRC bookshelf_node.cpp bookshelf_node_parser.hpp
                      bookshelf_pl.cpp   bookshelf_pl_parser.hpp   bookshelf_pl_generator.hpp
                      bookshelf_net.cpp  bookshelf_net_parser.hpp)
add_library(bookshelf SHARED ${BOOKSHELF_LIB_SRC})
set_target_properties(bookshelf PROPERTIES VERSION 0.1 SOVERSION 1)

add_subdirectory(unittest)

