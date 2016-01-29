/*!
 * \file bookshelf_node_parser.hpp
 * \author Peng Fei
 * \brief Parser definition for bookshelf .nodes files.
 */

#ifndef BOOKSHELF_NODE_PARSER_HPP
#define BOOKSHELF_NODE_PARSER_HPP

#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>

#include "bookshelf_node.h"

namespace qi=boost::spirit::qi;
namespace asc = boost::spirit::ascii;

BOOST_FUSION_ADAPT_STRUCT(
        thueda::BookshelfNode,
        (thueda::Id,         id)
        (thueda::Length,  width)
        (thueda::Length, height)
        (bool,            fixed)
        )

BOOST_FUSION_ADAPT_STRUCT(
        thueda::BookshelfNodes,
        (unsigned,                      num_nodes)
        (unsigned,                  num_terminals)
        (std::vector<thueda::BookshelfNode>, data)
        )

    namespace thueda {

        //! Symbols tables for the Bookshelf Node grammar.
        /*! 
         *  When the parser reads in a line ending with terminal, 
         *  it set the fixed bool to true using this symbol table.
         */
        struct NodeMoveTypeSymbolTable : qi::symbols<char, bool> {
            NodeMoveTypeSymbolTable() {
                add("terminal", true);
            }
        };

        //! The grammar definition for .nodes files.
        /*! 
         * Using PEG format and regular expression, the grammar can be written as follows:
         *
         * \code
         * header_rule        := "^UCLA\s*nodes\s*1.0"
         * comment_rule       := "^#.*$"
         * num_nodes_rule     := "^NumNodes\s*:\s*" uint_
         * num_terminals_rule := "^NumTerminals\s*:\s*" uint_
         * id_rule            := [^\s]*
         * node_rule          := id_rule uint_ uint_ | id unit_ unit_ "terminal$"
         * nodes_rule         := node_rule*
         * start              := header_rule comment_rule* num_nodes_rule num_terminals_rule nodes_rule
         * \endcode
         */
        template<typename Iterator>
            struct BookshelfNodeParser: qi::grammar<Iterator, BookshelfNodes(), asc::space_type> {

                BookshelfNodeParser() : BookshelfNodeParser::base_type(start) {

                    using qi::uint_;
                    using qi::lit;
                    using qi::lexeme;

                    header_rule  = lit("UCLA") >> lit("nodes") >> lit("1.0");

                    comment_rule = lexeme[lit("#") >> *~lit('\n') ];

                    num_nodes_rule      %=  lit("NumNodes")     >> lit(":") >> uint_;
                    num_terminals_rule  %=  lit("NumTerminals") >> lit(":") >> uint_;

                    id_rule     %= lexeme[+~qi::space];
                    node_rule   %= id_rule >> uint_ >> uint_ >> -node_move_type_symbol;
                    nodes_rule  %= *node_rule;

                    start       %= 
                        header_rule        >> 
                        *comment_rule      >> 
                        num_nodes_rule     >>
                        num_terminals_rule >>
                        nodes_rule;
                }

                static NodeMoveTypeSymbolTable  node_move_type_symbol;                        //!< Node file symbol table

                qi::rule<Iterator, qi::unused_type(), asc::space_type>           header_rule; //!< escape headers
                qi::rule<Iterator,        unsigned(), asc::space_type>        num_nodes_rule; //!< store NumNodes
                qi::rule<Iterator,        unsigned(), asc::space_type>    num_terminals_rule; //!< store NumTerminals
                qi::rule<Iterator, qi::unused_type(), asc::space_type>          comment_rule; //!< escape comments
                qi::rule<Iterator,     std::string(), asc::space_type>               id_rule; //!< cache node id
                qi::rule<Iterator,   BookshelfNode(), asc::space_type>             node_rule; //!< parse one line of node
                qi::rule<Iterator, std::vector<BookshelfNode>(), asc::space_type> nodes_rule; //!< parse all lines of nodes
                qi::rule<Iterator,  BookshelfNodes(), asc::space_type>                 start; //!< the start rule, i.e. the entry

            };//end template

        template<typename Iterator>
            NodeMoveTypeSymbolTable BookshelfNodeParser<Iterator>::node_move_type_symbol;

        /*! 
         * \fn bool parse_bookshelf_node(Iterator &iter, Iterator &end, BookshelfNodes &nodes);
         * 
         */
        template<typename Iterator>
            bool parse_bookshelf_node(Iterator &iter, Iterator &end, BookshelfNodes &nodes) {

                BookshelfNodeParser<Iterator> p;
                bool ret = qi::phrase_parse(iter, end, p, qi::ascii::space_type(), nodes);

                return ret;
            }

    }//end namespace thueda

#endif  

