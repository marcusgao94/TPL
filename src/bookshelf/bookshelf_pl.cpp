/*!
 * \file bookshelf_pl.cpp
 * \author Peng Fei
 * \brief bookshelf pl implementation file.
 */

#include "bookshelf_pl.h"

#include <iomanip>

namespace thueda {

    BookshelfPl::BookshelfPl() : id(""), x(0), y(0), fixed(false) {}

    std::ostream & operator<<(std::ostream &out, const BookshelfPl &pl) 
    {
        using std::setw;

        out << std::left
            << setw(8) << pl.id 
            << std::right
            << setw(8) << pl.x
            << setw(8) << pl.y 
            << "\t: N"; 

        if(pl.fixed) {
            out << " /FIXED";
        }
        return out;
    }

    BookshelfPls::BookshelfPls() 
    {
        data.reserve(10000);
    }

}//end namespace thueda

