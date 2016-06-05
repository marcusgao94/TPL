#include "tpl_standard_thermal_force_model.h"

#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <iomanip>
#include <chrono>
using std::cout;
using std::endl;

namespace tpl {
    using std::shared_ptr;
    using std::make_shared;
    using std::map;
    using std::pair;
    using std::make_pair;
    using std::vector;

#ifdef USE_STXXL
    TplStandardThermalForceModel::TplStandardThermalForceModel() : TplAbstractThermalForceModel(), _bs(64*1024*1024)
#else
    TplStandardThermalForceModel::TplStandardThermalForceModel()
#endif
    {
        initialize_model();


        _gw_num = floor( TplDB::db().modules.chip_width()  / BIN_WIDTH );
        _gh_num = floor( TplDB::db().modules.chip_height() / BIN_HEIGHT );

        BIN_WIDTH  = TplDB::db().modules.chip_width()  * 1.0 / _gw_num;
        BIN_HEIGHT = TplDB::db().modules.chip_height() * 1.0 / _gh_num;

#ifdef USE_STXXL
        _power_density     = make_shared<TMat>(_bs, _gw_num+1, _gh_num+1);
        _thermal_signature = make_shared<TMat>(_bs, _gw_num+3, _gh_num+3);
        _xhf_grid          = make_shared<TMat>(_bs, _gw_num+1, _gh_num+1);
        _yhf_grid          = make_shared<TMat>(_bs, _gw_num+1, _gh_num+1);
#else
        _power_density = (double**)malloc( sizeof(double*) * (_gw_num+1) );
        assert(_power_density != nullptr);
        for (int i=0; i<_gw_num+1; ++i) {
            *(_power_density+i) = (double*)malloc( sizeof(double) * (_gh_num+1) );
            assert(*(_power_density+i) != nullptr);
            memset(*(_power_density+i), 0, sizeof(double) * (_gh_num+1) );
        }

        _thermal_signature = (double**)malloc( sizeof(double*) * (_gw_num+3) );
        assert(_thermal_signature != nullptr);
        for (int i=0; i<_gw_num+3; ++i) {
            *(_thermal_signature+i) = (double*)malloc( sizeof(double) * (_gh_num+3) );
            assert(*(_thermal_signature+i) != nullptr);
            memset(*(_thermal_signature+i), 0, sizeof(double) * (_gh_num+3) );
        }

        _xhf_grid = (double**)malloc( sizeof(double*) * (_gw_num+1) );
        assert(_xhf_grid != nullptr);
        for (int i=0; i<_gw_num+1; ++i) {
            *(_xhf_grid+i) = (double*)malloc( sizeof(double) * (_gh_num+1) );
            assert(*(_xhf_grid+i) != nullptr);
            memset(*(_xhf_grid+i), 0, sizeof(double) * (_gh_num+1) );
        }

        _yhf_grid = (double**)malloc( sizeof(double*) * (_gw_num+1) );
        assert(_yhf_grid != nullptr);
        for (int i=0; i<_gw_num+1; ++i) {
            *(_yhf_grid+i) = (double*)malloc( sizeof(double) * (_gh_num+1) );
            assert(*(_yhf_grid+i) != nullptr);
            memset(*(_yhf_grid+i), 0, sizeof(double) * (_gh_num+1) );
        }
#endif

        int dx = static_cast<int>( ceil(R2 / BIN_WIDTH));
        int dy = static_cast<int>( ceil(R2 / BIN_HEIGHT));

#ifdef USE_STXXL
        _green_function    = make_shared< vector< vector<double> > >(dx, vector<double>(dy, 0));
#else
        _green_function = (double**)malloc( sizeof(double*) * dx);
        assert(_green_function != nullptr);
        for (int i=0; i<dx; ++i) {
            *(_green_function+i) = (double*)malloc( sizeof(double) * dy);
            assert(*(_green_function+i) != nullptr);
            memset(*(_green_function+i), 0, sizeof(double) * dy );
        }
#endif

        double green_func_val = 0;
        double distance_squre = 0;
        for (int i=0; i<dx; ++i) {
            for (int j=0; j<dy; ++j) {
                if (i==0 && j==0) {
                    green_func_val = 0;
                } else {
                    distance_squre = i*i*BIN_WIDTH*BIN_WIDTH + j*j*BIN_HEIGHT*BIN_HEIGHT;
                    if (distance_squre <= R1*R1) {
                        green_func_val =  1.0 / sqrt(distance_squre);
                    } else if (distance_squre <= R2*R2) {
                        green_func_val = 0.6 / sqrt(sqrt(distance_squre));
                    } else {
                        green_func_val = 0;
                    }
                }

#ifdef USE_STXXL
                _green_function->at(i).at(j) = green_func_val;
#else
                _green_function[i][j] = green_func_val;
#endif
            }
        }
    }

#ifndef USE_STXXL
    TplStandardThermalForceModel::~TplStandardThermalForceModel()
    {
        for (int i=0; i<_gw_num+1; ++i) {
            free( *(_power_density+i) );
        }
        free(_power_density);

        for (int i=0; i<_gw_num+3; ++i) {
            free( *(_thermal_signature+i) );
        }
        free(_thermal_signature);

        for (int i=0; i<_gw_num+1; ++i) {
            free( *(_xhf_grid+i) );
        }
        free(_xhf_grid);

        for (int i=0; i<_gw_num+1; ++i) {
            free( *(_yhf_grid+i) );
        }
        free(_yhf_grid);

        int dx = static_cast<int>( ceil(R2 / BIN_WIDTH));
//        int dy = static_cast<int>( ceil(R2 / BIN_HEIGHT));

        for (int i=0; i<dx; ++i) {
            free(*(_green_function+i));
        }
        free(_green_function);
    }
#endif

