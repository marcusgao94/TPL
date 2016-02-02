#include "tpl_algorithm.h"

#include "tpl_circuit.h"

#include <cassert>
#include <fstream>
#include <iterator>
#include <utility>

#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>


namespace tpl {
    using namespace std;

    typedef Eigen::SparseMatrix<double> SpMat;
    typedef Eigen::Triplet<double>     SpElem;
    typedef Eigen::ConjugateGradient<SpMat> CGSolver;
    typedef Eigen::SimplicialLLT<SpMat> LLTSolver;
    using Eigen::VectorXd;

    TplAlgorithm::TplAlgorithm()
    {
    }

    void TplAlgorithm::make_initial_placement()
    {
        vector<double> x_target, y_target;

        for(size_t i=0; i<10; ++i) {
            compute_net_force_target(x_target, y_target);
            db.update_free_module_position(x_target, y_target);

            x_target.clear();
            y_target.clear();
        }

        db.generate_placement_snapshot();
    }//end TplAlgorithm::initial_placement
    
    void TplAlgorithm::compute_net_force_target(vector<double> &x_target, vector<double> &y_target)
    {
#ifndef NDEBUG
        assert( x_target.size() == 0 );
        assert( y_target.size() == 0 );
#endif

        //compute net weight for each net
        NetWeight x_net_weight, y_net_weight;
        for(TplDB::net_iterator nit=db.net_begin(); nit!=db.net_end(); ++nit) {
            compute_net_weight(nit, x_net_weight, y_net_weight);
        }

        compute_net_force_target(x_net_weight, x_target);
        compute_net_force_target(y_net_weight, y_target);
    }//end TplAlgorithm::compute_net_force_target


    void TplAlgorithm::compute_net_weight(const TplDB::net_iterator &nit, NetWeight &x_net_weight, NetWeight &y_net_weight)
    {
        //define static data
        static vector<string> ids;
        static vector<double> xs, ys;
        static double xmin=db.get_chip_width(), xmax=-1, ymin=db.get_chip_height(), ymax=-1;
        static size_t xmin_idx, xmax_idx, ymin_idx, ymax_idx;

        //initilize static data
        size_t i=0;
        for(TplDB::pin_iterator pit=db.pin_begin(nit); pit!=db.pin_end(nit); ++pit) {
            TplModule module =  db.get_module(pit->id);
            ids.push_back( pit->id );

            if       (module.x < xmin) {
                xmin = module.x;
                xmin_idx = i;
            } else if(module.x > xmax) {
                xmax = module.x;
                xmax_idx = i;
            }
            xs.push_back(module.x);

            if       (module.y < ymin) {
                ymin = module.y;
                ymin_idx = i;
            } else if(module.y > ymax) {
                ymax = module.y;
                ymax_idx = i;
            }
            ys.push_back(module.y);

            ++i; 
        }

        //call private routine
        compute_net_weight(ids, xs, xmin, xmax, xmin_idx, xmax_idx, x_net_weight);
        compute_net_weight(ids, ys, ymin, ymax, ymin_idx, ymax_idx, y_net_weight);

        //restore static data
        ids.clear();
        xs.clear();
        ys.clear();
        xmin=db.get_chip_width(); 
        xmax=-1; 
        ymin=db.get_chip_height(); 
        ymax=-1;
    }//end TplAlgorithm::compute_net_weight


    //private routines
    void TplAlgorithm::compute_net_force_target(const NetWeight &net_weight, std::vector<double> &target)
    {
#ifndef NDEBUG
        assert( target.size() == 0 );
#endif

        //define matrix and vector
        unsigned int num_free = db.get_number_of_free_modules();
        SpMat C(num_free, num_free);
        VectorXd d(num_free); 
        d = VectorXd::Zero(num_free);

        //prepare data for bound2bound model 
        string id1, id2;
        size_t idx1, idx2;
        double weight;
        map<pair<size_t, size_t>, double> nw;
        vector<SpElem> coefficients;

        for(NetWeight::const_iterator it=net_weight.begin(); it!=net_weight.end(); ++it) {
            id1 = it->first.first;
            id2 = it->first.second;
            idx1    = db.get_module_index(id1);
            idx2    = db.get_module_index(id2);
            weight = it->second;

            if(db.is_module_fixed(id1) && db.is_module_fixed(id2)) {
                continue;
            } else if(db.is_module_fixed(id1)) {
                nw[make_pair(idx2, idx2)] += weight;
                d(idx2)       -= weight;
            } else if(db.is_module_fixed(id2)) {
                nw[make_pair(idx1, idx1)] += weight;
                d(idx1)       -= weight;
            } else {
                nw[make_pair(idx1, idx1)] += weight;
                nw[make_pair(idx2, idx2)] += weight;
                nw[make_pair(idx1, idx2)] -= weight;
                nw[make_pair(idx2, idx1)] -= weight;
            }
        }
        for(map<pair<size_t, size_t>, double>::iterator it=nw.begin(); it!=nw.end(); ++it) {
            coefficients.push_back( SpElem(it->first.first, it->first.second, it->second) );
        }
        C.setFromTriplets(coefficients.begin(), coefficients.end());
        d *= -1;

        //solve linear problem using a LLT solver
        LLTSolver llt;
        VectorXd eigen_target = llt.compute(C).solve(d);

        target.resize(eigen_target.size());
        VectorXd::Map(&target[0], eigen_target.size()) = eigen_target;
    }//end TplAlgorithm::compute_net_force_target

    void TplAlgorithm::compute_net_weight(const std::vector<std::string> &ids, const std::vector<double> &coordinates,
            const double &min, const double &max, const size_t &min_idx, const size_t &max_idx, NetWeight &net_weight)
    {
#ifndef NDEBUG
        assert( ids.size() == coordinates.size() );
#endif

        const size_t &degree = ids.size();
        double min_weight, max_weight;

        for(size_t cur_idx=0; cur_idx<degree; ++cur_idx) {
            if(cur_idx == min_idx || cur_idx == max_idx ) continue;

            if( coordinates[cur_idx] == min ) {
                min_weight = 0;
                max_weight = 2.0/(degree-1)*(max-min);
            } else if( coordinates[cur_idx] == max ) {
                min_weight = 2.0/(degree-1)*(max-min);
                max_weight = 0;
            } else {
                min_weight = 2.0/(degree-1)*(coordinates[cur_idx]-min);
                max_weight = 2.0/(degree-1)*(max-coordinates[cur_idx]);
            }

#ifndef NDEBUG
            //assert( net_weight.count(    make_pair(ids[cur_idx], ids[min_idx]) ) == 0 );
#endif
            net_weight[make_pair(ids[cur_idx], ids[min_idx])] += min_weight;
#ifndef NDEBUG
            //assert( net_weight.count(    make_pair(ids[cur_idx], ids[max_idx]) ) == 0 );
#endif
            net_weight[make_pair(ids[cur_idx], ids[max_idx])] += max_weight;
        }

        double min_max_weight = 2.0/(degree-1)*(max-min);
        net_weight[make_pair(ids[min_idx], ids[max_idx])] += min_max_weight;
    }//end TplAlgorithm::compute_net_weight

}//end namespace tpl

