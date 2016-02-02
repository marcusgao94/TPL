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

#define db (*tpl::TplDB::db())

SCENARIO("adaptec1", "[adaptec1]") {

    GIVEN("A circuit adaptec1") {
        char path[200];
        char *home;
        home = std::getenv("HOME");
        std::strcpy(path, home);
        std::strcat(path, "/Workspace/TPL/benchmark/ispd2005/adaptec1");

        bool load_status = db.load_circuit(path);

        REQUIRE(load_status == true);
        REQUIRE(db.get_chip_width()  == 11589);
        REQUIRE(db.get_chip_height() == 11589);
        REQUIRE(db.get_number_of_free_modules() == 210904);
    }
}//end adaptec1

SCENARIO("adaptec2", "[adaptec2]") {

    GIVEN("A circuit adaptec2") {
        char path[200];
        char *home;
        home = std::getenv("HOME");
        std::strcpy(path, home);
        std::strcat(path, "/Workspace/TPL/benchmark/ispd2005/adaptec2");

        bool load_status = db.load_circuit(path);

        REQUIRE(load_status == true);
        REQUIRE(db.get_chip_width()  == 15244);
        REQUIRE(db.get_chip_height() == 15244);
        REQUIRE(db.get_number_of_free_modules() == 254457);
    }
}//end adaptec2

SCENARIO("adaptec3", "[adaptec3]") {

    GIVEN("A circuit adaptec3") {
        char path[200];
        char *home;
        home = std::getenv("HOME");
        std::strcpy(path, home);
        std::strcat(path, "/Workspace/TPL/benchmark/ispd2005/adaptec3");

        bool load_status = db.load_circuit(path);

        REQUIRE(load_status == true);
        REQUIRE(db.get_chip_width()  == 23190);
        REQUIRE(db.get_chip_height() == 23386);
        REQUIRE(db.get_number_of_free_modules() == 450927);
    }
}//end adaptec3

SCENARIO("adaptec4", "[adaptec4]") {

    GIVEN("Test case adaptec4") {
        char path[200];
        char *home;
        home = std::getenv("HOME");
        std::strcpy(path, home);
        std::strcat(path, "/Workspace/TPL/benchmark/ispd2005/adaptec4");

        bool load_status = db.load_circuit(path);

        REQUIRE(load_status == true);
        REQUIRE(db.get_chip_width()  == 23226);
        REQUIRE(db.get_chip_height() == 23386);
        REQUIRE(db.get_number_of_free_modules() == 494716);
    }
}//end adaptec4