    bool TplStandardThermalForceModel::initialize_model()
    {
        try {
            namespace pt = boost::property_tree;
            pt::ptree tree;
            pt::read_json(getenv("TPLCONFIG"), tree);

            BIN_WIDTH  = tree.get<double>("bin_width");
            BIN_HEIGHT = tree.get<double>("bin_height");
            R1         = tree.get<double>("r1");
            R2         = tree.get<double>("r2");
            MU         = tree.get<double>("mu");

            return true;
        } catch(...) {
            return false;
        }
    }

    void TplStandardThermalForceModel::compute_heat_flux_vector(VectorXd &HFx, VectorXd &HFy)
    {
//        cout << "begin compute heat flux vector" << endl;
//        std::chrono::time_point<std::chrono::system_clock> start, end;
//        std::chrono::duration<double> elapsed_seconds;

        //preconditions
        assert(HFx.rows() == TplDB::db().modules.num_free());
        assert(HFy.rows() == TplDB::db().modules.num_free());

        generate_power_density();

        generate_thermal_profile();

        generate_heat_flux_grid();

        //compute module heat flux using bilinear interpolation method
        double x=0, y=0;//for module's coordinates
        int idx_x=0, idx_y=0;//for (x,y)'s containing grid point index
        double x1=0, x2=0, y1=0, y2=0;//for (x,y)'s containing grid coordinates
        double xhf=0, yhf=0;//for x and y heat flux value
        for(size_t i=0; i!=TplDB::db().modules.num_free(); ++i) {
            x = TplDB::db().modules[i].x + TplDB::db().modules[i].width/2.0;  //module center x
            y = TplDB::db().modules[i].y + TplDB::db().modules[i].height/2.0; //module center y

            idx_x = static_cast<int>(floor(x/BIN_WIDTH));
            idx_y = static_cast<int>(floor(y/BIN_HEIGHT));

            x1 = idx_x * BIN_WIDTH;
            x2 = x1 + BIN_WIDTH;
            y1 = idx_y * BIN_HEIGHT;
            y2 = y1 + BIN_HEIGHT;

#ifdef USE_STXXL
            TMat & xhf_grid = *_xhf_grid;
            xhf = xhf_grid(idx_x,idx_y)   * (x2-x) * (y2-y) +
                xhf_grid(idx_x+1,idx_y)   * (x-x1) * (y2-y) +
                xhf_grid(idx_x,idx_y+1)   * (x2-x) * (y-y1) +
                xhf_grid(idx_x+1,idx_y+1) * (x-x1) * (y-y1) ;
            xhf = xhf / (BIN_WIDTH * BIN_HEIGHT);

            TMat & yhf_grid = *_yhf_grid;
            yhf = yhf_grid(idx_x,idx_y)   * (x2-x) * (y2-y) +
                yhf_grid(idx_x+1,idx_y)   * (x-x1) * (y2-y) +
                yhf_grid(idx_x,idx_y+1)   * (x2-x) * (y-y1) +
                yhf_grid(idx_x+1,idx_y+1) * (x-x1) * (y-y1) ;
            yhf = yhf / (BIN_WIDTH * BIN_HEIGHT);
#else
            xhf = _xhf_grid[idx_x]   [idx_y]   * (x2-x) * (y2-y) +
                  _xhf_grid[idx_x+1] [idx_y]   * (x-x1) * (y2-y) +
                  _xhf_grid[idx_x]   [idx_y+1] * (x2-x) * (y-y1) +
                  _xhf_grid[idx_x+1] [idx_y+1] * (x-x1) * (y-y1) ;
            xhf = xhf / (BIN_WIDTH * BIN_HEIGHT);

            yhf = _yhf_grid[idx_x]   [idx_y]   * (x2-x) * (y2-y) +
                  _yhf_grid[idx_x+1] [idx_y]   * (x-x1) * (y2-y) +
                  _yhf_grid[idx_x]   [idx_y+1] * (x2-x) * (y-y1) +
                  _yhf_grid[idx_x+1] [idx_y+1] * (x-x1) * (y-y1) ;
            yhf = yhf / (BIN_WIDTH * BIN_HEIGHT);
#endif

            HFx(i) = xhf;
            HFy(i) = yhf;
        }
    }



