#include "tpl_standard_thermal_model.h"

#include <cmath>

#include "utils.h"
#include "tpl_config.h"
#include "tpl_db.h"

namespace tpl {

    using std::vector;

    const int TplStandardThermalModel::TIMES = 10000;

    TplStandardThermalModel::TplStandardThermalModel() : TplAbstractThermalModel()
    {
        _gsize   = boost::get<int>(TplConfig::instance()["init_grid_size"]);
        _gwidth  = TplDB::db().modules.chip_width()  / _gsize;
        _gheight = TplDB::db().modules.chip_height() / _gsize;

        update_power_density();
    }

    void TplStandardThermalModel::compute_thermal_distribution(std::vector<std::vector<double>> &distribution) const
    {
        //precondition
        distribution.clear();
        distribution.reserve(_gsize+3);
        for(int i=0; i<_gsize+3; ++i) {
            distribution[i].reserve(_gsize+3);
        }

        double radius = boost::get<double>(TplConfig::instance()["r2"]);
        int dx = static_cast<int>(ceil((radius * TIMES) / (_gwidth  * TIMES)));
        int dy = static_cast<int>(ceil((radius * TIMES) / (_gheight * TIMES)));

        //compute chip grid temperatures using green function method
        double ts = 0;
        for(int i=-1; i<_gsize+2; ++i) {
            for(int j=-1; j<_gsize+2; ++j) {
                ts = 0;
                for(int i0=i-dx; i0<=i+dx; ++i0) {
                    for(int j0=j-dy; j0<=j+dy; ++j0) {
                        ts += green_function(i, j, i0, j0) * power_density(i0, j0);
                    }
                }
                distribution[i+1][j+1] = ts;
            }
        }
    }

    double TplStandardThermalModel::green_function(int i, int j, int i0, int j0) const
    {
        if(i==i0 && j==j0) return 0;

        double distance = sqrt(pow(abs(i-i0) * _gwidth, 2) + pow(abs(j - j0) * _gheight, 2));

        const double &r1 = boost::get<double>(TplConfig::instance()["r1"]);
        const double &r2 = boost::get<double>(TplConfig::instance()["r2"]);

        if     (distance >  r2)  return 0;
        else if(distance <= r1)  return (1*TIMES)/(distance*TIMES);
        else                     return (0.6*TIMES)/(sqrt(distance)*TIMES);
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
        for(TplModules::iterator it=TplDB::db().modules.begin(); it!=TplDB::db().modules.end(); ++it) {
            left   = it->x - it->width/2.0;
            right  = it->x + it->width/2.0;
            bottom = it->y - it->height/2.0;
            top    = it->y + it->height/2.0;

            idx_left   = static_cast<unsigned int>( ceil ( (left*TIMES  )/(_gwidth*TIMES ) ) );
            idx_right  = static_cast<unsigned int>( floor( (right*TIMES )/(_gwidth*TIMES ) ) );
            idx_bottom = static_cast<unsigned int>( ceil ( (bottom*TIMES)/(_gheight*TIMES) ) );
            idx_top    = static_cast<unsigned int>( floor( (top*TIMES   )/(_gheight*TIMES) ) );

            for(unsigned int i=idx_left; i<idx_right; ++i) {
                for(unsigned int j=idx_bottom; j<idx_top; ++j) {
                    _power_density[i][j] += it->power_density;
                }
            }
        }
    }

}//namespace tpl
