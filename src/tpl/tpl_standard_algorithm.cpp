#include <cassert>
#include <fstream>
#include <iterator>

#include "tpl_standard_algorithm.h"

namespace tpl {
    using namespace std;

    TplNetForceModelInterface::~TplNetForceModelInterface()
    {
    }

    TplMoveForceModelInterface::~TplMoveForceModelInterface()
    {
    }

    TplAlgorithmInterface::~TplAlgorithmInterface()
    {
        delete net_force_model;
        delete move_force_model;
    }

    ////Begin TplStandardNetForceModel////

    TplStandardNetForceModel::~TplStandardNetForceModel()
    {
    }

    void TplStandardNetForceModel::compute_net_force_target(vector<double> &x_target, vector<double> &y_target)
    {
        assert( x_target.size() == 0 );
        assert( y_target.size() == 0 );

        unsigned int num_free = pdb.modules.num_free();
        SpMat Cx(num_free, num_free);
        SpMat Cy(num_free, num_free);
        VectorXd dx(num_free);
        dx = VectorXd::Zero(num_free);
        VectorXd dy(num_free);
        dy = VectorXd::Zero(num_free);

        compute_net_force_matrix(Cx, dx, Cy, dy);

        compute_net_force_target(Cx, dx, x_target);
        compute_net_force_target(Cy, dy, y_target);
    }

    void TplStandardNetForceModel::compute_net_force_matrix(SpMat &Cx, VectorXd &dx, SpMat &Cy, VectorXd &dy)
    {
        NetWeight x_net_weight, y_net_weight;

        compute_net_weight(x_net_weight, y_net_weight);

        compute_net_force_matrix(x_net_weight, Dimension::X, Cx, dx);
        compute_net_force_matrix(y_net_weight, Dimension::Y, Cy, dy);
    }

    void TplStandardNetForceModel::compute_net_weight(NetWeight &x_net_weight, NetWeight &y_net_weight)
    {
        //define static data
        static vector<TplPin*> pins;
        static vector<double> xs, ys;
        static double xmin=pdb.modules.chip_width(), xmax=-1, ymin=pdb.modules.chip_height(), ymax=-1;
        static size_t xmin_idx, xmax_idx, ymin_idx, ymax_idx;

        size_t i;
        double curx, cury;
        for(TplNets::net_iterator nit=pdb.nets.net_begin(); nit!=pdb.nets.net_end(); ++nit) {
            i = 0;
            curx = 0;
            cury = 0;

            //initialize static data
            for(TplNets::pin_iterator pit=pdb.nets.pin_begin(nit); pit!=pdb.nets.pin_end(nit); ++pit) {
                TplModule module =  pdb.modules.module(pit->id);
                pins.push_back( &(*pit) );

                curx = module.x + pit->dx;
                if       (curx < xmin) {
                    xmin = curx;
                    xmin_idx = i;
                } else if(curx > xmax) {
                    xmax = curx;
                    xmax_idx = i;
                }
                xs.push_back(curx);

                cury = module.y + pit->dy;
                if       (cury < ymin) {
                    ymin = cury;
                    ymin_idx = i;
                } else if(cury > ymax) {
                    ymax = cury;
                    ymax_idx = i;
                }
                ys.push_back(cury);

                ++i;
            }

            //call private routine
            compute_net_weight(pins, xs, xmin, xmax, xmin_idx, xmax_idx, x_net_weight);
            compute_net_weight(pins, ys, ymin, ymax, ymin_idx, ymax_idx, y_net_weight);

            //restore static data
            pins.clear();
            xs.clear();
            ys.clear();
            xmin=pdb.modules.chip_width();
            xmax=-1;
            ymin=pdb.modules.chip_height();
            ymax=-1;
        }
    }

    void TplStandardNetForceModel::compute_net_force_target(const SpMat &C, const VectorXd &d, std::vector<double> &target)
    {
        assert( target.size() == 0 );

        //solve linear problem using a LLT solver
        LLTSolver llt;
        VectorXd eigen_target = llt.compute(C).solve(d*-1);

        target.resize(eigen_target.size());
        VectorXd::Map(&target[0], eigen_target.size()) = eigen_target;
    }

