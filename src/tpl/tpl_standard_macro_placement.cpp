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
		int rowHeight = TplDB::db().modules[0].height;
		for (int i = TplDB::db().modules.num_free();
			 	i < TplDB::db().modules.size(); i++) {
			TplModule module = TplDB::db().modules[i];
			int wideNum = (module.width - 1) / rowHeight + 1;
			int heightNum = (module.height - 1) / rowHeight + 1;
			for (int m = 0; m < wideNum; m++) {
				for (int n = 0; n < heightNum; n++) {
					//string id = module.id + "_" +
				}
			}
		}
	}


} // namespace tpl