/*!
 * \file bookshelf_net.h
 * \author Peng Fei
 * \brief In-memory data structure definition for bookshelf .nets files.
 */

#ifndef BOOKSHELF_NET_H
#define BOOKSHELF_NET_H

#include <vector>
#include <iostream>

#include "utils.h"

namespace thueda {

    //! public class storing one pin's IO, and x y offset information.
    /*!
     * When BookshelfNetParser parses .nets files, the pins' information are stored in this struct. 
     */
    struct BookshelfPin {
        Id       id; //!< A pin's id
        IOType   io; //!< A pin's IO type
        Distance dx; //!< A pin's x offset from the center of its node
        Distance dy; //!< A pin's y offset from the center of its node
    };

    /*!
     * \fn std::ostream & operator<<(std::ostream &out, const BookshelfPin &pin);
     * \brief A help function used for printing the BookshelfPin's information.
     * \param out  the output stream object
     * \param pin the pin to be printed
     */
    std::ostream & operator<<(std::ostream &out, const BookshelfPin &pin);


    //! public class storing one net's degree and its associated pins.
    /*!
     * When BookshelfNetParser parses .nets files, the nets' information are stored in this struct. 
     */
    struct BookshelfNet {
        Id                            id; //!< A net's id
        unsigned int              degree; //!< A net's degree, i.e. number of pins attached on this net 
        std::vector<BookshelfPin>   pins; //!< A net's attached pins
    };

    /*!
     * \fn std::ostream & operator<<(std::ostream &out, const BookshelfNet &net);
     * \brief A help function used for printing the BookshelfNet's information.
     * \param out  the output stream object
     * \param net the net to be printed
     */
    std::ostream & operator<<(std::ostream &out, const BookshelfNet &net);


    //! The in-memory representation class for the .nets files.
    /*!
     * This struct can be seen as the in-memory equivalent of the .nets files, 
     * which contains all the pins' and the net' information.
     * A default constructor is provided for allocating some storage space in the first time.
     */
    struct BookshelfNets {
        unsigned int num_nets;              //!< Number of nets in .nets files
        unsigned int num_pins;              //!< Number of pins in .nets files
        std::vector<BookshelfNet> data;     //!< Vector storing all the nets in .nets files

        //! Default constructor for BookshelfNets
        BookshelfNets();
    };

}//end namespace thueda

#endif//BOOKSHELF_NET_H

