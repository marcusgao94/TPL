/*!
 * \file bookshelf_node.h
 * \author Peng Fei
 * \brief In-memory data structure definition for bookshelf .nodes files.
 */

#ifndef BOOKSHELF_NODE_H
#define BOOKSHELF_NODE_H

#include <vector>
#include <iostream>

#include "utils.h"

namespace thueda {

    //! public class storing one node's width and height information.
    /*!
     * When BookshelfNodeParser parse .nodes files, the nodes' information are stored in this struct. 
     * A bool fixed is used to indicate wether this node is movable within the chip's boundary.
     * A default constructor is explicitly provided mainly for setting the fixed bool to false.
     */
    struct BookshelfNode {
        Id         id;   //!< A node's id
        Length  width;   //!< A node's width 
        Length height;   //!< A node's height
        bool    fixed;   //!< A node's move mode flag

        BookshelfNode(); //!< Default constructor for BookshelfNode
    };

    /*!
     * \fn std::ostream & operator<<(std::ostream &out, const BookshelfNode &node);
     * \brief A help function used for printing the BookshelfNode's information.
     * \param out  the output stream object
     * \param node the node to be printed
     */
    std::ostream & operator<<(std::ostream &out, const BookshelfNode &node);


    //! The in-memory representation class for the .nodes files.
    /*!
     * This struct can be seen as the in-memory equivalent of the .nodes files, 
     * which contains all the nodes' information as well as other metadata, such as number of nodes.
     * A default constructor is provided for allocating some storage space in the first time.
     */
    struct BookshelfNodes {
        unsigned              num_nodes;  //!< Number of node in .node files
        unsigned          num_terminals;  //!< Number of terminals in .node files
        std::vector<BookshelfNode> data;  //!< Vector storing all the nodes in .nodes files

        BookshelfNodes();                 //!< Default constructor for BookshelfNodes
    };

}//end namespace thueda

#endif

