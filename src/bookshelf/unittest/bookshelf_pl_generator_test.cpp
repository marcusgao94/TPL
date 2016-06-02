/*!
 * \file bookshelf_pl_generator_test.cpp
 * \author Peng Fei
 * \brief bookshelf pl generator unittest.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iterator>
#include <cstdlib>
#include <cstring>

#include "bookshelf_pl_parser.hpp"
#include "bookshelf_pl_generator.hpp"

using namespace std;
using namespace thueda;

SCENARIO("Test .pl files", "[case 1]") {

    GIVEN("BookshelfPls constructed from a BookshelfPlParser.") {
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

        BookshelfPls pls;
        qi::phrase_parse(iter, end, p, qi::ascii::space_type(), pls);

        ofstream out("bigblue1.pl", ios_base::out);
        ostream_iterator<char> ositer(out, "");

        WHEN("we generate the output") {
            bool ret = generate_bookshelf_pl(ositer, pls);

            THEN("we get a .pl file the same as the previous one except the comments") {
                REQUIRE(ret == true);
            }
        }

        cout << endl;
    }//end GIVEN

}//end SCENARIO

