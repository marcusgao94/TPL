#include "tpl_standard_algorithm.h"

#define checkStatus(status, message) \
if (status) { \
    cout << "error in " << message << endl; \
    return; \
}

namespace tpl {
    using namespace std;

    void TplStandardAlgorithm::initialize_models()
    {
        _net_model           = shared_ptr<TplAbstractNetModel>(         new TplStandardNetModel());
        _net_force_model     = shared_ptr<TplAbstractNetForceModel>(    new TplStandardNetForceModel());
        _thermal_model       = shared_ptr<TplAbstractThermalModel>(     new TplStandardThermalModel());
        //_thermal_force_model = shared_ptr<TplAbstractThermalForceModel>(new TplStandardThermalForceModel(_thermal_model));
    }

    void TplStandardAlgorithm::shred() {
        // list<TplNet> shreddedNets;
		// find shredded cells of a module through the module id
		map<Id, vector<TplModule>> macro_cells;
        list<TplNet> shreddedNets;
		unsigned int rowHeight = TplDB::db().modules[0].height;
        unsigned int colWidth = rowHeight;
        // iterate over macros
		for (TplModules::iterator macro_iter = TplDB::db().modules.begin() + TplDB::db().modules.num_free();
                macro_iter != TplDB::db().modules.end(); macro_iter++) {
			int rowNum = (macro_iter->height - 1) / rowHeight + 1;
            int colNum = (macro_iter->width - 1) / colWidth + 1;
            //int colNum = 1;
            vector<TplModule> cells;
			for (int m = 0; m < rowNum; m++) {
				for (int n = 0; n < colNum; n++) {
                    // create new small cells
					Id id = macro_iter->id + "_" + to_string(m * colNum + n);
					Coordinate x = macro_iter->x + n * colWidth;
                    Coordinate y = macro_iter->y + m * rowHeight;
                    //Length width = colWidth;
                    Length width = macro_iter->width;
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
                    // new net with under cell
                    if (m != 0) {
                        TplModule underCell = cells[cells.size() - 1 - colNum];
                        TplPin underPin;
                        underPin.id = underCell.id;
                        underPin.io = IOType::Input;
                        underPin.dx = 0;
                        underPin.dy = 0;
                        TplNet net;
                        net.id = macro_iter->id + "_row_" + to_string(m * colNum + n);
                        net.degree = 2;
                        net.pins.push_back(underPin);
                        net.pins.push_back(pin);
                        shreddedNets.push_back(net);
                    }

                    // new net with left cell
                    if (n != 0) {
                        TplModule frontCell = cells[cells.size() - 2];
                        TplPin frontPin;
                        frontPin.id = frontCell.id;
                        frontPin.io = IOType::Input;
                        frontPin.dx = 0;
                        frontPin.dy = 0;
                        TplNet net;
                        net.id = macro_iter->id + "_col_" + to_string(m * rowNum + n);
                        net.degree = 2;
                        net.pins.push_back(frontPin);
                        net.pins.push_back(pin);
                        shreddedNets.push_back(net);
                    }
				}
			}
            macro_cells[macro_iter->id] = cells;
		}

        // back up original nets
        TplDB::db().nets.backup_net();

        // modify original nets
        for (list<TplNet>::iterator net_iter = TplDB::db().nets.net_begin();
                 net_iter != TplDB::db().nets.net_end(); net_iter++) {

            // iterate over original pins, exclude pins of new added cells
            const int len = net_iter->pins.size();
            // net_iter->pins[p] is pin on a macro, and i is used to control loop times
            for (int i = 0, p = 0; i < len; i++) {
                TplModule module = TplDB::db().modules.module(net_iter->pins[p].id);
                if (!module.fixed) {
                    p++;
                    continue;
                }

                // if this net contains a macro, then add new shredded cells to this net
                vector<TplModule> cells = macro_cells[module.id];
                for (TplModules::iterator cell_iter = cells.begin();
                        cell_iter != cells.end(); cell_iter++) {
                    TplPin pin;
                    pin.id = cell_iter->id;
                    pin.io = IOType::Input;
                    pin.dx = 0;
                    pin.dy = 0;
                    net_iter->pins.push_back(pin);
                }

                // delete original macro from this net, since next element become i, need to i--
                net_iter->pins.erase(net_iter->pins.begin() + p);
            }
        }

        // delete macros and add shredded cells
        TplDB::db().modules.add_shredded_cells(macro_cells);

        // add new nets between shredded cells
        TplDB::db().nets.add_net(shreddedNets);
    }

