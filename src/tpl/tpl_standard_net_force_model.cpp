#include "tpl_standard_net_force_model.h"

#ifndef NDEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif


namespace tpl {

    using std::string;
    using std::vector;
    using std::map;
    using std::pair;
    using std::make_pair;

    void TplStandardNetForceModel::compute_net_force_matrix(const NetWeight &NWx, const NetWeight &NWy,
                                                            SpMat &Cx, SpMat &Cy, VectorXd &dx, VectorXd &dy)
    {
        //preconditions
        assert(Cx.cols() == static_cast<int>(TplDB::db().modules.num_free()));
        assert(Cx.rows() == static_cast<int>(TplDB::db().modules.num_free()));
        assert(Cy.cols() == static_cast<int>(TplDB::db().modules.num_free()));
        assert(Cy.rows() == static_cast<int>(TplDB::db().modules.num_free()));
        assert(dx.rows() == static_cast<int>(TplDB::db().modules.num_free()));
        assert(dy.rows() == static_cast<int>(TplDB::db().modules.num_free()));

        Cx.setZero();
        Cy.setZero();
        dx.setZero();
        dy.setZero();

        //prepare data
        TplPin *pin1, *pin2;
        string id1, id2;
        size_t idx1, idx2;
        double weight;
        map<pair<size_t, size_t>, double> nw;
        vector<SpElem> coefficients;

        //compute Cx and dx
        for(NetWeight::const_iterator it=NWx.begin(); it!=NWx.end(); ++it) {
            pin1 = it->first.first;
            pin2 = it->first.second;
            id1 = pin1->id;
            id2 = pin2->id;
            idx1    = TplDB::db().modules.module_index(id1);
            idx2    = TplDB::db().modules.module_index(id2);
            weight = it->second;

            if(!TplDB::db().modules.is_module_fixed(id1) && !TplDB::db().modules.is_module_fixed(id2)) {
                nw[make_pair(idx1, idx1)] += weight;
                nw[make_pair(idx2, idx2)] += weight;
                nw[make_pair(idx1, idx2)] -= weight;
                nw[make_pair(idx2, idx1)] -= weight;

                dx(idx1) += weight*(pin1->dx - pin2->dx);
                dx(idx2) += weight*(pin2->dx - pin1->dx);
            } else if(!TplDB::db().modules.is_module_fixed(id1) && TplDB::db().modules.is_module_fixed(id2)) {
                nw[make_pair(idx1, idx1)] += weight;

                dx(idx1) += weight*(pin1->dx - pin2->dx - TplDB::db().modules[idx2].x);
            } else if(TplDB::db().modules.is_module_fixed(id1) && !TplDB::db().modules.is_module_fixed(id2)) {
                nw[make_pair(idx2, idx2)] += weight;

                dx(idx2) += weight*(pin2->dx - pin1->dx - TplDB::db().modules[idx1].x);
            } else {
                continue;
            }
        }

        for(map<pair<size_t, size_t>, double>::iterator it=nw.begin(); it!=nw.end(); ++it) {
            coefficients.push_back( SpElem(it->first.first, it->first.second, it->second) );
        }
        Cx.setFromTriplets(coefficients.begin(), coefficients.end());

        //clear temp container
        nw.clear();
        coefficients.clear();

        //compute Cy and dy
        for(NetWeight::const_iterator it=NWy.begin(); it!=NWy.end(); ++it) {
            pin1 = it->first.first;
            pin2 = it->first.second;
            id1 = pin1->id;
            id2 = pin2->id;
            idx1    = TplDB::db().modules.module_index(id1);
            idx2    = TplDB::db().modules.module_index(id2);
            weight = it->second;

            if(!TplDB::db().modules.is_module_fixed(id1) && !TplDB::db().modules.is_module_fixed(id2)) {
                nw[make_pair(idx1, idx1)] += weight;
                nw[make_pair(idx2, idx2)] += weight;
                nw[make_pair(idx1, idx2)] -= weight;
                nw[make_pair(idx2, idx1)] -= weight;

                dy(idx1) += weight*(pin1->dy - pin2->dy);
                dy(idx2) += weight*(pin2->dy - pin1->dy);
            } else if(!TplDB::db().modules.is_module_fixed(id1) && TplDB::db().modules.is_module_fixed(id2)) {
                nw[make_pair(idx1, idx1)] += weight;

                dy(idx1) += weight*(pin1->dy - pin2->dy - TplDB::db().modules[idx2].y);
            } else if(TplDB::db().modules.is_module_fixed(id1) && !TplDB::db().modules.is_module_fixed(id2)) {
                nw[make_pair(idx2, idx2)] += weight;

                dy(idx2) += weight*(pin2->dy - pin1->dy - TplDB::db().modules[idx1].y);
            } else {
                continue;
            }
        }

        for(map<pair<size_t, size_t>, double>::iterator it=nw.begin(); it!=nw.end(); ++it) {
            coefficients.push_back( SpElem(it->first.first, it->first.second, it->second) );
        }
        Cy.setFromTriplets(coefficients.begin(), coefficients.end());
    }


    void TplStandardNetForceModel::compute_net_force_target(const NetWeight &NWx, const NetWeight &NWy,
                                                            std::vector<double> &x_target, std::vector<double> &y_target)
    {
        //preconditions
        x_target.clear();
        y_target.clear();

        unsigned int num_free = TplDB::db().modules.num_free();
        x_target.resize(num_free);
        y_target.resize(num_free);

        SpMat Cx(num_free, num_free), Cy(num_free, num_free);
        Cx.setZero();
        Cy.setZero();

        VectorXd dx(num_free), dy(num_free);
        dx.setZero();
        dy.setZero();

        compute_net_force_matrix(NWx, NWy, Cx, Cy, dx, dy);

        LLTSolver llt;

        VectorXd x_eigen_target = llt.compute(Cx).solve(dx*-1);
        VectorXd y_eigen_target = llt.compute(Cy).solve(dy*-1);

        assert(static_cast<long>(x_target.size()) == x_eigen_target.size() );
        assert(static_cast<long>(y_target.size()) == y_eigen_target.size() );

        VectorXd::Map(&x_target[0], x_eigen_target.size()) = x_eigen_target;
        VectorXd::Map(&y_target[0], y_eigen_target.size()) = y_eigen_target;
    }

}//namespace tpl