    void TplStandardNetForceModel::compute_net_force_matrix(const NetWeight &net_weight, const Dimension dim, SpMat &C, VectorXd &d)
    {
        //prepare data for bound2bound model 
        TplPin *pin1, *pin2;
        string id1, id2;
        size_t idx1, idx2;
        double weight;
        map<pair<size_t, size_t>, double> nw;
        vector<SpElem> coefficients;

        for(NetWeight::const_iterator it=net_weight.begin(); it!=net_weight.end(); ++it) {
            pin1 = it->first.first;
            pin2 = it->first.second;
            id1 = pin1->id;
            id2 = pin2->id;
            idx1    = pdb.modules.module_index(id1);
            idx2    = pdb.modules.module_index(id2);
            weight = it->second;

            if(!pdb.modules.is_module_fixed(id1) && !pdb.modules.is_module_fixed(id2)) {
                nw[make_pair(idx1, idx1)] += weight;
                nw[make_pair(idx2, idx2)] += weight;
                nw[make_pair(idx1, idx2)] -= weight;
                nw[make_pair(idx2, idx1)] -= weight;

                switch(dim) {
                    case Dimension::X:
                        d(idx1) += weight*(pin1->dx - pin2->dx);
                        d(idx2) += weight*(pin2->dx - pin1->dx);
                        break;
                    case Dimension::Y:
                        d(idx1) += weight*(pin1->dy - pin2->dy);
                        d(idx2) += weight*(pin2->dy - pin1->dy);
                        break;
                }
            } else if(!pdb.modules.is_module_fixed(id1) && pdb.modules.is_module_fixed(id2)) {
                nw[make_pair(idx1, idx1)] += weight;

                switch(dim) {
                    case Dimension::X:
                        d(idx1) += weight*(pin1->dx - pin2->dx - pdb.modules[idx2].x);
                        break;
                    case Dimension::Y:
                        d(idx1) += weight*(pin1->dy - pin2->dy - pdb.modules[idx2].y);
                        break;
                }
            } else if(pdb.modules.is_module_fixed(id1) && !pdb.modules.is_module_fixed(id2)) {
                nw[make_pair(idx2, idx2)] += weight;

                switch(dim) {
                    case Dimension::X:
                        d(idx2) += weight*(pin2->dx - pin1->dx - pdb.modules[idx1].x);
                        break;
                    case Dimension::Y:
                        d(idx2) += weight*(pin2->dy - pin1->dy - pdb.modules[idx1].y);
                        break;
                }
            } else {
                continue;
            }
        }

        for(map<pair<size_t, size_t>, double>::iterator it=nw.begin(); it!=nw.end(); ++it) {
            coefficients.push_back( SpElem(it->first.first, it->first.second, it->second) );
        }
        C.setFromTriplets(coefficients.begin(), coefficients.end());
    }

    void TplStandardNetForceModel::compute_net_weight(const std::vector<TplPin*> &pins, const std::vector<double> &coordinates,
                                                      const double &min, const double &max, const size_t &min_idx, const size_t &max_idx, NetWeight &net_weight)
    {
        assert( pins.size() == coordinates.size() );

        const size_t &degree = pins.size();
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

            assert( net_weight.count( make_pair(pins[cur_idx], pins[min_idx]) ) == 0 );
            assert( net_weight.count( make_pair(pins[cur_idx], pins[max_idx]) ) == 0 );

            net_weight[make_pair(pins[cur_idx], pins[min_idx])] = min_weight;
            net_weight[make_pair(pins[cur_idx], pins[max_idx])] = max_weight;
        }

        assert( net_weight.count(make_pair(pins[min_idx], pins[max_idx]) ) == 0 );

