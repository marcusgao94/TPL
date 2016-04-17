#include "tpl_standard_detail_placement.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>

using namespace std;

#ifndef NDEBUG
#endif

#define checkStatus(status, process) \
if (status != 0) { \
    cout << "error in " << process << endl; \
    return ; \
}

namespace tpl {


    void TplStandardDetailPlacement::saveDEF(string benchmark) {
		string name = benchmark + "_gp.def";
        FILE* fout = fopen(name.c_str(), "w");
        int version1 = 5, version2 = 7;
        const char* caseSensitive = "";
        const char* dividerChar = ":";
        const char* busBitChars = "[]";
        const char* designName = benchmark.c_str();
        int res = defwInit(fout, version1, version2, caseSensitive, dividerChar,
                           busBitChars, designName, NULL, NULL, NULL, -1);
        checkStatus(res, "init");
        res = defwVersion(version1, version2);
        checkStatus(res, "version");
        res = defwDividerChar(dividerChar);
        checkStatus(res, "dividerChar");
        res = defwBusBitChars(busBitChars);
        checkStatus(res, "busBitChars");
        res = defwDesignName(designName);
        checkStatus(res, "designName");
        res = defwStartComponents(TplDB::db().modules.size());
        checkStatus(res, "start components");
        for (TplModules::iterator iter = TplDB::db().modules.begin();
                iter != TplDB::db().modules.end(); iter++) {
            res = defwComponent(iter->id.c_str(), "module", 0, NULL, NULL, NULL,
                    NULL, NULL, 0, NULL, NULL, NULL, NULL, "PLACED",
                    int(iter->x), int(iter->y), 0, -1.0, NULL, 0, 0, 0, 0);
            checkStatus(res, "component");
        }
        res = defwEndComponents();
        checkStatus(res, "end components");
        res = defwEnd();
        checkStatus(res, "end");
        fclose(fout);
    }

    void TplStandardDetailPlacement::detailPlacement(string benchmark) {
		const double density = 0.7;
		const double misplacement = 150.0;
        string tech_file = benchmark + ".aux.lef";
		string cell_file = benchmark + ".aux.lef";
		string floorplan_file = benchmark + ".aux.def";
		string placed_file = benchmark + "_gp.def";
		rippledp_read((char*)tech_file.c_str(), (char*)cell_file.c_str(),
					  (char*)floorplan_file.c_str(), (char*)placed_file.c_str());
		rippledp(density, misplacement);
    }

    vector<vector<TplModule*> > TplStandardDetailPlacement ::legalization() {
        _rowHeight = TplDB::db().modules[0].height;
        _chipHeight = (int(TplDB::db().modules.chip_height() / _rowHeight) + 1) * _rowHeight;
        _chipWidth = int(TplDB::db().modules.chip_width()) + 1;
        int totalRows = _chipHeight / _rowHeight;

        // assign each module to the nearest legal row
        vector<vector<TplModule*>> rows(totalRows);
        for (TplModules::iterator iter = TplDB::db().modules.begin();
             iter != TplDB::db().modules.end(); iter++) {
            if (iter->fixed)
                continue;
            int belongRow = int(iter->y / _rowHeight);
            rows[belongRow].push_back(&*iter);
        }
        // select part of candidates that smaller than chip width
        for (int i = 0; i < totalRows; i++) {
            int modulesInThisRow = rows[i].size();
			vector<vector<double> > f(modulesInThisRow + 1, vector<double>(0, _chipWidth + 1));

            // double f[modulesInThisRow+1][_chipWidth+1] = {0.0};
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
            // find which module is selected in each line when total cost is least
            vector<TplModule*> select;
            vector<TplModule*> defer;
            int target = _chipWidth;
            for (int j = modulesInThisRow; j >= 1; j--) {
                // f[j][target] == f[j-1][target - width[j]] + value[j] > f[j-1][target] indicates that the jth
                // module is selected, in this case the jth module is rows[i][j-1] because f[0] is additional
                if (f[j][target] > f[j-1][target]) {
                    select.push_back(rows[i][j-1]);
                    target -= rows[i][j-1]->width;
                } else {
                    defer.push_back(rows[i][j - 1]);
                }

            }
            rows[i] = select;
            // add deferred modules to next line
            if (i < totalRows - 1)
                rows[i+1].insert(rows[i+1].end(), defer.begin(), defer.end());
        }
        return rows;
    }

    void TplStandardDetailPlacement::detailPlacement(vector<vector<TplModule*>> rows) {
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

}//namespace tpl
