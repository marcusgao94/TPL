#include "tpl_algorithm.h"

#include <cassert>
#include <fstream>
#include <iterator>
#include <utility>
#include <cmath>

#include <boost/filesystem.hpp>

#include <Eigen/Sparse>

#include "../bookshelf/bookshelf_node_parser.hpp"
#include "../bookshelf/bookshelf_pl_parser.hpp"
#include "../bookshelf/bookshelf_net_parser.hpp"
#include "../bookshelf/utils.h"


namespace tpl {
	using std::vector;
	using std::list;
	using std::deque;
	using std::map;
	using std::pair;
	using std::string;

	typedef Eigen::SparseMatrix<double> SpMat;

	TplAlgorithm::TplAlgorithm() : CHIP_WIDTH(0), CHIP_HEIGHT(0), NUM_FREE_MODULE(0), GRID_SIZE(0) {
	}

	bool TplAlgorithm::load_circuit(const std::string &_path) {
		using namespace boost::filesystem;
		using namespace thueda;
		using namespace std;

		try {
			path benchmark_path(_path);
			string benchmark_name = benchmark_path.filename().string();

			string storage;
			string::const_iterator iter, end;

			//parse .nodes file
			path node_file_path(benchmark_path);
			node_file_path /= benchmark_name + ".nodes";
			read_file(node_file_path.c_str(), storage);

			iter = storage.begin();
			end = storage.end();

			BookshelfNodes nodes;
			parse_bookshelf_node(iter, end, nodes);

			//parse .pl file
			path pl_file_path(benchmark_path);
			pl_file_path /= benchmark_name + ".pl";
			read_file(pl_file_path.c_str(), storage);

			iter = storage.begin();
			end = storage.end();

			BookshelfPls pls;
			parse_bookshelf_pl(iter, end, pls);

			//parse .nets file
			path net_file_path(benchmark_path);
			net_file_path /= benchmark_name + ".nets";
			read_file(net_file_path.c_str(), storage);

			iter = storage.begin();
			end = storage.end();

			BookshelfNets nets;
			parse_bookshelf_net(iter, end, nets);

			//clear old circuit data
			MODULES.clear();
			NETS.clear();

			NUM_FREE_MODULE = nodes.num_nodes - nodes.num_terminals;

			//scanning nodes
			deque<Id> tids;
			deque<Coordinate> txcs, tycs;
			deque<Length> twds, thts;
			deque<bool> tflgs;
			size_t cur_terminal_idx = NUM_FREE_MODULE;
			for (size_t i = 0; i < nodes.num_nodes; ++i) {

				const BookshelfNode &node = nodes.data[i];
				const BookshelfPl &pl = pls.data[i];
				assert(node.id == pl.id);

				if (node.fixed) {
					MODULE_ID_INDEX_MAP.insert(make_pair(node.id, cur_terminal_idx++));

					tids.push_back(node.id);
					txcs.push_back(pl.x);
					tycs.push_back(pl.y);
					twds.push_back(node.width);
					thts.push_back(node.height);
					tflgs.push_back(node.fixed);

				} else {
					MODULE_ID_INDEX_MAP.insert(make_pair(node.id, MODULES.ids.size()));

					MODULES.ids.push_back(node.id);
					MODULES.xcs.push_back(pl.x);
					MODULES.ycs.push_back(pl.y);
					MODULES.wds.push_back(node.width);
					MODULES.hts.push_back(node.height);
					MODULES.flgs.push_back(node.fixed);
				}

				//update chip width and height
				double right_border = pl.x + node.width;
				if (right_border > CHIP_WIDTH) CHIP_WIDTH = right_border;
				double top_border = pl.y + node.height;
				if (top_border > CHIP_HEIGHT) CHIP_HEIGHT = top_border;
			}

			MODULES.ids.insert(MODULES.ids.end(), tids.begin(), tids.end());
			MODULES.xcs.insert(MODULES.xcs.end(), txcs.begin(), txcs.end());
			MODULES.ycs.insert(MODULES.ycs.end(), tycs.begin(), tycs.end());
			MODULES.wds.insert(MODULES.wds.end(), twds.begin(), twds.end());
			MODULES.hts.insert(MODULES.hts.end(), thts.begin(), thts.end());
			MODULES.flgs.insert(MODULES.flgs.end(), tflgs.begin(), tflgs.end());
			//end scanning nodes

			/*
			//scanning nets and pins
			map<string, TplPin*> pin_add_map;
			NETS.pinstore.reserve(nets.num_pins);
			for(vector<BookshelfNet>::iterator nit=nets.data.begin(); nit!=nets.data.end(); ++nit) {
				TplNet net;
				net.id = nit->id;

				for(vector<BookshelfPin>::iterator pit=nit->pins.begin(); pit!=nit->pins.end(); ++pit) {
					if( pin_add_map.count(pit->id) == 0 ) {
						TplPin pin;
						pin.id = pit->id;
						pin.dx = pit->dx;
						pin.dy = pit->dy;

						pin_add_map.insert( make_pair(pin.id, &NETS.pinstore+NETS.pinstore.size()) );
						NETS.pinstore.push_back(pin);
					}
					net.pins.push_back( pin_add_map[pit->id] );
				}

				NETS.netlist.push_back(net);
			}
			//end scanning nets and pins

			GRID_SIZE = 100;
			 */


			computer_matrix(nets);


			return true;
		} catch (...) {
			return false;
		}
	}// TplAlgorithm::load_circuit

