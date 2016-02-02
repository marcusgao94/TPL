/*!
 * \file bookshelf_pl.h
 * \author Peng Fei
 * \brief In-memory data structure definition for bookshelf .pl files.
 */

#ifndef BOOKSHELF_PL_H
#define BOOKSHELF_PL_H

#include <vector>
#include <iostream>

#include "utils.h"

namespace thueda {

    //! public class storing one node's x and y coordinate information.
    /*!
     * When BookshelfNodeParser parse .pl files, the nodes' information are stored in this struct. 
     * id is the node's name, and x and y stores the node's bounding box's location information. 
     * A bool fixed is used to indicate wether this node is movable within the chip's boundary.
     * A default constructor is explicitly provided mainly for setting the fixed bool to false.
     */
    struct BookshelfPl {
        Id        id;  //!< A node's id.
        Coordinate x;  //!< A node's x coordinate.
        Coordinate y;  //!< A node's y coordinate.
        bool   fixed;  //!< A node's move mode flag.

        //BookshelfPl(); //!< Default constructor for BookshelfNode.
        //BookshelfPl(const Id &_id, const Coordinate &_x, const Coordinate &_y, const bool &_fixed); //!< Normal constructor.
    };

    /*!
     * \fn std::ostream & operator<<(std::ostream &out, const BookshelfPl &pl);
     * \brief A help function used for printing the BookshelfPl's information.
     * \param out The output stream object.
     * \param pl The pl information to be printed.
     */
    std::ostream & operator<<(std::ostream &out, const BookshelfPl &pl);


    //! The in-memory representation class for the .pl files.
    /*!
     * This struct can be seen as the in-memory equivalent of the .pl files, 
     * which contains all the nodes' locations on the chip. 
     * A default constructor is provided for allocating some storage space in the first time.
     */
    struct BookshelfPls {
        std::vector<BookshelfPl> data;  //!< Vector storing all the pl information in .pl files

        BookshelfPls();                 //!< Default constructor for BookshelfPl
    };

}//end namespace thueda

#endif

