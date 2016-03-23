#include "tpl_standard_net_model.h"

namespace tpl {

    using std::vector;
    using std::make_pair;

    void TplStandardNetModel::compute_net_weight(NetWeight &NWx, NetWeight &NWy)
    {
        //preconditions
        NWx.clear();
        NWy.clear();

        //define pin related data
        vector<TplPin*> pins = {};
        vector<double> xs = {};
        vector<double> ys = {};
        double xmin=TplDB::db().modules.chip_width(), xmax=-1, ymin=TplDB::db().modules.chip_height(), ymax=-1;
        size_t xmin_idx=-1, xmax_idx=-1, ymin_idx=-1, ymax_idx=-1;

        //local helper variables
        size_t idx;
        double curx, cury;

        for(TplNets::net_iterator nit=TplDB::db().nets.net_begin(); nit!=TplDB::db().nets.net_end(); ++nit) {
            idx = 0;
            curx = 0;
            cury = 0;

            //initialize current net's pin's related data
            for(TplNets::pin_iterator pit=TplDB::db().nets.pin_begin(nit); pit!=TplDB::db().nets.pin_end(nit); ++pit) {
                TplModule &module =  TplDB::db().modules.module(pit->id);
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

}//namespace tpl


