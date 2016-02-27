/*!
 * \file tpl_algorithm_test.cpp
 * \author Peng Fei
 * \brief tpl algorithm unittest.
 */

#define CATCH_CONFIG_MAIN
#include "../tpl/catch.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>

#include "tpl_algorithm_pf.h"

using namespace std;
using namespace tpl;

SCENARIO("adaptec1", "[adaptec1]") {

    GIVEN("A circuit adaptec1") {
        char path[200];
        char *home;
        home = std::getenv("HOME");
        std::strcpy(path, home);
        std::strcat(path, "/Workspace/TPL/benchmark/ispd2005/bigblue1");

        pdb.load_circuit(path);

        TplAlgorithmPF alg;

        WHEN("we compute the first net's weight") {
            TplNets::net_iterator nit = pdb.nets.net_begin();

            NetWeight x_net_weight, y_net_weight;
            alg.compute_net_weight(nit, x_net_weight, y_net_weight);

            THEN("the net weight is not empty") {
                REQUIRE( x_net_weight.size() != 0);
                REQUIRE( y_net_weight.size() != 0);
            }
        }

        WHEN("we compute net force target") {
            vector<double> x_target, y_target;
            alg.compute_net_force_target(x_target, y_target);

            THEN("the targets are not empty") {
                REQUIRE( x_target.size() != 0);
                REQUIRE( y_target.size() != 0);
            }
        }

        WHEN("we make the initial placement") {
            alg.make_initial_placement();

            THEN("the free modules' coordinates will be chaned") {
                pdb.generate_placement_snapshot();
            }
        }
    }
}//end SCENARIO

