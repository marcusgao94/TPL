#include "tpl_standard_thermal_force_model.h"

#include "tpl_standard_thermal_model.h"
#include "tpl_db.h"
#include "tpl_config.h"

namespace tpl {
    using std::shared_ptr;

    const int TplStandardThermalForceModel::TIMES = 10000;

    void TplStandardThermalForceModel::compute_heat_flux_vector(const std::vector<std::vector<double>> &tss, VectorXd &HFx, VectorXd &HFy) const
    {
        //preconditions
        shared_ptr<TplStandardThermalModel> ptm = dynamic_pointer_cast<TplStandardThermalModel, TplAbstractThermalModel>(_tmodel);
        assert(ptm != nullptr);

        assert(HFx.rows() == TplDB::db().modules.num_free());
        assert(HFy.rows() == TplDB::db().modules.num_free());

        const int    &gsize   = ptm->grid_size();
        assert(tss.size() == 0 );

        const double &gwidth  = ptm->grid_width();
        const double &gheight = ptm->grid_height();

        HFx = VectorXd::Zero(TplDB::db().modules.num_free());
        HFy = VectorXd::Zero(TplDB::db().modules.num_free());

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
        for(size_t i=0; i!=TplDB::db().modules.size(); ++i) {
            //transform coordinate system
            //x = TplDB::db().modules[i].x + gwidth/2;
            //y = TplDB::db().modules[i].y + gheight/2;
            x = TplDB::db().modules[i].x;
            y = TplDB::db().modules[i].y;

            idx_x = static_cast<int>(floor((x*TIMES)/(gwidth*TIMES)));
            idx_y = static_cast<int>(floor((y*TIMES)/(gheight*TIMES)));

            x1 = idx_x * gwidth;
            x2 = x1 + gwidth;
            y1 = idx_y * gheight;
            y2 = y1 + gheight;

            xhf = grid_xhf[idx_x][idx_y]     * (x2-x) * (y2-y) +
                grid_xhf[idx_x+1][idx_y]   * (x-x1) * (y2-y) +
                grid_xhf[idx_x][idx_y+1]   * (x2-x) * (y-y1) +
                grid_xhf[idx_x+1][idx_y+1] * (x-x1) * (y-y1) ;
            xhf = (xhf*TIMES)/(gwidth*gheight*TIMES);

            yhf = grid_yhf[idx_x][idx_y]     * (x2-x) * (y2-y) +
                grid_yhf[idx_x+1][idx_y]   * (x-x1) * (y2-y) +
                grid_yhf[idx_x][idx_y+1]   * (x2-x) * (y-y1) +
                grid_yhf[idx_x+1][idx_y+1] * (x-x1) * (y-y1) ;
            yhf = (yhf*TIMES)/(gwidth*gheight*TIMES);

            HFx(i) = xhf;
            HFy(i) = yhf;
        }
    }

}//namespace tpl
