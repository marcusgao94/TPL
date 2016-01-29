/*!
 * \file bookshelf_pl_parser_test.cpp
 * \author Peng Fei
 * \brief bookshelf pl parser unittest.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <iterator>

#include "bookshelf_pl_parser.hpp"

using namespace std;
using namespace thueda;

SCENARIO("Test .pl files", "[case 1]") {

    GIVEN("A .pl file and a BookshelfPlParser") {
        ifstream in("/home/fpeng/Workspace/TPL/benchmark/bigblue1/bigblue1.pl", ios_base::in);
        in.unsetf(ios::skipws);

        string storage;
        copy(istream_iterator<char>(in),
             istream_iterator<char>(),
             back_inserter(storage));

        string::const_iterator iter = storage.begin();
        string::const_iterator end  = storage.end();

        BookshelfPlParser< string::const_iterator > p;

        BookshelfPls n;

        WHEN("we parse the input") {
            bool ret = qi::phrase_parse(iter, end, p, qi::ascii::space_type(), n);

            THEN("we get a bunch of Bookshelf pls") {
                REQUIRE(ret == true);
            }
        }

    }//end GIVEN

    cout << endl;
}

