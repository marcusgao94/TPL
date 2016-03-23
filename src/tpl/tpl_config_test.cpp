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

        REQUIRE(load_status == true);
        REQUIRE(boost::get<int>(TplConfig::instance()["init_grid_size"]) == 100000);
        REQUIRE(boost::get<double>(TplConfig::instance()["r1"]) == 0.1);
        REQUIRE(boost::get<double>(TplConfig::instance()["r2"]) == 0.5);
        REQUIRE(boost::get<int>(TplConfig::instance()["mu"]) == 10);
    }
}