	void TplAlgorithm::computer_matrix(BookshelfNets nets) {
		using namespace Eigen;
		MatrixXd Cx(NUM_FREE_MODULE, NUM_FREE_MODULE), Cy(NUM_FREE_MODULE, NUM_FREE_MODULE);
		Cx = MatrixXd::Zero(NUM_FREE_MODULE, NUM_FREE_MODULE);
		Cy = MatrixXd::Zero(NUM_FREE_MODULE, NUM_FREE_MODULE);
		VectorXd dx(NUM_FREE_MODULE), dy(NUM_FREE_MODULE);
		dx = VectorXd::Zero(NUM_FREE_MODULE);
		dy = VectorXd::Zero(NUM_FREE_MODULE);
		for (vector<BookshelfNet>::iterator nit = nets.data.begin();
			 nit != nets.data.end(); nit++) {
			unsigned int p = nit->degree;
			for (int i = 0; i != nit->pins.size() - 1; i++) {
				for (int j = i + 1; j != nit->pins.size(); j++) {
					// find index of two modules in MODULES
					size_t index_a = MODULE_ID_INDEX_MAP[nit->pins[i].id];
					size_t index_b = MODULE_ID_INDEX_MAP[nit->pins[j].id];
					// if both modules are fixed, pass
					if (MODULES.flgs[index_a] && MODULES.flgs[index_b])
						continue;
					double weight_x = compute_weight(nit->pins[i], nit->pins[j], x, p);
					double weight_y = compute_weight(nit->pins[i], nit->pins[j], y, p);
					// if index_a is movable and index_b is fixed
					if (!MODULES.flgs[index_a] && MODULES.flgs[index_b]) {
						Cx(index_a, index_a) += weight_x;
						dx(index_a) -= weight_x * MODULES.xcs[index_b];
						Cy(index_a, index_a) += weight_y;
						dy(index_a) -= weight_x * MODULES.ycs[index_b];
					}
					// if index_a is fixed and index_b is movable
					else if (MODULES.flgs[index_a] && !MODULES.flgs[index_b]) {
						Cx(index_b, index_b) += weight_x;
						dx(index_b) -= weight_x * MODULES.xcs[index_a];
						Cy(index_b, index_b) += weight_y;
						dy(index_b) -= weight_y * MODULES.ycs[index_a];
					}
					//index_a and index_b both are movable
					else {
						Cx(index_a, index_a) += weight_x;
						Cx(index_b, index_b) += weight_x;
						Cx(index_a, index_b) -= weight_x;
						Cx(index_b, index_a) -= weight_x;
						Cy(index_a, index_a) += weight_y;
						Cy(index_b, index_b) += weight_y;
						Cy(index_a, index_b) -= weight_y;
						Cy(index_b, index_a) -= weight_y;
					}
				}
			}
		}
	}

