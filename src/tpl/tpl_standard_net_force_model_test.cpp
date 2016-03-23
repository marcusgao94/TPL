/*!
 * \file tpl_standard_net_force_model_test.cpp
 * \author Peng Fei
 * \brief Standard tpl net force model unittest.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "tpl_db.h"
#include "tpl_standard_net_model.h"
#include "tpl_standard_net_force_model.h"

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
        TplStandardNetForceModel nfmodel;

        WHEN("we compute the first net's weight") {
            NetWeight x_net_weight, y_net_weight;
            nmodel.compute_net_weight(x_net_weight, y_net_weight);

            int degree = TplDB::db().modules.num_free();
            SpMat Cx(degree, degree), Cy(degree, degree);
            VectorXd dx = VectorXd::Zero(degree), dy = VectorXd::Zero(degree);

            nfmodel.compute_net_force_matrix(x_net_weight, y_net_weight, Cx, Cy, dx, dy);

            THEN("the net weight is not empty") {
                cout << Cx << endl;
            }
        }
    }
}//end SCENARIO

/*
SCENARIO("bigblue2", "[bigblue2]") {

    GIVEN("A circuit bigblue2") {
        char path[200];
        char *benchmark;
        benchmark = std::getenv("BENCHMARK");
        std::strcpy(path, benchmark);
        std::strcat(path, "/ispd2005/bigblue2");

        TplDB::db().load_circuit(path);

        TplStandardNetModel nmodel;
        TplStandardNetForceModel nfmodel;

        WHEN("we compute the first net's weight") {
            NetWeight x_net_weight, y_net_weight;
            nmodel.compute_net_weight(x_net_weight, y_net_weight);

            int degree = TplDB::db().modules.num_free();
            SpMat Cx(degree, degree), Cy(degree, degree);
            VectorXd dx = VectorXd::Zero(degree), dy = VectorXd::Zero(degree);

            nfmodel.compute_net_force_matrix(x_net_weight, y_net_weight, Cx, Cy, dx, dy);

            THEN("the net weight is not empty") {
                cout << Cx << endl;
            }
        }
    }
}//end SCENARIO
 */