/*!
 * \file bookshelf_node_parser_test.cpp
 * \author Peng Fei
 * \brief bookshelf node parser unittest.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <cstdlib>
#include <cstring>

#include "bookshelf_node_parser.hpp"

using namespace std;
using namespace thueda;

SCENARIO("Test .nodes files", "[case 1]") {

    GIVEN("A .nodes file and a BookshelfNodeParser") {
        char path[200];
        char *benchmark;
        benchmark = std::getenv("BENCHMARK");
        std::strcpy(path, benchmark);
        std::strcat(path, "/ispd2005/bigblue1/bigblue1.nodes");
        ifstream in(path, ios_base::in);
        in.unsetf(ios::skipws);

        string storage;
        copy(istream_iterator<char>(in),
             istream_iterator<char>(),
             back_inserter(storage));

        string::const_iterator iter = storage.begin();
        string::const_iterator end  = storage.end();

        BookshelfNodeParser< string::const_iterator > p;
        BookshelfNodes n;

        WHEN("we parse the input") {
            bool ret = qi::phrase_parse(iter, end, p, qi::ascii::space_type(), n);

            THEN("we get a bunch of Bookshelf nodes") {
                REQUIRE(ret == true);
            }
        }

        cout << endl;
    }//end GIVEN

}//SCENARIO

