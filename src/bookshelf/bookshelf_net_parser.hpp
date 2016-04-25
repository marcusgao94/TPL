/*!
 * \file bookshelf_net_parser.hpp
 * \author Peng Fei
 * \brief Parser definition for bookshelf .nets files.
 */

#ifndef BOOKSHELF_NET_PARSER_HPP
#define BOOKSHELF_NET_PARSER_HPP

#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>

#include "bookshelf_net.h"

namespace qi=boost::spirit::qi;
namespace asc = boost::spirit::ascii;

BOOST_FUSION_ADAPT_STRUCT(
        thueda::BookshelfPin,
        (thueda::Id,       id)
        (thueda::IOType,   io)
        (thueda::Distance, dx)
        (thueda::Distance, dy)
)

BOOST_FUSION_ADAPT_STRUCT(
        thueda::BookshelfNet,
        (unsigned int,                    degree)
        (thueda::Id,                          id)
        (std::vector<thueda::BookshelfPin>, pins)
)

BOOST_FUSION_ADAPT_STRUCT(
        thueda::BookshelfNets,
        (unsigned int,                  num_nets)
        (unsigned int,                  num_pins)
        (std::list<thueda::BookshelfNet>, data)
)

namespace thueda {

    //! Symbols tables for the Bookshelf Net grammar.
    /*!
     *  When the parser reads a pin,
     *  it set the its IOType according to this symbol table.
     */
    struct PinIOTypeSymbolTable : qi::symbols<char, IOType> {
        PinIOTypeSymbolTable() {
            add("I", IOType::Input );
            add("O", IOType::Output);
        }
    };

    //! The grammar definition for .net files.
    /*!
     * Using PEG format and regular expression, the grammar can be written as follows:
     *
     * \code
     * header_rule        := "^UCLA\s*net\s*1.0"
     * comment_rule       := "^#.*$"
     * num_nets_rule      := "^NumNets\s*:\s*" uint_
     * num_pins_rule      := "^NumPins\s*:\s*" uint_
     * id_rule            := [^\s]*
     * pin_rule           := id_rule pin_io_type_symbol ":" double_ double_
     * net_rule           := "^NetDegree :\s*" uint_ id_rule pin_rule*
     * nets_rule          := net_rule*
     * start              := header_rule comment_rule* pls_rule
     * \endcode
     */
    template<typename Iterator>
    struct BookshelfNetParser: qi::grammar<Iterator, BookshelfNets(), asc::space_type> {

        BookshelfNetParser() : BookshelfNetParser::base_type(start) {

            using qi::uint_;
            using qi::double_;
            using qi::lit;
            using qi::lexeme;
            using qi::eol;

            header_rule    = lit("UCLA") >> lit("nets") >> lit("1.0");

            comment_rule   = lexeme[lit("#") >> *~lit('\n') ];

            num_nets_rule %= lit("NumNets") >> lit(":") >> uint_;
            num_pins_rule %= lit("NumPins") >> lit(":") >> uint_;

            id_rule       %= lexeme[+~qi::space];
            pin_rule      %= id_rule >> pin_io_type_symbol >> lit(":") >> double_ >> double_;
            net_rule      %= lit("NetDegree") >> lit(":") >> uint_ >> id_rule >> *pin_rule;
            nets_rule     %= *net_rule;

            start         %=
                    header_rule   >>
                    *comment_rule >>
                    num_nets_rule >>
                    num_pins_rule >>
                    nets_rule;
        }

        static PinIOTypeSymbolTable pin_io_type_symbol;                             //!< Net file symbol table

        qi::rule<Iterator, qi::unused_type(), asc::space_type>         header_rule; //!< escape headers
        qi::rule<Iterator, qi::unused_type(), asc::space_type>        comment_rule; //!< escapte comments
        qi::rule<Iterator,        unsigned(), asc::space_type>       num_nets_rule; //!< store NumNets
        qi::rule<Iterator,        unsigned(), asc::space_type>       num_pins_rule; //!< store NumPins
        qi::rule<Iterator,     std::string(), asc::space_type>             id_rule; //!< cache net id
        qi::rule<Iterator,    BookshelfPin(), asc::space_type>            pin_rule; //!< parse one line of pin
        qi::rule<Iterator,    BookshelfNet(), asc::space_type>            net_rule; //!< parse one net
        qi::rule<Iterator, std::vector<BookshelfNet>(), asc::space_type> nets_rule; //!< parse all lines of nets
        qi::rule<Iterator,   BookshelfNets(), asc::space_type>               start; //!< the start rule, i.e. the entry

    };//end template

    template<typename Iterator>
    PinIOTypeSymbolTable BookshelfNetParser<Iterator>::pin_io_type_symbol;

    /*!
     * \fn bool parse_bookshelf_net(Iterator &iter, Iterator &end, BookshelfNets &nets);
     *
     */
    template<typename Iterator>
    bool parse_bookshelf_net(Iterator &iter, Iterator &end, BookshelfNets &nets) {

        BookshelfNetParser<Iterator> p;
        bool ret = qi::phrase_parse(iter, end, p, qi::ascii::space_type(), nets);

        return ret;
    }

}//end namespace thueda

#endif//BOOKSHELF_NET_PARSER_HPP