    void TplStandardAlgorithm::aggregate() {
        TplDB::db().modules.aggregate_cells();
        TplDB::db().nets.delete_net();
    }

    void TplStandardAlgorithm::make_initial_placement()
    {
        vector<double> x_target, y_target;
        NetWeight NWx, NWy;

        _net_model->compute_net_weight(NWx, NWy);

        for(size_t i=0; i<5; ++i) {
            _net_force_model->compute_net_force_target(NWx, NWy, x_target, y_target);
            TplDB::db().modules.set_free_module_coordinates(x_target, y_target);

            x_target.clear();
            x_target.reserve(TplDB::db().modules.num_free());

            y_target.clear();
            y_target.reserve(TplDB::db().modules.num_free());
        }
    }

    void TplStandardAlgorithm::make_global_placement()
    {

    }

    void TplStandardAlgorithm::initialize_move_force_matrix()
    {
        vector<SpElem> coefficients;
        unsigned int num_free = TplDB::db().modules.num_free();
        vector<double> module_power;

        double avg_power = 0;
        double power;
        for(TplModules::iterator it=TplDB::db().modules.begin(); it!=TplDB::db().modules.end(); ++it) {
            power = it->width * it->height * it->power_density;
            avg_power +=  power;

            if(!it->fixed) module_power.push_back(power);
        }
        avg_power /= TplDB::db().modules.size();

        for(size_t i=0; i<module_power.size(); ++i) {
            coefficients.push_back(SpElem(i, i, module_power[i]/avg_power/num_free));
        }

        Cx0.setFromTriplets(coefficients.begin(), coefficients.end());
        Cy0.setFromTriplets(coefficients.begin(), coefficients.end());
    }

    void TplStandardAlgorithm::update_move_force_matrix()
    {

    }

    void TplStandardAlgorithm::saveDEF(string benchmark) {
        string name = benchmark + "_gp.def";
        FILE* fout = fopen(name.c_str(), "w");
        int version1 = 5, version2 = 8;
        const char* caseSensitive = "";
        const char* dividerChar = ":";
        const char* busBitChars = "[]";
        const char* designName = benchmark.c_str();
        int res = defwInit(fout, version1, version2, caseSensitive, dividerChar,
                           busBitChars, designName, NULL, NULL, NULL, -1);
        /*
        checkStatus(res, "init");
        res = defwVersion(version1, version2);
        checkStatus(res, "version");
        res = defwDividerChar(dividerChar);
        checkStatus(res, "dividerChar");
        res = defwBusBitChars(busBitChars);
        checkStatus(res, "busBitChars");
        res = defwDesignName(designName);
        checkStatus(res, "designName");
         */
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

    void TplStandardAlgorithm::make_detail_placement(string benchmark) {
        const double density = 0.7;
        const double misplacement = 150.0;
        string tech_file = benchmark + ".aux.lef";
        string cell_file = benchmark + ".aux.lef";
        // string floorplan_file = benchmark + ".aux.def";
        string floorplan_file = benchmark + "_gp.def";
        string placed_file = benchmark + "_gp.def";
        rippledp_read((char*)tech_file.c_str(), (char*)cell_file.c_str(),
                      (char*)floorplan_file.c_str(), (char*)placed_file.c_str());
        rippledp(density, misplacement);
    }

}//end namespace tpl

