/*!
 * \file bookshelf_pl_generator.hpp
 * \author Peng Fei
 * \brief Generator definition for bookshelf .pl files.
 */

#ifndef BOOKSHELF_PL_GENERATOR_HPP
#define BOOKSHELF_PL_GENERATOR_HPP

#include <boost/spirit/include/karma.hpp>

#include "bookshelf_pl.h"

namespace thueda {
    namespace karma = boost::spirit::karma;

    /*! 
     * \fn bool generate_bookshelf_pl(OutputIterator &sink, const BookshelfPls &pls);
     * 
     */
    template<typename OutputIterator>
        bool generate_bookshelf_pl(OutputIterator &sink, const BookshelfPls &pls) {
            using karma::generate;
            using karma::lit;
            using karma::stream;
            using karma::eol;

            bool r = generate(
                    sink,
                    lit("UCLA pl 1.0\n") << stream % eol,
                    pls.data
                    );
            return r;

        };

}//end namespace thueda

#endif  

