#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <algorithm>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "tpl_standard_algorithm.h"

namespace tpl {
    using namespace std;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Begin TplConfigParser////

    TplConfig *TplConfig::_instance = 0;

    TplConfig *TplConfig::instance()
    {
        if(_instance == 0) {
            _instance = new TplConfig;
        }
        return _instance;
    }

    void TplConfig::destroy()
    {
        delete _instance;
        _instance = 0;
    }

    TplConfig::TplConfig()
    {
        namespace pt = boost::property_tree;
        pt::ptree tree;

        char path[200];
        char *configpath = std::getenv("TPLCONFIG");
        std::strcpy(path, configpath);
        std::strcat(path,"/config.json");

        pt::read_json(path, tree);

        _gsize = tree.get<int>("grid_size");
        _r1 = tree.get<double>("r1");
        _r2 = tree.get<double>("r2");
        _mu = tree.get<double>("mu");
        _times = tree.get<int>("times");
    }

    TplConfig &tplconfig = *TplConfig::instance();

    ////End TplConfigParser////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Begin TplStandardNetModel////

    void TplStandardNetModel::compute_net_weight(NetWeight &NWx, NetWeight &NWy)
    {
        //preconditions
        NWx.clear();
        NWy.clear();

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
            //loop to compute current pin pair set's non zero net weights
            //pin pair <==> net weight
            for(size_t cur_idx=0; cur_idx<degree; ++cur_idx)     {
                if( cur_idx == xmin_idx || cur_idx == xmax_idx ||
                    cur_idx == ymin_idx || cur_idx == ymax_idx ) {
                    continue;
                } else {
                    if (xs[cur_idx] == xmin) {
                        NWx[make_pair(pins[cur_idx], pins[xmax_idx])] = 2.0 / (degree-1) * (xmax - xmin);
                    } else if (xs[cur_idx] == xmax) {
                        NWx[make_pair(pins[cur_idx], pins[xmin_idx])] = 2.0 / (degree-1) * (xmax - xmin);
                    } else {
                        NWx[make_pair(pins[cur_idx], pins[xmin_idx])] = 2.0 / (degree-1) * (xs[cur_idx] - xmin);
                        NWx[make_pair(pins[cur_idx], pins[xmax_idx])] = 2.0 / (degree-1) * (xmax - xs[cur_idx]);
                    }

                    if (ys[cur_idx] == ymin) {
                        NWy[make_pair(pins[cur_idx], pins[ymax_idx])] = 2.0 / (degree-1) * (ymax - ymin);
                    }
                    else if (ys[cur_idx] == ymax) {
                        NWy[make_pair(pins[cur_idx], pins[ymin_idx])] = 2.0 / (degree-1) * (ymax - ymin);
                    } else {
                        NWy[make_pair(pins[cur_idx], pins[ymin_idx])] = 2.0 / (degree-1) * (xs[cur_idx] - ymin);
                        NWy[make_pair(pins[cur_idx], pins[ymax_idx])] = 2.0 / (degree-1) * (ymax - xs[cur_idx]);
                    }
                }
            }

            NWx[make_pair(pins[xmin_idx], pins[xmax_idx])] = 2.0/(degree-1)*(xmax-xmin);
            NWy[make_pair(pins[ymin_idx], pins[ymax_idx])] = 2.0/(degree-1)*(ymax-ymin);
        }
    }

    ////End TplStandardNetModel////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Begin TplStandardNetForceModel////

    void TplStandardNetForceModel::compute_net_force_matrix(const NetWeight &NWx, const NetWeight &NWy,
                                                            SpMat &Cx, SpMat &Cy, VectorXd &dx, VectorXd &dy)
    {
        //preconditions
        //TODO:refine assersions
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
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Begin TplStandardThermalModel////

    TplStandardThermalModel::TplStandardThermalModel()
    {
        _gsize   = tplconfig.grid_size();
        _gwidth  = pdb.modules.chip_width()  / tplconfig.grid_size();
        _gheight = pdb.modules.chip_height() / tplconfig.grid_size();

        update_power_density();
    }

    double TplStandardThermalModel::green_function(int i, int j, int i0, int j0) const
    {
        if(i==i0 && j==j0) return 0;

        double distance = sqrt(pow(abs(i-i0) * _gwidth, 2) + pow(abs(j - j0) * _gheight, 2));

        if     (distance >  tplconfig.r2())  return 0;
        else if(distance <= tplconfig.r1())  return (1*tplconfig.times())/(distance*tplconfig.times());
        else                                 return (0.6*tplconfig.times())/(sqrt(distance)*tplconfig.times());
    }

    double TplStandardThermalModel::power_density(int i, int j) const
    {

        assert(-_gsize <= i && i < 2*_gsize);
        assert(-_gsize <= j && j < 2*_gsize);

        int               x_idx = i;//[0...gsize] remain
        if     (i<0)      x_idx = -i;//-1<==>1
        else if(i>_gsize) x_idx = 2*_gsize-i;//gsize+1<==>gsize-1

        int               y_idx = j;
        if     (j<0)      y_idx = -j;
        else if(j>_gsize) y_idx = 2*_gsize-j;

        return _power_density.at(x_idx).at(y_idx);
    }

    void TplStandardThermalModel::update_power_density()
    {
        //reset _power_density
        _power_density.clear();

        for(int i=0; i<=_gsize; ++i) {
            _power_density.push_back(vector<double>(_gsize+1, 0));
        }

        double left, right, bottom, top;
        unsigned int idx_left, idx_right, idx_bottom, idx_top;
        for(TplModules::iterator it=pdb.modules.begin(); it!=pdb.modules.end(); ++it) {
            left   = it->x - it->width/2.0;
            right  = it->x + it->width/2.0;
            bottom = it->y - it->height/2.0;
            top    = it->y + it->height/2.0;

            idx_left   = static_cast<unsigned int>( ceil ( (left*tplconfig.times()  )/(_gwidth*tplconfig.times() ) ) );
            idx_right  = static_cast<unsigned int>( floor( (right*tplconfig.times() )/(_gwidth*tplconfig.times() ) ) );
            idx_bottom = static_cast<unsigned int>( ceil ( (bottom*tplconfig.times())/(_gheight*tplconfig.times()) ) );
            idx_top    = static_cast<unsigned int>( floor( (top*tplconfig.times()   )/(_gheight*tplconfig.times()) ) );

            for(unsigned int i=idx_left; i<idx_right; ++i) {
                for(unsigned int j=idx_bottom; j<idx_top; ++j) {
                    _power_density[i][j] += it->power_density;
                }
            }
        }
    }

    ////End TplStandardThermalModel////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Begin TplStandardThermalForceModel////

    TplThermalForceModelInterface::TplThermalForceModelInterface(TplThermalModelInterface *thermal_model)
    {
        initialize_thermal_model(thermal_model);
    }

    TplThermalForceModelInterface::~TplThermalForceModelInterface()
    {
        delete _tmodel;
        _tmodel = 0;
    }

    TplStandardThermalForceModel::TplStandardThermalForceModel(TplThermalModelInterface *thermal_model)
            : TplThermalForceModelInterface(thermal_model)
    {
    }

    void TplStandardThermalForceModel::initialize_thermal_model(TplThermalModelInterface *thermal_model)
    {
        _tmodel = thermal_model;
    }

    void TplStandardThermalForceModel::compute_heat_flux_vector(VectorXd &HFx, VectorXd &HFy)
    {
        //preconditions
        assert(HFx.rows() == pdb.modules.num_free());
        assert(HFy.rows() == pdb.modules.num_free());

        HFx = VectorXd::Zero(pdb.modules.num_free());
        HFy = VectorXd::Zero(pdb.modules.num_free());

        //local data
        const int    &gsize   = dynamic_cast<TplStandardThermalModel*>(_tmodel)->grid_size();
        const double &gwidth  = dynamic_cast<TplStandardThermalModel*>(_tmodel)->grid_width();
        const double &gheight = dynamic_cast<TplStandardThermalModel*>(_tmodel)->grid_height();

        int dx = static_cast<int>(ceil((tplconfig.r2()*tplconfig.times())/(gwidth*tplconfig.times())));
        int dy = static_cast<int>(ceil((tplconfig.r2()*tplconfig.times())/(gheight*tplconfig.times())));

        //compute chip grid temperatures using green function method
        double tss[gsize+3][gsize+3] = {};
        double ts = 0;
        for(int i=-1; i<gsize+2; ++i) {
            for(int j=-1; j<gsize+2; ++j) {
                ts = 0;
                for(int i0=i-dx; i0<=i+dx; ++i0) {
                    for(int j0=j-dy; j0<=j+dy; ++j0) {
                        ts += _tmodel->green_function(i, j, i0, j0) * _tmodel->power_density(i0, j0);
                    }
                }
                tss[i+1][j+1] = ts;
            }
        }

        //compute x and y direction head flux using finite difference method
        double grid_xhf[gsize+2][gsize+2] = {};
        double grid_yhf[gsize+2][gsize+2] = {};

        for(int i=0; i<gsize+1; ++i) {
            for(int j=0; j<gsize+1; ++j) {
                grid_xhf[i][j] = (tss[i+2][j+1] - tss[i][j+1])/2.0;
                grid_yhf[i][j] = (tss[i+1][j+2] - tss[i+1][j])/2.0;
            }
        }

        //compute module heat flux using bilinear interpolation method
        double x=0, y=0;//for module's coordinates
        int idx_x=0, idx_y=0;//for (x,y)'s containing grid point index
        double x1=0, x2=0, y1=0, y2=0;//for (x,y)'s containing grid coordinates
        double xhf=0, yhf=0;//for x and y heat flux value
        for(size_t i=0; i!=pdb.modules.size(); ++i) {
            //transform coordinate system
            //x = pdb.modules[i].x + gwidth/2;
            //y = pdb.modules[i].y + gheight/2;
            x = pdb.modules[i].x;
            y = pdb.modules[i].y;

            idx_x = static_cast<int>(floor((x*tplconfig.times())/(gwidth*tplconfig.times())));
            idx_y = static_cast<int>(floor((y*tplconfig.times())/(gheight*tplconfig.times())));

            x1 = idx_x * gwidth;
            x2 = x1 + gwidth;
            y1 = idx_y * gheight;
            y2 = y1 + gheight;

            xhf = grid_xhf[idx_x][idx_y]     * (x2-x) * (y2-y) +
                  grid_xhf[idx_x+1][idx_y]   * (x-x1) * (y2-y) +
                  grid_xhf[idx_x][idx_y+1]   * (x2-x) * (y-y1) +
                  grid_xhf[idx_x+1][idx_y+1] * (x-x1) * (y-y1) ;
            xhf = (xhf*tplconfig.times())/(gwidth*gheight*tplconfig.times());

            yhf = grid_yhf[idx_x][idx_y]     * (x2-x) * (y2-y) +
                  grid_yhf[idx_x+1][idx_y]   * (x-x1) * (y2-y) +
                  grid_yhf[idx_x][idx_y+1]   * (x2-x) * (y-y1) +
                  grid_yhf[idx_x+1][idx_y+1] * (x-x1) * (y-y1) ;
            yhf = (yhf*tplconfig.times())/(gwidth*gheight*tplconfig.times());

            HFx(i) = xhf;
            HFy(i) = yhf;
        }
    }


    ////End TplStandardThermalForceModel////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Begin TplStandardAlgorithm////

    TplAlgorithmInterface::~TplAlgorithmInterface()
    {
        delete net_model;
        net_model = 0;
        delete net_force_model;
        net_force_model = 0;
        delete thermal_model;
        thermal_model = 0;
        delete thermal_force_model;
        thermal_force_model = 0;
    }

    void TplStandardAlgorithm::initialize_models()
    {
        net_model           = new TplStandardNetModel;
        net_force_model     = new TplStandardNetForceModel;
        thermal_model       = new TplStandardThermalModel;
        thermal_force_model = new TplStandardThermalForceModel(thermal_model);
    }

    void TplStandardAlgorithm::make_initial_placement()
    {
        vector<double> x_target, y_target;
        NetWeight NWx, NWy;

        net_model->compute_net_weight(NWx, NWy);

        for(size_t i=0; i<5; ++i) {
            net_force_model->compute_net_force_target(NWx, NWy, x_target, y_target);
            pdb.modules.set_free_module_coordinates(x_target, y_target);

            x_target.clear();
            x_target.reserve(pdb.modules.num_free());

            y_target.clear();
            y_target.reserve(pdb.modules.num_free());
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
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}//end namespace tpl