    void TplStandardThermalForceModel::generate_power_density()
    {
#ifdef USE_STXXL
        TMat &power_density = *_power_density;
#endif
        try {
            // reset _power_density
#ifdef USE_STXXL
            power_density.set_zero();
#else
            for (int i=0; i<_gw_num+1; ++i) {
                memset(*(_power_density+i), 0, sizeof(double) * (_gh_num+1) );
            }
#endif

            double left, right, bottom, top;
            unsigned int idx_left, idx_right, idx_bottom, idx_top;
            for(TplModules::iterator it=TplDB::db().modules.begin(); it!=TplDB::db().modules.end(); ++it) {

                left   = it->x;
                right  = it->x + it->width;
                bottom = it->y;
                top    = it->y + it->height;

                idx_left   = static_cast<unsigned int>( ceil (left   / BIN_WIDTH ) );
                idx_right  = static_cast<unsigned int>( floor(right  / BIN_WIDTH ) );
                idx_bottom = static_cast<unsigned int>( ceil (bottom / BIN_HEIGHT) );
                idx_top    = static_cast<unsigned int>( floor(top    / BIN_HEIGHT) );

                for(unsigned int i=idx_left; i<=idx_right; ++i) {
                    for(unsigned int j=idx_bottom; j<=idx_top; ++j) {
#ifdef USE_STXXL
                        *power_density(i,j) += it->power_density;
#else
                        _power_density[i][j] += it->power_density;
#endif
                    }
                }
            }
        } catch(...) {
            std::cout << "update power density exception"  << std::endl;
        }
    }

    double TplStandardThermalForceModel::power_density(int i, int j)
    {
        assert(-_gw_num-1 <= i && i <= 2*_gw_num+1);
        assert(-_gh_num-1 <= j && j <= 2*_gh_num+1);

        int                                      x_idx = i;//[0...gw_num] remain
        if     (i < 0       && i >= -_gw_num-1 ) x_idx = -1-i;//-1<==>0
        else if(i > _gw_num && i <= 2*_gw_num+1) x_idx = 2*_gw_num+1-i;//gw_num+1<==>gw_num
        else if(i < -_gw_num-1 || i > 2*_gw_num+1) {
            std::cout << "i index error : " << i << std::endl;
            exit(-1);
        }

        int                                      y_idx = j;
        if     (j < 0       && j >= -_gh_num-1 ) y_idx = -1-j;
        else if(j > _gw_num && j <= 2*_gh_num+1) y_idx = 2*_gh_num+1-j;
        else if(j < -_gh_num-1 || j > 2*_gh_num+1) {
            std::cout << "j index error : " << j << std::endl;
            exit(-1);
        }

#ifdef USE_STXXL
        TMat &power_density = *_power_density;
        return *power_density(x_idx, y_idx);
#else
        assert(0<= x_idx && x_idx < _gw_num+1);
        assert(0<= y_idx && y_idx < _gh_num+1);
        return _power_density[x_idx][y_idx];
#endif
    }

    /*
    double TplStandardThermalForceModel::green_function(int i, int j, int i0, int j0)
    {
        if(i==i0 && j==j0) return 0;

        double distance = sqrt(pow(abs(i-i0) * BIN_WIDTH, 2) + pow(abs(j - j0) * BIN_HEIGHT, 2));

        if     (distance >  R2)  return 0;
        else if(distance <= R1)  return 1/distance;
        else                     return 0.6/sqrt(distance);
    }
     */

