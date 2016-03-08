#include <cassert>
#include <cmath>
#include <fstream>
#include <iterator>
#include <algorithm>

#include "tpl_standard_algorithm.h"

namespace tpl {
    using namespace std;

    ////Begin TplStandardNetModel////

    TplNetModelInterface::~TplNetModelInterface()
    {
    }

    TplStandardNetModel::~TplStandardNetModel()
    {
    }

    void TplStandardNetModel::compute_net_weight(NetWeight &x_net_weight, NetWeight &y_net_weight)
    {
        //preconditions
        assert( x_net_weight.size() == 0);
        assert( y_net_weight.size() == 0);

        //define pin related data
        vector<TplPin*> pins = {};
        vector<double> xs = {};
        vector<double> ys = {};
        double xmin=pdb.modules.chip_width(), xmax=-1, ymin=pdb.modules.chip_height(), ymax=-1;
        size_t xmin_idx=-1, xmax_idx=-1, ymin_idx=-1, ymax_idx=-1;

        //local helper variables
        size_t idx;
        double curx, cury;

        for(TplNets::net_iterator nit=pdb.nets.net_begin(); nit!=pdb.nets.net_end(); ++nit) {
            idx = 0;
            curx = 0;
            cury = 0;

            //initialize current net's pin's related data
            for(TplNets::pin_iterator pit=pdb.nets.pin_begin(nit); pit!=pdb.nets.pin_end(nit); ++pit) {
                TplModule &module =  pdb.modules.module(pit->id);
                pins.push_back( &(*pit) );

                curx = module.x + pit->dx;
                if       (curx < xmin) {
                    xmin = curx;
                    xmin_idx = idx;
                } else if(curx > xmax) {
                    xmax = curx;
                    xmax_idx = idx;
                }
                xs.push_back(curx);

                cury = module.y + pit->dy;
                if       (cury < ymin) {
                    ymin = cury;
                    ymin_idx = idx;
                } else if(cury > ymax) {
                    ymax = cury;
                    ymax_idx = idx;
                }
                ys.push_back(cury);

                ++idx;
            }

            const size_t &degree = pins.size();
            //initialize current net's net weights to zero
            /*
            for(size_t i=0; i<degree; ++i) {
                for(size_t j=0; j<degree; ++j) {

                    assert( x_net_weight.count(make_pair(pins[i], pins[j])) == 0);
                    assert( y_net_weight.count(make_pair(pins[i], pins[j])) == 0);

                    x_net_weight[make_pair(pins[i], pins[j])] = 0;
                    y_net_weight[make_pair(pins[i], pins[j])] = 0;
                }
            }
             */

            //loop to compute current net's non zero net weights
            for(size_t cur_idx=0; cur_idx<degree; ++cur_idx)     {
                if( cur_idx == xmin_idx || cur_idx == xmax_idx ||
                    cur_idx == ymin_idx || cur_idx == ymax_idx ) {
                    continue;
                } else {
                    if (xs[cur_idx] == xmin) {
                        x_net_weight[make_pair(pins[cur_idx], pins[xmax_idx])] = 2.0 / (degree-1) * (xmax - xmin);
                        x_net_weight[make_pair(pins[xmax_idx], pins[cur_idx])] = 2.0 / (degree-1) * (xmax - xmin);

                    } else if (xs[cur_idx] == xmax) {
                        x_net_weight[make_pair(pins[cur_idx], pins[xmin_idx])] = 2.0 / (degree-1) * (xmax - xmin);
                        x_net_weight[make_pair(pins[xmin_idx], pins[cur_idx])] = 2.0 / (degree-1) * (xmax - xmin);
                    } else {
                        x_net_weight[make_pair(pins[cur_idx], pins[xmin_idx])] = 2.0 / (degree-1) * (xs[cur_idx] - xmin);
                        x_net_weight[make_pair(pins[xmin_idx], pins[cur_idx])] = 2.0 / (degree-1) * (xs[cur_idx] - xmin);
                        x_net_weight[make_pair(pins[cur_idx], pins[xmax_idx])] = 2.0 / (degree-1) * (xmax - xs[cur_idx]);
                        x_net_weight[make_pair(pins[xmax_idx], pins[cur_idx])] = 2.0 / (degree-1) * (xmax - xs[cur_idx]);
                    }

                    if (ys[cur_idx] == ymin) {
                        y_net_weight[make_pair(pins[cur_idx], pins[ymax_idx])] = 2.0 / (degree-1) * (ymax - ymin);
                        y_net_weight[make_pair(pins[ymax_idx], pins[cur_idx])] = 2.0 / (degree-1) * (ymax - ymin);
                    }
                    else if (ys[cur_idx] == ymax) {
                        y_net_weight[make_pair(pins[cur_idx], pins[ymin_idx])] = 2.0 / (degree-1) * (ymax - ymin);
                        y_net_weight[make_pair(pins[ymin_idx], pins[cur_idx])] = 2.0 / (degree-1) * (ymax - ymin);
                    } else {
                        y_net_weight[make_pair(pins[cur_idx], pins[ymin_idx])] = 2.0 / (degree-1) * (xs[cur_idx] - ymin);
                        y_net_weight[make_pair(pins[ymin_idx], pins[cur_idx])] = 2.0 / (degree-1) * (xs[cur_idx] - ymin);
                        y_net_weight[make_pair(pins[cur_idx], pins[ymax_idx])] = 2.0 / (degree-1) * (ymax - xs[cur_idx]);
                        y_net_weight[make_pair(pins[ymax_idx], pins[cur_idx])] = 2.0 / (degree-1) * (ymax - xs[cur_idx]);
                    }
                }
            }

            x_net_weight[make_pair(pins[xmin_idx], pins[xmax_idx])] = 2.0/(degree-1)*(xmax-xmin);
            x_net_weight[make_pair(pins[xmax_idx], pins[xmin_idx])] = 2.0/(degree-1)*(xmax-xmin);
            y_net_weight[make_pair(pins[ymin_idx], pins[ymax_idx])] = 2.0/(degree-1)*(ymax-ymin);
            y_net_weight[make_pair(pins[ymax_idx], pins[ymin_idx])] = 2.0/(degree-1)*(ymax-ymin);
        }
    }

