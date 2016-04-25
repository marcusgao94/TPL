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
		vector<TplModule> shreddedCells;
        list<TplNet> shreddedNets;
		unsigned int rowHeight = TplDB::db().modules[0].height;
        unsigned int colWidth = rowHeight;
		for (unsigned int i = TplDB::db().modules.num_free();
			 	i < TplDB::db().modules.size(); i++) {
			TplModule module = TplDB::db().modules[i];
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
                    shreddedCells.push_back(cell);
                    // modify original nets
                    // ...


                    // add new cells to new nets
                    TplPin pin;
                    pin.id = id;
                    pin.io = IOType::Input;
                    pin.dx = 0;
                    pin.dy = 0;
                    // new net with left cell
                    if (n != 0) {
                        TplModule frontCell = shreddedCells[shreddedCells.size() - 2];
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
                        TplModule underCell = shreddedCells[shreddedCells.size() - rowNum];
                        TplPin frontPin;
                        frontPin.id = underCell.id;
                        frontPin.io = IOType::Input;
                        frontPin.dx = 0;
                        frontPin.dy = 0;
                        TplNet net;
                        net.id = module.id + "_col_" + to_string(m * rowNum + n);
                        net.degree = 2;
                        net.pins.push_back(frontPin);
                        net.pins.push_back(pin);
                        shreddedNets.push_back(net);
                    }
				}
			}
		}
	}


} // namespace tpl