    void TplStandardThermalForceModel::generate_thermal_profile() {
#ifdef USE_STXXL
        TMat &thermal_signature = *_thermal_signature;

        thermal_signature.set_zero();

        const int & green_func_window_width  = _green_function->size();
        const int & green_func_window_height = _green_function->at(0).size();
#else
        for (int i=0; i<_gw_num+3; ++i) {
            memset(*(_thermal_signature+i), 0, sizeof(double) * (_gh_num+3) );
        }

        int green_func_window_width  = static_cast<int>( ceil(R2 / BIN_WIDTH));
        int green_func_window_height = static_cast<int>( ceil(R2 / BIN_HEIGHT));
#endif

        // compute chip grid temperatures using green function method
        double ts = 0;
        //1. compute thermal signature in the chip.
        for (int i = 0; i <= _gw_num; ++i) {
            for (int j = 0; j <= _gh_num; ++j) {
                ts = 0;

                for (int dx = -green_func_window_width+1; dx<green_func_window_width; ++dx) {
                    for (int dy = -green_func_window_height+1; dy<green_func_window_height; ++dy) {
#ifdef USE_STXXL
                        ts += _green_function->at(abs(dx)).at(abs(dy)) * power_density(i+dx, j+dy);
#else
                        ts += _green_function[abs(dx)][abs(dy)] * power_density(i+dx, j+dy);
#endif
                    }
                }

#ifdef USE_STXXL
                *thermal_signature(i + 1, j + 1) = ts;
#else
                _thermal_signature[i+1][j+1] = ts;
#endif
            }
        }

        //2. compute thermal signature around the chip.
        //bottom and top side
        for (int i = 0; i <= _gw_num; ++i) {
#ifdef USE_STXXL
            *thermal_signature(i, 0) = *thermal_signature(i, 2);
            *thermal_signature(i, _gh_num + 2) = *thermal_signature(i, _gh_num);
#else
            _thermal_signature[i][0] = _thermal_signature[i][2];
            _thermal_signature[i][_gh_num + 2] = _thermal_signature[i][_gh_num];
#endif
        }

        //left and right side
        for (int j = 0; j <= _gh_num; ++j) {
#ifdef USE_STXXL
            *thermal_signature(0, j) = *thermal_signature(2, j);
            *thermal_signature(_gw_num + 2, j) = *thermal_signature(_gw_num, j);
#else
            _thermal_signature[0][j] = _thermal_signature[2][j];
            _thermal_signature[_gw_num + 2][j] = _thermal_signature[_gw_num][j];
#endif
        }

        //four corners
#ifdef USE_STXXL
        *thermal_signature(0, 0) = *thermal_signature(2, 2);//ll
        *thermal_signature(_gw_num + 2, 0) = *thermal_signature(_gw_num, 2); //lr
        *thermal_signature(_gw_num + 2, _gh_num + 2) = *thermal_signature(_gw_num, _gh_num);//tr
        *thermal_signature(0, _gh_num) = *thermal_signature(2, _gh_num - 2);//tl
#else
        _thermal_signature[0][0] = _thermal_signature[2][2];//ll
        _thermal_signature[_gw_num + 2][0] = _thermal_signature[_gw_num][2]; //lr
        _thermal_signature[_gw_num + 2][_gh_num + 2] = _thermal_signature[_gw_num][_gh_num];//tr
        _thermal_signature[0][_gh_num] = _thermal_signature[2][_gh_num - 2];//tl
#endif
    }


    void TplStandardThermalForceModel::generate_heat_flux_grid()
    {
#ifdef USE_STXXL
        //aliases
        TMat & xhf_grid = *_xhf_grid;
        TMat & yhf_grid = *_yhf_grid;
        TMat & thermal_signature = *_thermal_signature;
        xhf_grid.set_zero();
        yhf_grid.set_zero();
#else
        for (int i=0; i<_gw_num+1; ++i) {
            memset(*(_xhf_grid+i), 0, sizeof(double) * (_gh_num+1) );
        }

        for (int i=0; i<_gw_num+1; ++i) {
            memset(*(_yhf_grid+i), 0, sizeof(double) * (_gh_num+1) );
        }
#endif
        //compute x and y direction head flux using finite difference method
        for (int i=0; i<=_gw_num; ++i) {
            for (int j=0; j<=_gh_num; ++j) {
#ifdef USE_STXXL
                *xhf_grid(i, j) = (*thermal_signature(i+2, j+1) - *thermal_signature(i, j+1)) / 2;
                *yhf_grid(i, j) = (*thermal_signature(i+1, j+2) - *thermal_signature(i+1, j)) / 2;
#else
                _xhf_grid[i][j] = (_thermal_signature[i+2][j+1] - _thermal_signature[i][j+1]) / 2;
                _yhf_grid[i][j] = (_thermal_signature[i+1][j+2] - _thermal_signature[i+1][j]) / 2;
#endif
            }
        }
    }

}//namespace tpl
