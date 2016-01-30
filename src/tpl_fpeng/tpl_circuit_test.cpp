/*!
 * \file tpl_circuit_test.cpp
 * \author Peng Fei
 * \brief tpl circuit related data structures unittest.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>

#include "tpl_circuit.h"

using namespace std;
using namespace tpl;

SCENARIO("Test load_circuit", "[case 1]") {

    GIVEN("A test case file path") {
        char path[200];
        char *home;
        home = std::getenv("HOME");
        std::strcpy(path, home);
        std::strcat(path, "/Workspace/TPL/benchmark/ispd2005/bigblue1");

        string _path(path);

        WHEN("we load a circuit") {
            bool ret = TplDB::db()->load_circuit(_path);
            THEN("a circuit's information is loaded") {
                REQUIRE(ret == true);
            }
        }
    }//end GIVEN

}//end SCENARIO

