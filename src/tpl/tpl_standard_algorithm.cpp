#include "tpl_standard_algorithm.h"

#define checkStatus(status, message) \
if (status) { \
    cout << "error in " << message << endl; \
    return; \
}

namespace tpl {
    using namespace std;

    TplStandardAlgorithm::TplStandardAlgorithm()
    {
       initialize_models();

        const unsigned &msize = TplDB::db().modules.num_free();

        Cx  = SpMat(msize, msize);
        Cy  = SpMat(msize, msize);

        Cx0 = SpMat(msize, msize);
        Cy0 = SpMat(msize, msize);

        HFx.resize(msize);
        HFy.resize(msize);
    }

    void TplStandardAlgorithm::control(std::string path, bool mmp) {
        double t0 = double(clock());
        TplDB::db().load_circuit(path);
        double t1 = double(clock());
        printf("load circuit finish, took %.3lf seconds\n", (t1 - t0) / CLOCKS_PER_SEC);
        if (mmp) {
            shred();
            double t2 = double(clock());
            printf("shred finish, took %.3lf seconds\n", (t2 - t1) / CLOCKS_PER_SEC);
        }
        double t3 = double(clock());
        make_initial_placement();
        double t4 = double(clock());
        printf("initial placement finish, took %.3lf seconds\n", (t4 - t3) / CLOCKS_PER_SEC);
        if (mmp) {
            aggregate();
            double t5 = double(clock());
            printf("aggregate finish, took %.3lf seconds\n", (t5 - t4) / CLOCKS_PER_SEC);
        }
        /*
        double t6 = double(clock());
        make_global_placement();
        double t7 = double(clock());
        printf("global placement finish, took %.3lf seconds\n", (t7 - t6) / CLOCKS_PER_SEC);
        // make_detail_placement();
         */

    }

    void TplStandardAlgorithm::initialize_models()
    {
        _net_model           = make_shared<TplStandardNetModel>();
        _net_force_model     = make_shared<TplStandardNetForceModel>();
        _thermal_force_model = make_shared<TplStandardThermalForceModel>();
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
        const unsigned &msize = TplDB::db().modules.num_free();

        initialize_move_force_matrix();//compute Cx0 and Cy0
        VectorXd dx(msize), dy(msize);
        VectorXd rhsx(msize), rhsy(msize);

        while (!terminate.shouldStop()) {
            _net_model->compute_net_weight(NWx, NWy);
            _net_force_model->compute_net_force_matrix(NWx, NWy, Cx, Cy, dx, dy);//compute Cx and Cy
            _thermal_force_model->compute_heat_flux_vector(HFx, HFy);//Compute HFx and HFy

            LLTSolver llt;

            rhsx = Cx0*HFx*-1;
            rhsy = Cy0*HFy*-1;
            VectorXd delta_x = llt.compute(Cx+Cx0).solve(rhsx);
            VectorXd delta_y = llt.compute(Cy+Cy0).solve(rhsy);


            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //the following code breaks the incapsulation, but more quickly, please note!!!
            for (unsigned i=0; i<msize; ++i) {
                TplDB::db().modules[i].x += delta_x(i);
                TplDB::db().modules[i].y += delta_y(i);
            }

            update_move_force_matrix(delta_x, delta_y, _thermal_force_model->get_mu());//udpate Cx0 and Cy0
        }

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

    void TplStandardAlgorithm::update_move_force_matrix(const VectorXd &delta_x, const VectorXd &delta_y, double mu)
    {
        const unsigned &msize = TplDB::db().modules.num_free();

        double avg_mu = 0;
        for (unsigned int i=0; i<msize; ++i) {
            avg_mu = sqrt( pow(delta_x(i), 2) + pow(delta_y(i), 2) );
        }
        avg_mu /= msize;

        double k = 1 + tanh(log(mu/avg_mu));

        Cx0 *= k;
        Cy0 *= k;
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


    bool Terminate::shouldStop() {
        using namespace std;

        const double STOP = 0.2;

        unsigned long A = 0; // total area of all modules

        segments.clear();
        nodes.clear();
        pos.clear();

        // calculate total area and initialize segments and nodes
        for (unsigned i = 0; i < TplDB::db().modules.size(); i++) {
            TplModule module = TplDB::db().modules[i];
            A += module.width * module.height;

            // push bottom y of a module
            Segment s(module.x, module.x + module.width, module.y, 1);
            segments.push_back(s);
            // push top y of a module
            s.y += module.height;
            s.flag = -1;
            segments.push_back(s);

            // map x to pos[index]
            pos.push_back(module.x);
            pos.push_back(module.x + module.width);
        }

        // sort segments
        sort(segments.begin(), segments.end());
        // delete completely overlapping modules
        segments.erase(unique(segments.begin(), segments.end()), segments.end());

        // sort pos
        sort(pos.begin(), pos.end());
        // delete same x, map x to pos[0] to pos[m-1]
        pos.erase(unique(pos.begin(), pos.end()), pos.end());
        int m = pos.size();

        // leaf number = m => nodes number = m + m/2 + ... + 1 = 2m - 1
        nodes.resize(2 * m - 1);
        // build segment tree
        build(0, m - 1, 0);

        // modules area union
        unsigned long res = 0;
        for (size_t i = 0; i < segments.size() - 1; i++) {
            int low = binarySearch(0, m - 1, segments[i].x1);
            int high = binarySearch(0, m - 1, segments[i].x2);
            if (low == -1 || high == -1) {
                cout << "error i = " << i << endl;
                break;
            }
            update(low, high, segments[i].flag, 0);
            if (segments[i].y != segments[i+1].y) {
                res += nodes[0].len * (segments[i+1].y - segments[i].y);
            }
        }

        cout << "union area = " << res << endl;
        cout << "total area = " << A << endl;

        double u = 1 - double(res) / A;
        cout << "u = " << u << endl;
        return (u < STOP);
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
        pushup(tidx);
    }


    int Terminate::binarySearch(int low, int high, double target) {
        const double DELTA = 0.00001;

        while (low <= high) {
            int mid = (low + high) / 2;
            if (abs(pos[mid] - target) < DELTA) return mid;
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

}//end namespace tpl

