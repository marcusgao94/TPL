/*!
 * \file bookshelf_net.cpp
 * \author Peng Fei
 * \brief bookshelf net implementation file.
 */

#include <iomanip>

#include "bookshelf_net.h"

namespace thueda {

    std::ostream & operator<<(std::ostream &out, const BookshelfPin &pin)
    {
        using std::setw;

        out << std::left << setw(8) << pin.id;
        out << std::right;
        switch(pin.io) {
            case IOType::Input:
                out << " I : ";
                break;
            case IOType::Output:
                out << " O : ";
                break;
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

