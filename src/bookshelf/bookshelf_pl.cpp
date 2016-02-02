/*!
 * \file bookshelf_pl.cpp
 * \author Peng Fei
 * \brief bookshelf pl implementation file.
 */

#include "bookshelf_pl.h"

#include <iomanip>

namespace thueda {

    std::ostream & operator<<(std::ostream &out, const BookshelfPl &pl) 
    {
        using std::setw;
        using std::fixed;
        using std::setprecision;

        out << std::left
            << setw(8) << pl.id 
            << std::right
            << setprecision(10) << fixed << pl.x << "\t"
            << setprecision(10) << fixed << pl.y << "\t"
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

