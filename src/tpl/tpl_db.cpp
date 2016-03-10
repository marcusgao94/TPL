#include <fstream>
#include <string>
#include <utility>
#include <cmath>

#include <boost/filesystem.hpp>

#include "../bookshelf/bookshelf_node_parser.hpp"
#include "../bookshelf/bookshelf_pl_parser.hpp"
#include "../bookshelf/bookshelf_net_parser.hpp"
#include "../bookshelf/bookshelf_pl_generator.hpp"

#include "tpl_db.h"

namespace tpl {
    using namespace std;
    using namespace thueda;

    TplModules::TplModules(const BookshelfNodes &bnodes, const BookshelfPls &bpls) :
            _num_modules(bnodes.num_nodes),
            _num_free(bnodes.num_nodes - bnodes.num_terminals),
            _chip_width(0),
            _chip_height(0)
    {
        assert(bnodes.data.size() == bpls.data.size());

        for(size_t i=0; i<bnodes.data.size(); ++i) {
            const BookshelfNode &bnode = bnodes.data[i];
            const BookshelfPl   &bpl   = bpls.data[i];

            TplModule m = {bnode.id, bpl.x, bpl.y, bnode.width, bnode.height, bnode.fixed, 1};
            _modules.push_back(std::move(m));
            _id_index_map.insert( make_pair(bnode.id, i) );

            double right_border = bpl.x + bnode.width;
            if(right_border>_chip_width) _chip_width  = right_border;
            double top_border = bpl.y + bnode.height;
            if(top_border>_chip_height)  _chip_height = top_border;
        }
    }

    TplModules::TplModules(TplModules &&temp) :
            _num_modules(std::move(temp._num_modules)),
            _num_free(std::move(temp._num_free)),
            _chip_width(std::move(temp._chip_width)),
            _chip_height(std::move(temp._chip_height)),
            _modules(std::move(temp._modules)),
            _id_index_map(std::move(temp._id_index_map))
    {
    }

    TplModules &TplModules::operator=(TplModules &&temp)
    {
        _num_modules  = std::move(temp._num_modules);
        _num_free     = std::move(temp._num_free);

        _chip_width   = std::move(temp._chip_width);
        _chip_height  = std::move(temp._chip_height);

        _modules      = std::move(temp._modules);
        _id_index_map = std::move(temp._id_index_map);

        return *this;
    }

    void TplModules::clear()
    {
        _num_modules = 0;
        _num_free    = 0;

        _chip_width  = 0;
        _chip_height = 0;

        _modules.clear();
        _id_index_map.clear();
    }

    TplModule& TplModules::module(const std::string &id)
    {
        assert(_id_index_map.count(id) != 0);

        return _modules[_id_index_map[id]];
    }

    const TplModule& TplModules::module(const std::string &id) const
    {
        assert(_id_index_map.count(id) != 0);

        return _modules.at(_id_index_map.at(id));
    }

    size_t TplModules::module_index(const std::string &id) const
    {
        assert(_id_index_map.count(id) != 0);

        return _id_index_map.at(id);
    }

    bool TplModules::is_module_fixed(const std::string &id) const
    {
        assert(_id_index_map.count(id) != 0);

        return _modules.at(_id_index_map.at(id)).fixed;
    }

    void TplModules::set_free_module_coordinates(const std::vector<double> &xs, const std::vector<double> &ys)
    {
        assert( xs.size() == _num_free );
        assert( ys.size() == _num_free );

        for(size_t i=0; i<_num_free; ++i) {
            _modules[i].x = xs[i];
            _modules[i].y = ys[i];
        }
    }

    void TplModules::get_bookshelf_pls(thueda::BookshelfPls &bpls) const
    {
        assert(bpls.data.size() == 0);

        for(size_t i=0; i<_num_modules; ++i) {
            const TplModule &m = _modules.at(i);
            bpls.data.push_back( {m.id, m.x, m.y, m.fixed} );
        }
    }

