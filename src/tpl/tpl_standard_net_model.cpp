#include "tpl_standard_net_model.h"

#ifndef NDEBUG
#include <cassert>
#include <iostream>
#endif

namespace tpl {

    using std::vector;
    using std::make_pair;

    void TplStandardNetModel::compute_net_weight(NetWeight &NWx, NetWeight &NWy)
    {
        //preconditions
        NWx.clear();
        NWy.clear();

        //define pin related data
        vector<TplPin*> pins;
        vector<double> xs;
        vector<double> ys;

        double xmin = 0;
        double xmax = 0;
        double ymin = 0;
        double ymax = 0;

        int xmin_idx=-1, xmax_idx=-1, ymin_idx=-1, ymax_idx=-1;

        //local helper variables
        int idx=0;
        double curx=0, cury=0;
        int degree = 0;

        for(TplNets::net_iterator nit=TplDB::db().nets.net_begin(); nit!=TplDB::db().nets.net_end(); ++nit) {
            degree = nit->pins.size();
            if (degree < 2) continue;

            //restore local variable to initial state
            pins.clear();
            xs.clear();
            ys.clear();

            xmin = TplDB::db().modules.chip_width();
            xmax = -TplDB::db().modules.chip_width();
            ymin = TplDB::db().modules.chip_height();
            ymax = -TplDB::db().modules.chip_height();

            xmin_idx = -1;
            xmax_idx = -1;
            ymin_idx = -1;
            ymax_idx = -1;

            idx = 0;
            curx = 0;
            cury = 0;

            //initialize current net's pin's related data
            for (TplNets::pin_iterator pit = TplDB::db().nets.pin_begin(nit);
                 pit != TplDB::db().nets.pin_end(nit); ++pit) {
                TplModule &module = TplDB::db().modules.module(pit->id);
                pins.push_back(&(*pit));

                curx = module.x + pit->dx;
                if (curx < xmin) {
                    xmin = curx;
                    xmin_idx = idx;
                }
                if (curx > xmax) {
                    xmax = curx;
                    xmax_idx = idx;
                }
                xs.push_back(curx);

                cury = module.y + pit->dy;
                if (cury < ymin) {
                    ymin = cury;
                    ymin_idx = idx;
                }
                if (cury > ymax) {
                    ymax = cury;
                    ymax_idx = idx;
                }
                ys.push_back(cury);

                ++idx;
            }

            //loop to compute current pin pair set's non zero net weights
            //pin pair <==> net weight
            for (int cur_idx = 0; cur_idx < degree; ++cur_idx) {

                if (cur_idx != xmin_idx && cur_idx != xmax_idx) {
                    if (xs[cur_idx] != xmin) {
#ifndef NDEBUG
                        if(NWx.count(make_pair(pins[cur_idx], pins[xmin_idx])) != 0) {
                            std::cout << "x1 already exist " << pins[cur_idx] << " " << pins[xmin_idx] << std::endl;
                        }
                        if (pins[cur_idx] == pins[xmin_idx]) {
                            std::cout << " x1 pin equal " << pins[cur_idx]->id << std::endl;
                        }
                        assert(NWx.count(make_pair(pins[cur_idx], pins[xmin_idx])) == 0);
#endif
                        NWx[make_pair(pins[cur_idx], pins[xmin_idx])] = 2.0 / (degree - 1) * (xs[cur_idx] - xmin);
                    }

                    if (xs[cur_idx] != xmax) {
#ifndef NDEBUG
                        if(NWx.count(make_pair(pins[cur_idx], pins[xmax_idx])) != 0) {
                            std::cout << "x2 already exist " << pins[cur_idx] << " " << pins[xmax_idx] << std::endl;
                        }
                        if (pins[cur_idx] == pins[xmax_idx]) {
                            std::cout << " x2 pin equal " << pins[cur_idx]->id << std::endl;
                        }
                        assert(NWx.count(make_pair(pins[cur_idx], pins[xmax_idx])) == 0);
#endif
                        NWx[make_pair(pins[cur_idx], pins[xmax_idx])] = 2.0 / (degree - 1) * (xmax - xs[cur_idx]);
                    }
                }

                if (cur_idx != ymin_idx && cur_idx != ymax_idx) {
                    if (ys[cur_idx] != ymin) {
#ifndef NDEBUG
                        if(NWy.count(make_pair(pins[cur_idx], pins[ymin_idx])) != 0) {
                            std::cout << "y1 already exist " << pins[cur_idx] << " " << pins[ymin_idx] << std::endl;
                        }
                        if (pins[cur_idx] == pins[ymin_idx]) {
                            std::cout << " y1 pin equal " << pins[cur_idx]->id << std::endl;
                        }
                        assert(NWy.count(make_pair(pins[cur_idx], pins[ymin_idx])) == 0);
#endif
                        NWy[make_pair(pins[cur_idx], pins[ymin_idx])] = 2.0 / (degree - 1) * (ys[cur_idx] - ymin);
                    }

                    if (ys[cur_idx] != ymax) {
#ifndef NDEBUG
                        if(NWy.count(make_pair(pins[cur_idx], pins[ymax_idx])) != 0) {
                            std::cout << "y2 already exist " << pins[cur_idx] << " " << pins[ymax_idx] << std::endl;
                        }
                        if (pins[cur_idx] == pins[ymax_idx]) {
                            std::cout << " y2 pin equal " << pins[cur_idx]->id << std::endl;
                        }
                        assert(NWy.count(make_pair(pins[cur_idx], pins[ymax_idx])) == 0);
#endif
                        NWy[make_pair(pins[cur_idx], pins[ymax_idx])] = 2.0 / (degree - 1) * (ymax - ys[cur_idx]);
                    }
                }
            }

            if (xs[xmin_idx] != xs[xmax_idx]) {
#ifndef NDEBUG
                if((NWx.count(make_pair(pins[xmin_idx], pins[xmax_idx])) != 0)) {
                    std::cout << "xminmax already exist " << pins[xmin_idx]->id << " " << pins[xmax_idx]->id << std::endl;
                }
                if (pins[xmin_idx] == pins[xmax_idx]) {
                    std::cout << " xminmax pin equal " << nit->id << " " << pins[xmin_idx]->id << std::endl;
                }
                assert(NWx.count(make_pair(pins[xmin_idx], pins[xmax_idx])) == 0);
#endif
                NWx[make_pair(pins[xmin_idx], pins[xmax_idx])] = 2.0 / (degree - 1) * (xmax - xmin);
            }

            if (ys[ymin_idx] != ys[ymax_idx]) {
#ifndef NDEBUG
                if ((NWy.count(make_pair(pins[ymin_idx], pins[ymax_idx])) != 0)) {
                    std::cout << "yminmax already exist " << pins[ymin_idx]->id << " " << pins[ymax_idx]->id << std::endl;
                }
                if (pins[ymin_idx] == pins[ymax_idx]) {
                    std::cout << " yminmax pin equal " << nit->id << " " << pins[ymin_idx]->id << std::endl;
                }
                assert(NWy.count(make_pair(pins[ymin_idx], pins[ymax_idx])) == 0);
#endif
                NWy[make_pair(pins[ymin_idx], pins[ymax_idx])] = 2.0 / (degree - 1) * (ymax - ymin);
            }
        }
    }

}//namespace tpl

