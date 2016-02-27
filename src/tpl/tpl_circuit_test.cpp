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

SCENARIO("adaptec1", "[adaptec1]") {

    GIVEN("A circuit adaptec1") {
        char path[200];
        char *benchmark;
        benchmark = std::getenv("BENCHMARK");
        std::strcpy(path, benchmark);
        std::strcat(path, "/ispd2005/adaptec1");

        bool load_status = pdb.load_circuit(path);

        REQUIRE(load_status == true);
        REQUIRE(pdb.modules.chip_width()  == 11589);
        REQUIRE(pdb.modules.chip_height() == 11589);
        REQUIRE(pdb.modules.num_free()    == 210904);
    }
}//end adaptec1

SCENARIO("adaptec2", "[adaptec2]") {

    GIVEN("A circuit adaptec2") {
        char path[200];
        char *benchmark;
        benchmark = std::getenv("BENCHMARK");
        std::strcpy(path, benchmark);
        std::strcat(path, "/ispd2005/adaptec2");

        bool load_status = pdb.load_circuit(path);

        REQUIRE(load_status == true);
        REQUIRE(pdb.modules.chip_width()  == 15244);
        REQUIRE(pdb.modules.chip_height() == 15244);
        REQUIRE(pdb.modules.num_free()    == 254457);
    }
}//end adaptec2

SCENARIO("adaptec3", "[adaptec3]") {

    GIVEN("A circuit adaptec3") {
        char path[200];
        char *benchmark;
        benchmark = std::getenv("BENCHMARK");
        std::strcpy(path, benchmark);
        std::strcat(path, "/ispd2005/adaptec3");

        bool load_status = pdb.load_circuit(path);

        REQUIRE(load_status == true);
        REQUIRE(pdb.modules.chip_width()  == 23190);
        REQUIRE(pdb.modules.chip_height() == 23386);
        REQUIRE(pdb.modules.num_free()    == 450927);
    }
}//end adaptec3

SCENARIO("adaptec4", "[adaptec4]") {

    GIVEN("Test case adaptec4") {
        char path[200];
        char *benchmark;
        benchmark = std::getenv("BENCHMARK");
        std::strcpy(path, benchmark);
        std::strcat(path, "/ispd2005/adaptec4");

        bool load_status = pdb.load_circuit(path);

        REQUIRE(load_status == true);
        REQUIRE(pdb.modules.chip_width()  == 23226);
        REQUIRE(pdb.modules.chip_height() == 23386);
        REQUIRE(pdb.modules.num_free()    == 494716);
    }
}//end adaptec4

