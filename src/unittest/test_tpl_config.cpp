/*!
 * \file tpl_config_test.cpp
 * \author Peng Fei
 * \brief tpl configuration system test.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "tpl_config.h"

using namespace tpl;

SCENARIO("load configuration file", "[config]") {

    GIVEN("A configuration file") {
        const char *config = std::getenv("TPLCONFIG");

        bool load_status = TplConfig::instance().load_configuration(config);

        WHEN("We load the configuration file") {

            THEN("We can get the algorithm running parameters") {

                REQUIRE(load_status == true);
                REQUIRE(TplConfig::instance().init_grid_size == 100000);
                REQUIRE(TplConfig::instance().r1 == 0.1);
                REQUIRE(TplConfig::instance().r2 == 0.5);
                REQUIRE(TplConfig::instance().mu == 10);
            }
        }
    }
}