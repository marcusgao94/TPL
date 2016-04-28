/*!
 * \file tpl_standard_net_model_test.cpp
 * \author Peng Fei
 * \brief Standard tpl net model unittest.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "tpl_db.h"
#include "tpl_standard_detail_placement.h"

using namespace std;
using namespace tpl;
SCENARIO("bigblue1", "[bigblue1]") {

    GIVEN("A circuit bigblue1") {
        string path(getenv("BENCHMARK"));
        path += "/ispd2005/bigblue1";

        TplDB::db().load_circuit(path);

        TplStandardDetailPlacement detailPlacement;
        vector<vector<TplModule*>> rows;
        WHEN("We do legalization") {
            rows = detailPlacement.legalization();

            THEN("We get the legalizatiion rows") {
                REQUIRE( rows.size() > 0);
            }
        }

        WHEN("we do detail placement") {
            detailPlacement.detailPlacement(rows);
            THEN("we get legal modules in every row") {
                REQUIRE(rows.size() > 0);
            }
        }
    }
}//end SCENARIO