	double TplAlgorithm::compute_weight(BookshelfPin pinA, BookshelfPin pinB, Coord coordinate, unsigned int p) {
		size_t index_a = MODULE_ID_INDEX_MAP[pinA.id]; // index in MODULES of moduleA
		size_t index_b = MODULE_ID_INDEX_MAP[pinB.id]; // index in MODULES of moduleB
		if (index_a > index_b)
			std::swap(index_a, index_b);
		// use MODULE_WEIGHT to record weight of two modules
		pair<size_t, size_t> pa = std::make_pair(index_a, index_b);
		NetWeight::iterator it;
		if (coordinate == x) {
			it = MODULE_WEIGHT_X.find(pa);
			if (it != MODULE_WEIGHT_X.end())
				return it->second;
		}
		else {
			it = MODULE_WEIGHT_Y.find(pa);
			if (it != MODULE_WEIGHT_Y.end())
				return it->second;
		}
		double weight;
		if (!is_boundary(index_a, coordinate) &&
			!is_boundary(index_b, coordinate)) {
			weight = 0.0;
		}
		else {
			if (coordinate == x) {
				double pinA_x = MODULES.xcs[index_a] + pinA.dx;
				double pinB_x = MODULES.xcs[index_b] + pinB.dx;
				weight = 2.0 / (p - 1) / std::abs(pinA_x - pinB_x);
				MODULE_WEIGHT_X.insert(std::make_pair(pa, weight));
			}
			else {
				double pinA_y = MODULES.ycs[index_a] + pinA.dy;
				double pinB_y = MODULES.ycs[index_b] + pinB.dy;
				weight = 2.0 / (p - 1) / std::abs(pinA_y - pinB_y);
				MODULE_WEIGHT_Y.insert(std::make_pair(pa, weight));
			}
		}
		return weight;
	}

	bool TplAlgorithm::is_boundary(size_t idx, Coord coordinate) {
		if (coordinate == x) {
			if ((MODULES.xcs[idx] == 0.0) ||
				(MODULES.xcs[idx] + MODULES.wds[idx] == CHIP_WIDTH))
				return true;
			else
				return false;
		}
		else {
			if ((MODULES.ycs[idx] == 0.0) ||
				(MODULES.ycs[idx] + MODULES.hts[idx] == CHIP_HEIGHT))
				return true;
			else
				return false;
		}
	}

	void TplAlgorithm::initial_placement() {


	}//end TplAlgorithm::initial_placement

	void TplAlgorithm::compute_net_force_target(vector<double> &x_target, vector<double> &y_target) {
		/*
		MatrixXd Cx(NUM_FREE_MODULE, NUM_FREE_MODULE), Cy(NUM_FREE_MODULE, NUM_FREE_MODULE);
		Cx = MatrixXd::Zero(NUM_FREE_MODULE, NUM_FREE_MODULE);
		Cy = MatrixXd::Zero(NUM_FREE_MODULE, NUM_FREE_MODULE);
		VectorXd dx(NUM_FREE_MODULE), dy(NUM_FREE_MODULE);
		dx = VectorXd::Zero(NUM_FREE_MODULE);
		dy = VectorXd::Zero(NUM_FREE_MODULE);

		NetWeight x_net_weight, y_net_weight;
		for(vector<TplNet>::iterator nit=NETS.begin(); nit!=NETS.end(); ++nit) {
			compute_net_weight(*nit, x_net_weight, y_net_weight);
		}

		for(NetWeight::iterator it=x_net_weight.begin(); it!=x_net_weight.end(); ++it) {
			const unsigned int &idx1   = MODULE_ID_INDEX_MAP[it->first.first ];
			const unsigned int &idx2   = MODULE_ID_INDEX_MAP[it->first.second];
			const double       &weight = it->second;

			if(MODULES[idx1].fixed && MODULES[idx2].fixed) {
				continue;
			} else if(MODULES[idx1].fixed) {
				Cx(idx2, idx2) += weight;
				dx(idx2)       -= weight;
			} else if(MODULES[idx2].fixed) {
				Cx(idx1, idx1) += weight;
				dx(idx1)       -= weight;
			} else {
				Cx(idx1, idx1) += weight;
				Cx(idx2, idx2) += weight;
				Cx(idx1, idx2) -= weight;
				Cx(idx2, idx1) -= weight;
			}
		}

		for(NetWeight::iterator it=y_net_weight.begin(); it!=y_net_weight.end(); ++it) {
			const unsigned int &idx1   = MODULE_ID_INDEX_MAP[it->first.first ];
			const unsigned int &idx2   = MODULE_ID_INDEX_MAP[it->first.second];
			const double       &weight = it->second;

			if(MODULES[idx1].fixed && MODULES[idx2].fixed) {
				continue;
			} else if(MODULES[idx1].fixed) {
				Cy(idx2, idx2) += weight;
				dy(idx2)       -= weight;
			} else if(MODULES[idx2].fixed) {
				Cy(idx1, idx1) += weight;
				dy(idx1)       -= weight;
			} else {
				Cy(idx1, idx1) += weight;
				Cy(idx2, idx2) += weight;
				Cy(idx1, idx2) -= weight;
				Cy(idx2, idx1) -= weight;
			}
		}

		VectorXd eigen_x_target = Cx.llt().solve(dx);
		x_target.resize(eigen_x_target.size());
		VectorXd::Map(&x_target[0], eigen_x_target.size()) = eigen_x_target;

		VectorXd eigen_y_target = Cy.llt().solve(dy);
		y_target.resize(eigen_y_target.size());
		VectorXd::Map(&y_target[0], eigen_y_target.size()) = eigen_y_target;
		 */

	}//end TplAlgorithm::compute_net_force_target


