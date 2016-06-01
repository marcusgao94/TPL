/*!
 * \file bookshelf_pl_parser.hpp
 * \author Peng Fei
 * \brief Parser definition for bookshelf .pl files.
 */

#ifndef BOOKSHELF_PL_PARSER_HPP
#define BOOKSHELF_PL_PARSER_HPP

#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>

#include "bookshelf_pl.h"

namespace qi=boost::spirit::qi;
namespace asc = boost::spirit::ascii;

BOOST_FUSION_ADAPT_STRUCT(
        thueda::BookshelfPl,
        (thueda::Id,            id)
        (thueda::Coordinate,     x)
        (thueda::Coordinate,     y)
        (bool,               fixed)
)

BOOST_FUSION_ADAPT_STRUCT(
        thueda::BookshelfPls,
        (std::vector<thueda::BookshelfPl>, data)
)

namespace thueda {

    //! Symbols tables for the Bookshelf Pl grammar.
    /*!
     *  When the parser reads in a line ending with "/FIXED",
     *  it set the fixed bool to true using this symbol table.
     */
    struct PlMoveTypeSymbolTable : qi::symbols<char, bool> {
        PlMoveTypeSymbolTable() {
            add("/FIXED", true);
        }
    };

    //! The grammar definition for .pl files.
    /*!
     * Using PEG format and regular expression, the grammar can be written as follows:
     *
     * \code
     * header_rule        := "^UCLA\s*pl\s*1.0"
     * comment_rule       := "^#.*$"
     * id_rule            := [^\s]*
     * pl_rule            := id_rule double_ double_ ":\sN"| id double_ double_ ":\sN\s\/FIXED$"
     * pls_rule           := pl_rule*
     * start              := header_rule comment_rule* pls_rule
     * \endcode
     */
    template<typename Iterator>
    struct BookshelfPlParser: qi::grammar<Iterator, BookshelfPls(), asc::space_type> {

        BookshelfPlParser() : BookshelfPlParser::base_type(start) {

            using qi::double_;
            using qi::lit;
            using qi::lexeme;
            using qi::eoi;

            header_rule  = lit("UCLA") >> lit("pl") >> lit("1.0");

            comment_rule = lexeme[lit("#") >> *~lit('\n') ];

            id_rule     %= lexeme[+~qi::space];
            pl_rule     %= id_rule >> double_ >> double_ >> lit(":") >> lit("N") >> -pl_move_type_symbol;
            pls_rule    %= *pl_rule;

            start       %=
                    header_rule   >>
                    *comment_rule >>
                    pls_rule      >>
                    eoi;
        }

        static PlMoveTypeSymbolTable  pl_move_type_symbol;                        //!< Pl file symbol table

        qi::rule<Iterator, qi::unused_type(), asc::space_type>       header_rule; //!< escape headers
        qi::rule<Iterator, qi::unused_type(), asc::space_type>      comment_rule; //!< escapte comments
        qi::rule<Iterator,     std::string(), asc::space_type>           id_rule; //!< cache node id
        qi::rule<Iterator,     BookshelfPl(), asc::space_type>           pl_rule; //!< parse one line of pl
        qi::rule<Iterator, std::vector<BookshelfPl>(), asc::space_type> pls_rule; //!< parse all lines of pls
        qi::rule<Iterator,    BookshelfPls(), asc::space_type>             start; //!< the start rule, i.e. the entry

    };//end template

    template<typename Iterator>
    PlMoveTypeSymbolTable BookshelfPlParser<Iterator>::pl_move_type_symbol;

    /*!
     * \fn bool parse_bookshelf_pl(Iterator &iter, Iterator &end, BookshelfPls &pls);
     *
     */
    template<typename Iterator>
    bool parse_bookshelf_pl(Iterator &iter, Iterator &end, BookshelfPls &pls) {

        BookshelfPlParser<Iterator> p;
        bool ret = qi::phrase_parse(iter, end, p, qi::ascii::space_type(), pls);

        return ret;
    }

}//end namespace thueda

#endif//BOOKSHELF_PL_PARSER_HPP