    ////End TplStandardNetModel////


    ////Begin TplStandardNetForceModel////

    TplNetForceModelInterface::~TplNetForceModelInterface()
    {
    }

    TplStandardNetForceModel::~TplStandardNetForceModel()
    {
    }

    void TplStandardNetForceModel::compute_net_force_matrix(const NetWeight &NWx, const NetWeight &NWy,
                                                            SpMat &Cx, SpMat &Cy, VectorXd &dx, VectorXd &dy)
    {
        //preconditions
        assert(Cx.cols() == pdb.modules.num_free());
        assert(Cx.rows() == pdb.modules.num_free());
        assert(Cy.cols() == pdb.modules.num_free());
        assert(Cy.rows() == pdb.modules.num_free());
        assert(dx.size() == pdb.modules.num_free());
        assert(dy.size() == pdb.modules.num_free());

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
            idx1    = pdb.modules.module_index(id1);
            idx2    = pdb.modules.module_index(id2);
            weight = it->second;

            if(!pdb.modules.is_module_fixed(id1) && !pdb.modules.is_module_fixed(id2)) {
                nw[make_pair(idx1, idx1)] += weight;
                nw[make_pair(idx2, idx2)] += weight;
                nw[make_pair(idx1, idx2)] -= weight;
                nw[make_pair(idx2, idx1)] -= weight;

                dx(idx1) += weight*(pin1->dx - pin2->dx);
                dx(idx2) += weight*(pin2->dx - pin1->dx);
            } else if(!pdb.modules.is_module_fixed(id1) && pdb.modules.is_module_fixed(id2)) {
                nw[make_pair(idx1, idx1)] += weight;

                dx(idx1) += weight*(pin1->dx - pin2->dx - pdb.modules[idx2].x);
            } else if(pdb.modules.is_module_fixed(id1) && !pdb.modules.is_module_fixed(id2)) {
                nw[make_pair(idx2, idx2)] += weight;

                dx(idx2) += weight*(pin2->dx - pin1->dx - pdb.modules[idx1].x);
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
            idx1    = pdb.modules.module_index(id1);
            idx2    = pdb.modules.module_index(id2);
            weight = it->second;

            if(!pdb.modules.is_module_fixed(id1) && !pdb.modules.is_module_fixed(id2)) {
                nw[make_pair(idx1, idx1)] += weight;
                nw[make_pair(idx2, idx2)] += weight;
                nw[make_pair(idx1, idx2)] -= weight;
                nw[make_pair(idx2, idx1)] -= weight;

                dy(idx1) += weight*(pin1->dy - pin2->dy);
                dy(idx2) += weight*(pin2->dy - pin1->dy);
            } else if(!pdb.modules.is_module_fixed(id1) && pdb.modules.is_module_fixed(id2)) {
                nw[make_pair(idx1, idx1)] += weight;

                dy(idx1) += weight*(pin1->dy - pin2->dy - pdb.modules[idx2].y);
            } else if(pdb.modules.is_module_fixed(id1) && !pdb.modules.is_module_fixed(id2)) {
                nw[make_pair(idx2, idx2)] += weight;

                dy(idx2) += weight*(pin2->dy - pin1->dy - pdb.modules[idx1].y);
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
        assert( x_target.size() == pdb.modules.num_free() );
        assert( y_target.size() == pdb.modules.num_free() );

        unsigned int num_free = pdb.modules.num_free();
        SpMat Cx(num_free, num_free);
        SpMat Cy(num_free, num_free);
        VectorXd dx(num_free);
        dx = VectorXd::Zero(num_free);
        VectorXd dy(num_free);
        dy = VectorXd::Zero(num_free);

        compute_net_force_matrix(NWx, NWy, Cx, Cy, dx, dy);

        LLTSolver llt;

        VectorXd x_eigen_target = llt.compute(Cx).solve(dx*-1);
        VectorXd y_eigen_target = llt.compute(Cy).solve(dy*-1);

        assert( x_target.size() == x_eigen_target.size() );
        assert( y_target.size() == y_eigen_target.size() );

        VectorXd::Map(&x_target[0], x_eigen_target.size()) = x_eigen_target;
        VectorXd::Map(&y_target[0], y_eigen_target.size()) = y_eigen_target;
    }

    ////End TplStandardNetForceModel////


    ////Begin TplStandardMoveForceModel////
    TplMoveForceModelInterface::~TplMoveForceModelInterface()
    {
    }

    TplStandardMoveForceModel::~TplStandardMoveForceModel()
    {
    }

    TplStandardMoveForceModel::TplStandardMoveForceModel(SpMat &Cx0, SpMat &Cy0, double r1, double r2,
                                                         unsigned int grid_size, double mu) :
            _Cx0(Cx0), _Cy0(Cy0), _r1(r1), _r2(r2), _grid_size(grid_size), _mu(mu)
    {
        _bin_width  = pdb.modules.chip_width()  / _grid_size;
        _bin_height = pdb.modules.chip_height() / _grid_size;

        update_power_density();

        //initialize_move_force_matrix(_Cx0, _Cy0);
    }

    void TplStandardMoveForceModel::compute_heat_flux_vector(VectorXd &HFx, VectorXd &HFy)
    {
        //compute chip grid temperatures using green function method
        int dx = static_cast<int>(floor((_r2*10000)/(_bin_width*10000)));
        int dy = static_cast<int>(floor((_r2*10000)/(_bin_height*10000)));
        double tss[_grid_size+3][_grid_size+3] = {};

        double ts = 0;
        for(int i=-1; i<_grid_size+2; ++i) {
            for(int j=-1; j<_grid_size+2; ++j) {
                ts = 0;
                for(int i0=i-dx; i0<=i+dx; ++i0) {
                    for(int j0=j-dy; j0<=j+dy; ++j0) {
                        ts += green_function(make_pair(i, j), make_pair(i0, j0)) * power_density(i0, j0);
                    }
                }
                tss[i+1][j+1] = ts;
            }
        }

        //compute x and y direction head flux using finite difference method
        double grid_xhf[_grid_size+2][_grid_size+2] = {};
        double grid_yhf[_grid_size+2][_grid_size+2] = {};

        for(int i=0; i<_grid_size+2; ++i) {
            for(int j=0; j<_grid_size+2; ++i) {
                grid_xhf[i][j] = tss[i+1][j] - tss[i][j];
                grid_yhf[i][j] = tss[i][j+1] - tss[i][j];
            }
        }

        //compute module heat flux using bilinear interpolation method
        double x=0, y=0;//for module's coordinates
        int idx_x=0, idx_y=0;//for (x,y)'s containing grid point index
        double x1=0, x2=0, y1=0, y2=0;//for (x,y)'s containing grid coordinates
        double xhf=0, yhf=0;//for x and y heat flux value
        for(size_t i=0; i!=pdb.modules.size(); ++i) {
            //transform coordinate system
            x = pdb.modules[i].x + _bin_width/2;
            y = pdb.modules[i].y + _bin_height/2;

            idx_x = static_cast<int>(floor(x*10000/_bin_width*10000));
            idx_y = static_cast<int>(floor(y*10000/_bin_height*10000));

            x1 = idx_x * _bin_width;
            x2 = x1 + _bin_width;
            y1 = idx_y * _bin_height;
            y2 = y1 + _bin_height;

            xhf = grid_xhf[idx_x][idx_y]     * (x2-x) * (y2-y) +
                  grid_xhf[idx_x+1][idx_y]   * (x-x1) * (y2-y) +
                  grid_xhf[idx_x][idx_y+1]   * (x2-x) * (y-y1) +
                  grid_xhf[idx_x+1][idx_y+1] * (x-x1) * (y-y1) ;
            xhf = (xhf*10000)/(_bin_width*_bin_height*10000);

            yhf = grid_yhf[idx_x][idx_y]     * (x2-x) * (y2-y) +
                  grid_yhf[idx_x+1][idx_y]   * (x-x1) * (y2-y) +
                  grid_yhf[idx_x][idx_y+1]   * (x2-x) * (y-y1) +
                  grid_yhf[idx_x+1][idx_y+1] * (x-x1) * (y-y1) ;
            yhf = (yhf*10000)/(_bin_width*_bin_height*10000);

            HFx(i) = xhf;
            HFy(i) = yhf;
        }
    }


    double TplStandardMoveForceModel::green_function(const std::pair<int, int> &idx, const std::pair<int, int> &idx0) const
    {
        const int &i = idx.first;
        const int &j = idx.second;
        const int &i0 = idx0.first;
        const int &j0 = idx0.second;

        if(i==i0 && j==j0) return 0;

        double distance = sqrt(pow(abs(i-i0)*_bin_width, 2) + pow(abs(j-j0)*_bin_height, 2));

        if     (distance > _r2)  return 0;
        else if(distance <= _r1) return 1/distance;
        else                     return 0.6/sqrt(distance);
    }

    double TplStandardMoveForceModel::power_density(int i, int j) const
    {
        int gsize(_grid_size);

        assert(-gsize <= i && i < 2*gsize);
        assert(-gsize <= j && j < 2*gsize);

        int x_idx=i;
        if(i<0) x_idx = -i - 1;
        else if(i>=gsize) x_idx = -i + 2*gsize - 1;

        int y_idx=j;
        if(j<0) y_idx = -j - 1;
        else if(j>=gsize) y_idx = -j + 2*gsize - 1;

        return _power_density.at(x_idx).at(y_idx);
    }

    void TplStandardMoveForceModel::update_power_density()
    {
        _power_density.clear();
        _power_density.reserve(_grid_size);
        for(size_t i=0; i<_power_density.size(); ++i) {
            _power_density[i].reserve(_grid_size);
        }
        for_each(begin(_power_density), end(_power_density), [](vector<double> &p){
            for_each(begin(p), end(p), [](double &v){
                v = 0;
            });
        });

        double left, right, bottom, top;
        unsigned int idx_left, idx_right, idx_bottom, idx_top;
        for(TplModules::iterator it=pdb.modules.begin(); it!=pdb.modules.end(); ++it) {
            left   = it->x - it->width/2.0;
            right  = it->x + it->width/2.0;
            bottom = it->y - it->height/2.0;
            top    = it->y + it->height/2.0;

            idx_left   = static_cast<unsigned int>(floor((left*10000)  /(_bin_width*10000)) );
            idx_right  = static_cast<unsigned int>(ceil((right*10000)  /(_bin_width*10000)) );
            idx_bottom = static_cast<unsigned int>(floor((bottom*10000)/(_bin_height*10000)));
            idx_top    = static_cast<unsigned int>(ceil((top*10000)    /(_bin_height*10000)));

            if(fmod(left*10000,  _bin_width*10000   ) > _bin_width/2 ) idx_left   += 1;
            if(fmod(right*10000,  _bin_width*10000  ) < _bin_width/2 ) idx_right  -= 1;
            if(fmod(bottom*10000, _bin_height*10000 ) > _bin_height/2) idx_bottom += 1;
            if(fmod(top*10000,    _bin_height*10000 ) < _bin_height/2) idx_top    -= 1;

            for(unsigned int i=idx_left; i<idx_right; ++i) {
                for(unsigned int j=idx_bottom; j<idx_top; ++j) {
                    _power_density[i][j] += it->power_density;
                }
            }
        }
    }

    ////End TplStandardMoveForceModel////


    ////Begin TplStandardAlgorithm////

    TplAlgorithmInterface::~TplAlgorithmInterface()
    {
        delete net_force_model;
        delete move_force_model;
    }

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

    void TplStandardAlgorithm::initialize_move_force_matrix()
    {
        vector<SpElem> coefficients;
        unsigned int num_free = pdb.modules.num_free();
        vector<double> module_power;

        double avg_power = 0;
        double power;
        for(TplModules::iterator it=pdb.modules.begin(); it!=pdb.modules.end(); ++it) {
            power = it->width * it->height * it->power_density;
            avg_power +=  power;

            if(!it->fixed) module_power.push_back(power);
        }
        avg_power /= pdb.modules.size();

        for(size_t i=0; i<module_power.size(); ++i) {
            coefficients.push_back(SpElem(i, i, module_power[i]/avg_power/num_free));
        }

        Cx0.setFromTriplets(coefficients.begin(), coefficients.end());
        Cy0.setFromTriplets(coefficients.begin(), coefficients.end());
    }

    void TplStandardAlgorithm::update_move_force_matrix()
    {

    }

    ////End TplStandardAlgorithm////

}//end namespace tpl

