/*!
 * \file bookshelf_node.cpp
 * \author Peng Fei
 * \brief bookshelf node implementation file.
 */

#include "bookshelf_node.h"

namespace thueda {

    BookshelfNode::BookshelfNode() : id(""), width(0), height(0), fixed(false) {}

    std::ostream & operator<<(std::ostream &out, const BookshelfNode &node) 
    {
        out << node.id << " " << node.width << " " << node.height << " " << node.fixed;
        return out;
    }

    BookshelfNodes::BookshelfNodes() : num_nodes(0), num_terminals(0) 
    {
        data.reserve(10000);
    }

}
