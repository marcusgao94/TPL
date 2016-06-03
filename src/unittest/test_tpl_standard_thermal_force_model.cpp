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

#include <chrono>

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

        std::chrono::time_point<std::chrono::system_clock> start, end;
        std::chrono::duration<double> elapsed_seconds;

        WHEN("We compute the chip's thermal distribution") {
            cout << "begin generate power density " << endl;
            start = std::chrono::system_clock::now();

            tfmodel.generate_power_density();

            end = std::chrono::system_clock::now();
            elapsed_seconds = end-start;
            cout << "generate power density " << elapsed_seconds.count() << " seconds" << endl;

            cout << "begin generate thermal profile" << endl;
            start = std::chrono::system_clock::now();

            tfmodel.generate_thermal_profile();

            end = std::chrono::system_clock::now();
            elapsed_seconds = end-start;
            cout << "generate thermal profile " << elapsed_seconds.count() << " seconds" << endl;
        }
    }
}//end SCENARIO