        double min_max_weight = 2.0/(degree-1)*(max-min);
        net_weight[make_pair(pins[min_idx], pins[max_idx])] = min_max_weight;
    }

    ////End TplStandardNetForceModel////


    ////Begin TplStandardMoveForceModel////

    TplStandardMoveForceModel::~TplStandardMoveForceModel()
    {
    }

    TplStandardMoveForceModel::TplStandardMoveForceModel(double r1, double r2, unsigned int grid_size) :
            _r1(r1), _r2(r2), _grid_size(grid_size), _bin_width(0), _bin_height(0)
    {
        _bin_width  = pdb.modules.chip_width()  / _grid_size;
        _bin_height = pdb.modules.chip_height() / _grid_size;
    }

    void TplStandardMoveForceModel::initialize_move_force_matrix(SpMat &Cx0, SpMat &Cy0)
    {

    }

    void TplStandardMoveForceModel::compute_move_force_matrix(SpMat &Cx0, SpMat &Cy0)
    {

    }

    void TplStandardMoveForceModel::compute_heat_flux(VectorXd &x_heat_flux, VectorXd &y_heat_flux)
    {

    }

    double TplStandardMoveForceModel::green_function(const std::pair<int, int> &idx, const std::pair<int, int> &idx0) const
    {
#ifndef NDEBUG
        int gsize(_grid_size);
#endif
        assert(0 <= idx.first  && idx.first  < gsize);
        assert(0 <= idx.second && idx.second < gsize);
        assert(-gsize <= idx0.first  && idx0.first  < 2*gsize);
        assert(-gsize <= idx0.second && idx0.second < 2*gsize);

        return _green_function.at(make_pair(idx, idx0));
    }

    double TplStandardMoveForceModel::power_density(int i, int j) const
    {
        int gsize(_grid_size);

        assert(-gsize <= i && i < 2*gsize);
        assert(-gsize <= j && j < 2*gsize);

        int x_idx=i;
        if(i<0) x_idx = -i + 1;
        else if(i>=gsize) x_idx = -i + 2*gsize - 1;

        int y_idx=j;
        if(j<0) y_idx = -i + 1;
        else if(j>=gsize) y_idx = -i + 2*gsize - 1;

        return _power_density.at(x_idx).at(y_idx);
    }

    void TplStandardMoveForceModel::update_green_function(unsigned int grid_size)
    {
        _grid_size = grid_size;

        _bin_width  = pdb.modules.chip_width()  / _grid_size;
        _bin_height = pdb.modules.chip_height() / _grid_size;

        _green_function.clear();
        double distance = 0;
        int gsize(grid_size);
        for(int i=0; i<gsize; ++i) {
            for(int j=0; j<gsize; ++j) {
                for(int i0=-gsize; i0<2*gsize; ++i0) {
                    for(int j0=-gsize; j0<2*gsize; ++j0) {
                        if(i==i0 && j==j0) continue;
                        pair<int, int> idx  = make_pair(i, j);
                        pair<int, int> idx0 = make_pair(i0, j0);

                        distance = sqrt(pow(abs(i-i0)*_bin_width, 2) + pow(abs(j-j0)*_bin_height, 2));

                        if(distance <= _r1) {
                            _green_function[make_pair(idx, idx0)] = 1/distance;
                        } else if( distance > _r2) {
                            _green_function[make_pair(idx, idx0)] = 0;
                        } else {
                            _green_function[make_pair(idx, idx0)] = 0.6/sqrt(distance);
                        }
                    }
                }
            }
        }
    }

    void TplStandardMoveForceModel::update_power_density()
    {
        _power_density.clear();
        _power_density.reserve(_grid_size);
        for(size_t i=0; i<_power_density.size(); ++i) {
            _power_density[i].reserve(_grid_size);
        }

        unsigned int density_grid_size = _grid_size * 10;
        double x_step = _bin_width  / 10;
        double y_step = _bin_height / 10;

        vector<vector<double> > power_density_grid(density_grid_size, vector<double>(density_grid_size, 0));

        double x_orig = x_step / 2.0;
        double y_orig = y_step / 2.0;
        double x_cord = 0;
        double y_cord = 0;
        for(size_t i=0; i<_grid_size; ++i) {
            for(size_t j=0; j<_grid_size; ++j) {
                x_cord = x_orig + i*x_step;
                y_cord = y_orig + j*y_step;

                for(TplModules::iterator it=pdb.modules.begin(); it!=pdb.modules.end(); ++it) {
                    const TplModule &m = *it;
                    if( m.x-m.width  <= x_cord && x_cord <= m.x+m.width &&
                        m.y-m.height <= y_cord && y_cord <= m.y+m.height) {
                        power_density_grid[i][j] += m.power_density;
                    }
                }
            }
        }

        for(size_t i=0; i<_grid_size; ++i) {
            for(size_t j=0; j<_grid_size; ++j) {
                for(size_t x_idx = i*10; x_idx<i*10+10; ++x_idx) {
                    for(size_t y_idx = j*10; y_idx<j*10+10; ++y_idx) {
                        _power_density[i][j] += power_density_grid[x_idx][y_idx];
                    }
                }
            }
        }
    }

    ////End TplStandardMoveForceModel////


    ////Begin TplStandardAlgorithm////

    TplStandardAlgorithm::~TplStandardAlgorithm()
    {
    }

    void TplStandardAlgorithm::initialize_models()
    {
        net_force_model  = new TplStandardNetForceModel;
        move_force_model = new TplStandardMoveForceModel(0.1, 0.2, 100000);
    }

    void TplStandardAlgorithm::make_initial_placement()
    {
        vector<double> x_target, y_target;

        for(size_t i=0; i<5; ++i) {
            net_force_model->compute_net_force_target(x_target, y_target);
            pdb.modules.set_free_module_coordinates(x_target, y_target);

            x_target.clear();
            y_target.clear();
        }
    }

    ////End TplStandardAlgorithm////

}//end namespace tpl

