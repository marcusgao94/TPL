/*!
 * \file tpl_standard_net_model_test.cpp
 * \author Peng Fei
 * \brief Standard tpl net model unittest.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "tpl_db.h"
#include "tpl_config.h"
#include "tpl_standard_thermal_model.h"

using namespace std;
using namespace tpl;

SCENARIO("bigblue1", "[bigblue1]") {

    GIVEN("A circuit bigblue1") {
        string path(getenv("BENCHMARK"));
        path += "/ispd2005/bigblue1";

        TplDB::db().load_circuit(path);
        TplConfig::instance().load_configuration(getenv("TPLCONFIG"));

        TplStandardThermalModel tmodel;

        WHEN("We compute the chip's thermal distribution") {
            std::shared_ptr<TMat> tss;
            tmodel.compute_thermal_distribution(tss);

            THEN("We get the current thermal distribution") {
                int gsize = tmodel.grid_size();

                REQUIRE( tss->get_width()  == gsize+3);
                REQUIRE( tss->get_height() == gsize+3);
            }
        }
    }
}//end SCENARIO

