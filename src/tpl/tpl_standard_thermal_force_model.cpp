#include "tpl_standard_thermal_force_model.h"

#include "tpl_standard_thermal_model.h"
#include "tpl_db.h"
#include "tpl_config.h"

namespace tpl {
    using std::shared_ptr;

    const int TplStandardThermalForceModel::TIMES = 10000;


    void TplStandardThermalForceModel::compute_heat_flux_vector(const std::vector<std::vector<double>> &tss, VectorXd &HFx, VectorXd &HFy) const
    {
        /*
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
         */
    }

    bool TplStandardThermalForceModel::shouldStop() {
        return terminate.shouldStop();

    }

    bool Terminate::shouldStop() {
        const double STOP = 0.2;

        unsigned long A = 0; // total area of all modules

        segments.clear();
        nodes.clear();
        pos.clear();

        // calculate total area and initialize segments and nodes
        for (TplModules::iterator module_iter = TplDB::db().modules.begin();
             module_iter != TplDB::db().modules.end(); module_iter++) {
            A += module_iter->width * module_iter->height;

            // push bottom y of a module
            Segment s(module_iter->x, module_iter->x + module_iter->width, module_iter->y, 1);
            segments.push_back(s);
            // push top y of a module
            s.y += module_iter->height;
            s.flag = -1;
            segments.push_back(s);

            // map x to pos[index]
            pos.push_back(module_iter->x);
            pos.push_back(module_iter->x + module_iter->width);
        }

        // sort pos
        // delete same x, map x to pos[0] to pos[m-1]
        int m = 0;
        for (int i = 1; i < pos.size(); i++) {
            if (pos[i] != pos[i - 1]) {
                pos[m++] = pos[i];
            }
        }
        sort(pos.begin(), pos.begin() + m);

        // sort segments
        sort(segments.begin(), segments.begin() + m);

        // leaf number = m => nodes number = m + m/2 + ... + 1 = 2m - 1
        nodes.resize(2 * m - 1);
        // build segment tree
        build(0, m - 1, 0);

        // modules area union
        unsigned long res = 0;
        for (int i = 0; i < m - 1; i++) {
            int low = binarySearch(0, m - 1, segments[i].x1);
            int high = binarySearch(0, m - 1, segments[i].x2);
            update(low, high, segments[i].flag, 0);
            res += nodes[0].len * (segments[i + 1].y - segments[i].y);
        }

        double u = 1 - double(res) / A;
        return u < 0.2;
    }

    // update segment length in an interval
    void Terminate::pushup(int tidx) {
        if (nodes[tidx].cover != 0) {
            nodes[tidx].len = pos[nodes[tidx].high] - pos[nodes[tidx].low];
        }
        else if (nodes[tidx].low == nodes[tidx].high) {
            nodes[tidx].len = 0;
        }
        else {
            nodes[tidx].len = pos[nodes[2 * tidx + 1].len] + pos[nodes[2 * tidx + 2].len];
        }
    }

    // update segment cover count
    void Terminate::update(int low, int high, int flag, int tidx) {
        // if nodes[idx] is exactly the segment [low, high]
        if (nodes[tidx].low == low && nodes[tidx].high == high) {
            nodes[tidx].cover += flag;
            pushup(tidx);
            return ;
        }
        int mid = (nodes[tidx].low + nodes[tidx].high) / 2;
        // equal or not should be identity with build()
        // when build() the mid is belong to left child
        if (high <= mid) {
            update(low, high, flag, 2 * tidx + 1);
        }
        else if (low > mid) {
            update(low, high, flag, 2 * tidx + 2);
        }
        else {
            update(low, mid, flag, 2 * tidx + 1);
            update(mid + 1, high, flag, 2 * tidx + 2);
        }
    }


    int Terminate::binarySearch(int low, int high, double target) {
        while (low <= high) {
            int mid = (low + high) / 2;
            if (pos[mid] == target) return mid;
            if (pos[mid] < target)
                low = mid + 1;
            else
                high = mid - 1;
        }
        return -1;
    }

    void Terminate::build(int low, int high, int tidx) {
        nodes[tidx].low = low;
        nodes[tidx].high = high;
        nodes[tidx].cover = 0;
        nodes[tidx].len = 0;
        if (low == high) return ;
        // mid belong to left child, right child starts from mid + 1
        // left child = 2 * idx + 1, right child = 2 * idx + 2
        int mid = (low + high) / 2;
        build(low, mid, 2 * tidx + 1);
        build(mid + 1, high, 2 * tidx + 2);
    }




}//namespace tpl
