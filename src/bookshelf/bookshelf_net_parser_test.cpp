/*!
 * \file bookshelf_net_parser_test.cpp
 * \author Peng Fei
 * \brief bookshelf net parser unittest.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <iterator>

#include "bookshelf_net_parser.hpp"

using namespace std;
using namespace thueda;

SCENARIO("Test .net files", "[case 1]") {

    GIVEN("A .nets file and a BookshelfNetParser") {
        ifstream in("/home/fpeng/Workspace/TPL/benchmark/bigblue1/bigblue1.nets", ios_base::in);
        in.unsetf(ios::skipws);

        string storage;
        copy(istream_iterator<char>(in),
             istream_iterator<char>(),
             back_inserter(storage));

        string::const_iterator iter = storage.begin();
        string::const_iterator end  = storage.end();

        BookshelfNetParser< string::const_iterator > p;

        BookshelfNets nets;

        WHEN("we parse the input") {
            bool ret = qi::phrase_parse(iter, end, p, qi::ascii::space_type(), nets);

            THEN("we get a bunch of Bookshelf nets") {
                REQUIRE(ret == true);
            }
        }

    }//end GIVEN

    cout << endl;
}

