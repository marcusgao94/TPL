#include "tpl_standard_net_model.h"

#include <algorithm>

#ifndef NDEBUG
#include <cassert>
#include <iostream>
#endif

namespace tpl {

    using std::vector;
	using std::sort;
    using std::make_pair;

    void TplStandardNetModel::compute_net_weight(NetWeight &NWx, NetWeight &NWy)
	{
		//preconditions
		NWx.clear();
		NWy.clear();

//		vector<TplPin*> xpins;
//		vector<TplPin*> ypins;

		vector<PinPos> xpins;
		vector<PinPos> ypins;
		size_t degree = 0;

		const double DELTA = 0.001;

		for (TplNets::net_iterator nit=TplDB::db().nets.net_begin(); nit!=TplDB::db().nets.net_end(); ++nit) {
			degree = nit->pins.size();
			if ( degree < 2 ) continue;

			xpins.clear();
			ypins.clear();

			/////////////////////////////////////////////////////////////////////
			//get the current net's pins, and sort them by x and y separately
			for (TplNets::pin_iterator pit = TplDB::db().nets.pin_begin(nit);
				 pit != TplDB::db().nets.pin_end(nit); ++pit) {
				TplModule &module = TplDB::db().modules.module(pit->id);
				xpins.emplace_back(&(*pit), module.x + module.width /2.0 + pit->dx);
				ypins.emplace_back(&(*pit), module.y + module.height/2.0 + pit->dy);
			}

			assert(xpins.size() == degree);
			assert(ypins.size() == degree);

			sort(xpins.begin(), xpins.end(), compare_pin);
			sort(ypins.begin(), ypins.end(), compare_pin);
			/////////////////////////////////////////////////////////////////////

			/////////////////////////////////////////////////////////////////////
			//compute the net weight by B2B net model, ignore the pin on the boundary
			double x1 = xpins[0].pos;
			double x2 = xpins[degree-1].pos;

			double y1 = ypins[0].pos;
			double y2 = ypins[degree-1].pos;

			for (size_t i=1; i<degree-1; ++i) {
				//process by x
				double &x = xpins[i].pos;

				if (fabs(x-x1) > DELTA) {
					assert( NWx.count(make_pair(xpins[0].pin, xpins[i].pin)) == 0 );
					NWx[make_pair(xpins[0].pin, xpins[i].pin)] = 2.0 / (degree - 1) / (fabs(x-x1));
				}

				if (fabs(x-x2) > DELTA) {
					assert( NWx.count(make_pair(xpins[degree-1].pin, xpins[i].pin)) == 0 );
					NWx[make_pair(xpins[degree-1].pin, xpins[i].pin)] = 2.0 / (degree - 1) / (fabs(x-x2));
				}

				//process pins by y
				double &y = ypins[i].pos;

				if (fabs(y-y1) > DELTA) {
					assert( NWy.count(make_pair(ypins[0].pin, ypins[i].pin)) == 0 );
					NWy[make_pair(ypins[0].pin, ypins[i].pin)] = 2.0 / (degree - 1) / (fabs(y-y1));
				}

				if (fabs(y-y2) > DELTA) {
					assert( NWy.count(make_pair(ypins[degree-1].pin, ypins[i].pin)) == 0 );
					NWy[make_pair(ypins[degree-1].pin, ypins[i].pin)] = 2.0 / (degree - 1) / (fabs(y-y2));
				}
			}

			/////////////////////////////////////////////////////////////////////
		}//end net traversal
	}

    void TplStandardNetModel::update_shred_net_weight(NetWeight &NWx,
                                                      NetWeight &NWy, int k) {
		// calculate kth fibonacci
		int multiply = 0;
		if (k <= 1) multiply = 1;
		int a = 0, b = 1;
		for (int i = 2; i <= k; i++) {
			multiply = a + b;
			a = b;
			b = multiply;
		}
//		int i = 0;
		double DELTA = 0.00001;
        TplNets::net_iterator net_iter = TplDB::db().nets.net_begin();
        advance(net_iter, TplDB::db().nets.num_origin_nets());
        for (; net_iter != TplDB::db().nets.net_end(); net_iter++) {
			TplPin pin1 = net_iter->pins[0];
			TplPin pin2 = net_iter->pins[1];
			TplModule module1 = TplDB::db().modules.module(pin1.id);
			TplModule module2 = TplDB::db().modules.module(pin2.id);
			if ((module1.x - module2.x) > DELTA) {
				NWx[make_pair(&pin1, &pin2)] = multiply * 2.0 / (module1.x - module2.x);
			}
			if ((module1.y - module2.y > DELTA)) {
				NWy[make_pair(&pin1, &pin2)] = multiply * 2.0 / (module1.y - module2.y);
			}
		}
    }

	bool compare_pin(const PinPos &lhs, const PinPos &rhs)
	{
		const TplModule & m1 = TplDB::db().modules.module(lhs.pin->id);
		const TplModule & m2 = TplDB::db().modules.module(rhs.pin->id);

		return ( (m1.x + m1.width/2.0 + lhs.pin->dx) < (m2.x + m2.width/2.0 + rhs.pin->dx) );
	}

}//namespace tpl

