//
// Created by marcusgao on 16/4/24.
//

#include "tpl_standard_macro_placement.h"

#ifndef NDEBUG
#include <iostream>
#include <string>
#include <cstring>
#endif

namespace tpl {

	using namespace std;

	void TplStandardMacroPlacement::shred() {
        list<TplNet> shreddedNets;
		// find shredded cells of a module through the module id
		unordered_map<Id, vector<TplModule>> map;
		unsigned int rowHeight = TplDB::db().modules[0].height;
        unsigned int colWidth = rowHeight;
		for (unsigned int i = TplDB::db().modules.num_free();
			 	i < TplDB::db().modules.size(); i++) {
			TplModule module = TplDB::db().modules[i];
			vector<TplModule> cells;
			int rowNum = (module.width - 1) / colWidth + 1;
			int colNum = (module.height - 1) / rowHeight + 1;
			for (int m = 0; m < rowNum; m++) {
				for (int n = 0; n < colNum; n++) {
                    // create new small cells
					Id id = module.id + "_" + to_string(m * rowNum + n);
					Coordinate x = module.x + n * colWidth;
                    Coordinate y = module.y + m * rowHeight;
                    Length width = colWidth;
                    Length height = rowHeight;
                    bool fixed = false;
                    double density = 1.0;
                    TplModule cell(id, x, y, width, height, fixed, density);
					cells.push_back(cell);

                    // add new cells to new nets
                    TplPin pin;
                    pin.id = id;
                    pin.io = IOType::Input;
                    pin.dx = 0;
                    pin.dy = 0;
                    // new net with left cell
                    if (n != 0) {
                        TplModule frontCell = cells[cells.size() - 2];
                        TplPin frontPin;
                        frontPin.id = frontCell.id;
                        frontPin.io = IOType::Input;
                        frontPin.dx = 0;
                        frontPin.dy = 0;
                        TplNet net;
                        net.id = module.id + "_row_" + to_string(m * rowNum + n);
                        net.degree = 2;
                        net.pins.push_back(frontPin);
                        net.pins.push_back(pin);
                        shreddedNets.push_back(net);
                    }
                    // new net with under cell
                    if (m != 0) {
                        TplModule underCell = cells[cells.size() - rowNum];
                        TplPin underPin;
                        underPin.id = underCell.id;
                        underPin.io = IOType::Input;
                        underPin.dx = 0;
                        underPin.dy = 0;
                        TplNet net;
                        net.id = module.id + "_col_" + to_string(m * rowNum + n);
                        net.degree = 2;
                        net.pins.push_back(underPin);
                        net.pins.push_back(pin);
                        shreddedNets.push_back(net);
                    }
				}
			}
			map.insert(make_pair(module.id, cells));
		}

		// modify original nets
		for (list<TplNet>::iterator iter = TplDB::db().nets.net_begin();
			 	iter != TplDB::db().nets.net_end(); iter++) {
			vector<TplPin>::iterator it = iter->pins.begin();
			int cnt = 0, limit = iter->pins.size();
			// iterate over original pins, exclude pins of new added cells
			while (it != iter->pins.end() && cnt < limit) {
				cnt++;
				size_t index = TplDB::db().modules.module_index(it->id);
				TplModule module = TplDB::db().modules[index];
				if (!module.fixed) {
					it++;
					continue;
				}
				// if this net contains a macro,
				// then add new shredded cells to this net
				for (vector<TplModule>::iterator cell = map[module.id].begin();
						cell != map[module.id].end(); cell++) {
					TplPin pin;
					pin.id = cell->id;
					pin.io = IOType::Input;
					pin.dx = 0;
					pin.dy = 0;
					iter->pins.push_back(pin);
				}
				// delete original macro from this net and move it to next
				it = iter->pins.erase(it);
			}
		}

		// add new cells
		TplDB::db().modules.add_cells(map);
		// add new nets
		TplDB::db().nets.add_net(shreddedNets);
	}


} // namespace tpl