/*!
 * \file tpl_standard_net_model_test.cpp
 * \author Peng Fei
 * \brief Standard tpl net model unittest.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "utils.h"
#include "tpl_db.h"
#include "tpl_standard_thermal_force_model.h"

using namespace std;
using namespace tpl;

SCENARIO("adaptec1", "[adaptec1]") {

    GIVEN("A circuit adaptec1") {
        string path(getenv("BENCHMARK"));
        path += "/ispd2005/adaptec1";

        TplDB::db().load_circuit(path);

        TplStandardThermalForceModel tfmodel;

        unsigned int num_free = TplDB::db().modules.num_free();
        VectorXd xhf(num_free), yhf(num_free);
        xhf.setZero();
        yhf.setZero();

        WHEN("We compute the chip's thermal distribution") {
             tfmodel.compute_heat_flux_vector(xhf, yhf);
        }
    }
}//end SCENARIO
