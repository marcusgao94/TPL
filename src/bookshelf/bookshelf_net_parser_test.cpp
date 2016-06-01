/*!
 * \file bookshelf_net_parser_test.cpp
 * \author Peng Fei
 * \brief bookshelf net parser unittest.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iterator>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "bookshelf_net_parser.hpp"

using namespace std;
using namespace thueda;

SCENARIO("Test .net files", "[case 1]") {

    GIVEN("A .nets file and a BookshelfNetParser") {
        char path[200];
        char *benchmark;
        benchmark= std::getenv("BENCHMARK");
        std::strcpy(path, benchmark);
        std::strcat(path, "/ispd2005/bigblue1/bigblue1.nets");
        ifstream in(path, ios_base::in);
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


            ofstream fout("/home/gaoy/TPL/src/tem");

            for (list<BookshelfNet>::iterator iter = nets.data.begin();
                    iter != nets.data.end(); ++iter) {
                fout << "id = " << iter->id << endl;
                fout << "degree = " << iter->degree << endl;
                fout << endl << endl;
            }
            fout.close();


            THEN("we get a bunch of Bookshelf nets") {
                cout << "data.size = " << nets.data.size() << endl;
                cout << "num_nets = " << nets.num_nets << endl;
                REQUIRE(ret == true);
                REQUIRE(nets.data.size() == nets.num_nets);
            }
        }

        cout << endl;
    }//end GIVEN

}//end SCENARIO

