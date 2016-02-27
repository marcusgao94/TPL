/*!
 * \file tpl_gy_algorithm_test.cpp
 * \author Gao Yue 
 * \brief Refined tpl algorithm unittest.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>

#include "tpl_gy_algorithm.h"

using namespace std;
using namespace tpl;

SCENARIO("adaptec1", "[adaptec1]") {

    GIVEN("A circuit adaptec1") {
        char path[200];
        char *benchmark;
        benchmark = std::getenv("BENCHMARK");
        std::strcpy(path, benchmark);
        std::strcat(path, "/ispd2005/bigblue1");

        pdb.load_circuit(path);

        TplNetForceModelInterface *net_force_model = new TplGYNetForceModel;

        TplAlgorithmInterface *alg = new TplGYAlgorithm;

        WHEN("we compute the first net's weight") {
            NetWeight x_net_weight, y_net_weight;
            net_force_model->compute_net_weight(x_net_weight, y_net_weight);

            THEN("the net weight is not empty") {
                REQUIRE( x_net_weight.size() != 0);
                REQUIRE( y_net_weight.size() != 0);
            }
        }

        WHEN("we compute net force target") {
            vector<double> x_target, y_target;
            net_force_model->compute_net_force_target(x_target, y_target);

            THEN("the targets are not empty") {
                REQUIRE( x_target.size() != 0);
                REQUIRE( y_target.size() != 0);
            }
        }

        WHEN("we make the initial placement") {
            alg->make_initial_placement();

            THEN("the free modules' coordinates will be chaned") {
                pdb.generate_placement_snapshot();
            }
        }

        delete net_force_model;
        delete alg;
    }

}//end SCENARIO

