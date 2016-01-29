/*!
 * \file bookshelf_net.cpp
 * \author Peng Fei
 * \brief bookshelf net implementation file.
 */

#include "bookshelf_net.h"

#include <iomanip>

namespace thueda {

    std::ostream & operator<<(std::ostream &out, const BookshelfPin &pin)
    {
        using std::setw;

        out << std::left << setw(8) << pin.id;
        out << std::right;
        switch(pin.io) {
            case Input:
                out << " I : ";
            case Output:
                out << " O : ";
            default:
                ;//error
        }
        out << setw(12) << pin.dx
            << setw(12) << pin.dy;

        return out;
    }

    std::ostream & operator<<(std::ostream &out, const BookshelfNet &net)
    {
        using std::setw;

        out << "NetDegree : ";
        out << setw(4) << net.degree
            << setw(8) << net.id
            << std::endl;

        for(std::vector<BookshelfPin>::const_iterator it=net.pins.begin(); it!=net.pins.end(); ++it) {
            out << *it << std::endl;
        }

        return out;
    }

    BookshelfNets::BookshelfNets() : num_nets(0), num_pins(0) 
    {
        data.reserve(10000);
    }

}//end namespace thueda

