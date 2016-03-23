/*!
 * \file tpl_standard_net_model_test.cpp
 * \author Peng Fei
 * \brief Standard tpl net model unittest.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "tpl_db.h"
#include "tpl_standard_net_model.h"

using namespace std;
using namespace tpl;

SCENARIO("bigblue1", "[bigblue1]") {

    GIVEN("A circuit bigblue1") {
        char path[200];
        char *benchmark;
        benchmark = std::getenv("BENCHMARK");
        std::strcpy(path, benchmark);
        std::strcat(path, "/ispd2005/bigblue1");

        TplDB::db().load_circuit(path);

        TplStandardNetModel nmodel;

        WHEN("we compute the first net's weight") {
            NetWeight x_net_weight, y_net_weight;
            nmodel.compute_net_weight(x_net_weight, y_net_weight);

            THEN("the net weight is not empty") {
                REQUIRE( x_net_weight.size() != 0);
                REQUIRE( y_net_weight.size() != 0);
            }
        }
    }
}//end SCENARIO

SCENARIO("bigblue2", "[bigblue2]") {

    GIVEN("A circuit bigblue2") {
        char path[200];
        char *benchmark;
        benchmark = std::getenv("BENCHMARK");
        std::strcpy(path, benchmark);
        std::strcat(path, "/ispd2005/bigblue2");

        TplDB::db().load_circuit(path);

        TplStandardNetModel nmodel;

        WHEN("we compute the first net's weight") {
            NetWeight x_net_weight, y_net_weight;
            nmodel.compute_net_weight(x_net_weight, y_net_weight);

            THEN("the net weight is not empty") {
                REQUIRE( x_net_weight.size() != 0);
                REQUIRE( y_net_weight.size() != 0);
            }
        }
    }
}//end SCENARIO

SCENARIO("bigblue3", "[bigblue3]") {

    GIVEN("A circuit bigblue3") {
        char path[200];
        char *benchmark;
        benchmark = std::getenv("BENCHMARK");
        std::strcpy(path, benchmark);
        std::strcat(path, "/ispd2005/bigblue3");

        TplDB::db().load_circuit(path);

        TplStandardNetModel nmodel;

        WHEN("we compute the first net's weight") {
            NetWeight x_net_weight, y_net_weight;
            nmodel.compute_net_weight(x_net_weight, y_net_weight);

            THEN("the net weight is not empty") {
                REQUIRE( x_net_weight.size() != 0);
                REQUIRE( y_net_weight.size() != 0);
            }
        }
    }
}//end SCENARIO

SCENARIO("bigblue4", "[bigblue4]") {

    GIVEN("A circuit bigblue4") {
        char path[200];
        char *benchmark;
        benchmark = std::getenv("BENCHMARK");
        std::strcpy(path, benchmark);
        std::strcat(path, "/ispd2005/bigblue4");

        TplDB::db().load_circuit(path);

        TplStandardNetModel nmodel;

        WHEN("we compute the first net's weight") {
            NetWeight x_net_weight, y_net_weight;
            nmodel.compute_net_weight(x_net_weight, y_net_weight);

            THEN("the net weight is not empty") {
                REQUIRE( x_net_weight.size() != 0);
                REQUIRE( y_net_weight.size() != 0);
            }
        }
    }
}//end SCENARIO