    TplNets::TplNets(const BookshelfNets &bnets)
    {
        _num_nets = bnets.num_nets;
        _num_pins = bnets.num_pins;

        copy(bnets.data.begin(), bnets.data.end(), back_inserter(_netlist));
    }

    TplNets::TplNets(TplNets &&temp) :
            _num_nets(std::move(temp._num_nets)),
            _num_pins(std::move(temp._num_pins)),
            _netlist(std::move(temp._netlist))
    {
    }

    TplNets& TplNets::operator=(TplNets &&temp)
    {
        _num_nets = std::move(temp._num_nets);
        _num_pins = std::move(temp._num_pins);
        _netlist  = std::move(temp._netlist);

        return *this;
    }

    void TplNets::clear()
    {
        _num_nets = 0;
        _num_pins = 0;

        _netlist.clear();
    }

    TplDB* TplDB::_instance = NULL;

    TplDB *TplDB::db()
    {
        if(_instance == NULL) {
            _instance = new TplDB;
        }
        return _instance;
    }

    void TplDB::destroy_db()
    {
        delete _instance;
        _instance = NULL;
    }

    //////////////////////////////////Helper Functions///////////////////////////////////
    bool TplDB::load_circuit(const char *path)
    {
        try {
            boost::filesystem::path   benchmark_path(path);
            _benchmark_name = benchmark_path.filename().string();

            //load modules
            boost::filesystem::path node_file_path(benchmark_path);
            node_file_path /= _benchmark_name + ".nodes";
            boost::filesystem::path pl_file_path(benchmark_path);
            pl_file_path /= _benchmark_name + ".pl";

            initialize_modules(node_file_path.c_str(), pl_file_path.c_str());

            //load nets
            boost::filesystem::path net_file_path(benchmark_path);
            net_file_path /= _benchmark_name + ".nets";

            initialize_nets(net_file_path.c_str());

            return true;
        } catch(...) {
            return false;
        }
    }//end TplDB::load_circuit

    void TplDB::generate_placement_snapshot() const
    {
        static int version = 0;

        BookshelfPls bpls;
        modules.get_bookshelf_pls(bpls);

        string out_file_name = _benchmark_name + string("_") + std::to_string(version++) + ".pl";

        ofstream out(out_file_name.c_str(), ios_base::out);
        ostream_iterator<char> ositer(out, "");

        generate_bookshelf_pl(ositer, bpls);
    }

    //private routines

    bool TplDB::read_file(const char *file_name, std::string &storage)
    {
        using namespace std;

        ifstream in(file_name, ios_base::in);
        in.unsetf(ios::skipws);

        storage.clear();
        copy(istream_iterator<char>(in), istream_iterator<char>(), back_inserter(storage));

        in.close();

        return in.good();
    }//end TplDB::read_file

    void TplDB::initialize_modules(const char *node_file, const char *pl_file)
    {
        modules.clear();

        string storage;
        string::const_iterator iter, end;

        //process .nodes file
        read_file(node_file, storage);

        iter = storage.begin();
        end  = storage.end();

        BookshelfNodes bnodes;
        parse_bookshelf_node(iter, end, bnodes);

        //process .pl file
        read_file(pl_file, storage);

        iter = storage.begin();
        end  = storage.end();

        BookshelfPls bpls;
        parse_bookshelf_pl(iter, end, bpls);

        modules = std::move( TplModules(bnodes, bpls) );
    }

    void TplDB::initialize_nets(const char *net_file)
    {
        nets.clear();

        string storage;
        string::const_iterator iter, end;

        read_file(net_file, storage);

        iter = storage.begin();
        end  = storage.end();

        BookshelfNets bnets;
        parse_bookshelf_net(iter, end, bnets);

        nets = std::move( TplNets(bnets) );
    }

    TplDB &pdb = *TplDB::db();

}//end namespace tpl