	void TplAlgorithm::compute_net_weight(const TplNet &net, NetWeight &x_net_weight, NetWeight &y_net_weight) {
		/*
		//define aliases
		const boost::ptr_vector<TplPin> &pins = net.pins;
		const unsigned int &degree = net.pins.size();

		//prepare data
		double xmin=CHIP_WIDTH, xmax=-1, ymin=CHIP_HEIGHT, ymax=-1;
		size_t xmin_idx, xmax_idx, ymin_idx, ymax_idx;
		vector<double> xs, ys;
		for(size_t i=0; i<degree; ++i) {
			TplModule &module = MODULES[ MODULE_ID_INDEX_MAP[pins[i].id] ];

			if       (module.x < xmin) {
				xmin = module.x;
				xmin_idx = i;
			} else if(module.x > xmax) {
				xmax = module.x;
				xmax_idx = i;
			}
			xs.push_back(module.x);

			if       (module.y < ymin) {
				ymin = module.y;
				ymin_idx = i;
			} else if(module.y > ymax) {
				ymax = module.y;
				ymax_idx = i;
			}
			ys.push_back(module.y);
		}

		double min_weight, max_weight;
		size_t cur_idx;
		//scan xs
		for(cur_idx=0; cur_idx<degree; ++cur_idx) {
			if(cur_idx == xmin_idx || cur_idx == xmax_idx ) continue;

			if( xs[cur_idx] == xmin ) {
				min_weight = 0;
				max_weight = 2.0/(degree-1)*(xmax-xmin);
			} else if( xs[cur_idx] == xmax ) {
				min_weight = 2.0/(degree-1)*(xmax-xmin);
				max_weight = 0;
			} else {
				min_weight = 2.0/(degree-1)*(xs[cur_idx]-xmin);
				max_weight = 2.0/(degree-1)*(xmax-xs[cur_idx]);
			}
			assert( x_net_weight.count(    make_pair(pins[cur_idx].id, pins[xmin_idx].id) ) == 0 );
			x_net_weight.insert( make_pair(make_pair(pins[cur_idx].id, pins[xmin_idx].id), min_weight) );
			assert( x_net_weight.count(    make_pair(pins[cur_idx].id, pins[xmax_idx].id) ) == 0 );
			x_net_weight.insert( make_pair(make_pair(pins[cur_idx].id, pins[xmax_idx].id), max_weight) );
		}//end scan xs

		//scan ys
		for(cur_idx=0; cur_idx<degree; ++cur_idx) {
			if(cur_idx == ymin_idx || cur_idx == ymax_idx ) continue;

			if( ys[cur_idx] == ymin ) {
				min_weight = 0;
				max_weight = 2.0/(degree-1)*(ymax-ymin);
			} else if( ys[cur_idx] == ymax ) {
				min_weight = 2.0/(degree-1)*(ymax-ymin);
				max_weight = 0;

			} else {
				min_weight = 2.0/(degree-1)*(ys[cur_idx]-ymin);
				max_weight = 2.0/(degree-1)*(ymax-ys[cur_idx]);
			}
			assert( y_net_weight.count(    make_pair(pins[cur_idx].id, pins[ymin_idx].id) ) == 0 );
			y_net_weight.insert( make_pair(make_pair(pins[cur_idx].id, pins[ymin_idx].id), min_weight) );
			assert( y_net_weight.count(    make_pair(pins[cur_idx].id, pins[ymin_idx].id) ) == 0 );
			y_net_weight.insert( make_pair(make_pair(pins[cur_idx].id, pins[ymax_idx].id), max_weight) );
		}//end scan ys
		 */

	}//end TplAlgorithm::get_current_net_weight

//private routines
	bool TplAlgorithm::read_file(const char *file_name, std::string &storage) {
		using namespace std;

		ifstream in(file_name, ios_base::in);
		in.unsetf(ios::skipws);

		storage.clear();
		copy(istream_iterator<char>(in), istream_iterator<char>(), back_inserter(storage));

		in.close();

		return in.good();
	}// TplAlgorithm::read_file

/*
void TplAlgorithm::compute_net_weight(const std::vector<double> &pos, NetWeight &net_weight)
{
}
*/

}//end namespace tpl

