#include "tpl_gy_algorithm.h"

#include <cassert>
#include <fstream>
#include <iterator>

namespace tpl {
    using namespace std;

    TplGYNetForceModel::~TplGYNetForceModel()
    {
    }

    TplGYThermalForceModel::TplGYThermalForceModel(TplThermalModelInterface *thermal_model) :
			TplStandardThermalForceModel(thermal_model)
    {
    }

	TplGYThermalForceModel::~TplGYThermalForceModel() { }

    TplGYAlgorithm::~TplGYAlgorithm()
    {
    }

    void TplGYAlgorithm::initialize_models()
    {
        net_force_model  = new TplGYNetForceModel;
    }

    vector<vector<TplModule*> > TplGYDetailPlacement::legalization() {
		_rowHeight = pdb.modules[0].height;
		_chipHeight = (int(pdb.modules.chip_height() / _rowHeight) + 1) * _rowHeight;
		_chipWidth = int(pdb.modules.chip_width()) + 1;
		int totalRows = _chipHeight / _rowHeight;

		// assign each module to the nearest legal row
		vector<vector<TplModule*>> rows(totalRows);
		for (TplModules::iterator iter = pdb.modules.begin();
			 	iter != pdb.modules.end(); iter++) {
			if (iter->fixed)
				continue;
			int belongRow = int(iter->y / _rowHeight);
			rows[belongRow].push_back(&*iter);
		}
		// select part of candidates that smaller than chip width
		for (int i = 0; i < totalRows; i++) {
			int modulesInThisRow = rows[i].size();
			double f[modulesInThisRow+1][_chipWidth+1] = {0.0};
			// 01 knapsack problem, dynamic programming
			for (int j = 1; j <= modulesInThisRow; j++) {
				// cost equals distance from illegal to legal
				int yTarget = i * _rowHeight;
				double cost = pow(yTarget - rows[i][j-1]->y, 2.0);
				// self-define function to make value negatively correlated to cost
				double value = exp(-cost);

				unsigned int width = rows[i][j]->width;
				for (int v = width; v <= _chipWidth; v++) {
					f[j][v] = max(f[j-1][v], f[j-1][v - width] + value);
				}
			}
			vector<TplModule*> select;
			vector<TplModule*> defer;
			int target = _chipWidth;
			for (int j = modulesInThisRow; j >= 1; j--) {
				if (f[j][target] > f[j-1][target]) {
					select.push_back(rows[i][j-1]);
					target -= rows[i][j-1]->width;
				} else {
					defer.push_back(rows[i][j - 1]);
				}

			}
			rows[i] = select;
			if (i < totalRows - 1)
				rows[i+1].insert(rows[i+1].end(), defer.begin(), defer.end());
		}
		return rows;
    }

	void TplGYDetailPlacement::detailPlacement(vector<vector<TplModule*>> rows) {
		// evenly disperse modules in each row
		for (int i = 0; i < rows.size(); i++) {
			if (rows[i].size() <= 1)
				continue;
			int widthOccupied = 0;
			for (vector<TplModule*>::iterator iter = rows[i].begin();
					iter != rows[i].end(); iter++) {
				widthOccupied += (*iter)->width;
			}
			int space = (_chipWidth - widthOccupied) / (rows[i].size() - 1);
			int location = 0;
			for (vector<TplModule*>::iterator iter = rows[i].begin();
					iter != rows[i].end(); iter++) {
				(*iter)->x = location;
				(*iter)->y = i * _rowHeight;
				location += (*iter)->width + space;
			}
		}
	}

}

