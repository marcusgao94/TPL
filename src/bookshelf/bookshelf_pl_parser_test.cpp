/*!
 * \file bookshelf_pl_parser_test.cpp
 * \author Peng Fei
 * \brief bookshelf pl parser unittest.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iterator>
#include <cstdlib>
#include <cstring>

#include "bookshelf_pl_parser.hpp"

using namespace std;
using namespace thueda;

SCENARIO("Test .pl files", "[case 1]") {

    GIVEN("A .pl file and a BookshelfPlParser") {
        char path[200];
        char *benchmark;
        benchmark = std::getenv("BENCHMARK");
        std::strcpy(path, benchmark);
        std::strcat(path, "/ispd2005/bigblue1/bigblue1.pl");
        ifstream in(path, ios_base::in);
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

        cout << endl;
    }//end GIVEN

}//end SCENARIO

