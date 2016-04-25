#include "tpl_db.h"

#include <cmath>
#include <sstream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include "../bookshelf/bookshelf_node_parser.hpp"
#include "../bookshelf/bookshelf_pl_parser.hpp"
#include "../bookshelf/bookshelf_net_parser.hpp"
#include "../bookshelf/bookshelf_pl_generator.hpp"


namespace tpl {
    using namespace std;
    using namespace thueda;

    TplModule::TplModule(Id id, Coordinate x, Coordinate y, Length width, Length height,
                         bool fixed, double power_density) :
            id(id), x(x), y(y), width(width), height(height), fixed(fixed), power_density(power_density)
    {
    }

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

            _modules.emplace_back(bnode.id, bpl.x, bpl.y, bnode.width, bnode.height, bnode.fixed, 1);
            _id_index_map.insert( make_pair(bnode.id, i) );

            double right_border = bpl.x + bnode.width;
            if(right_border>_chip_width) _chip_width  = right_border;
            double top_border = bpl.y + bnode.height;
            if(top_border>_chip_height)  _chip_height = top_border;
        }
    }

    TplModules::TplModules(BOOST_RV_REF(TplModules) temp) :
            _num_modules(temp._num_modules),
            _num_free(temp._num_free),
            _chip_width(temp._chip_width),
            _chip_height(temp._chip_height),
            _modules(std::move(temp._modules)),
            _id_index_map(std::move(temp._id_index_map))
    {
    }

    TplModules &TplModules::operator=(BOOST_RV_REF(TplModules) temp)
    {
        _num_modules  = temp._num_modules;
        _num_free     = temp._num_free;

        _chip_width   = temp._chip_width;
        _chip_height  = temp._chip_height;

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
        bpls.data.clear();

        for(size_t i=0; i<_num_modules; ++i) {
            const TplModule &m = _modules.at(i);
            bpls.data.emplace_back( m.id, m.x, m.y, m.fixed );
        }
    }

    TplNets::TplNets(const BookshelfNets &bnets)
    {
        _num_nets = bnets.num_nets;
        _num_pins = bnets.num_pins;

        copy(bnets.data.begin(), bnets.data.end(), back_inserter(_netlist));
    }

    TplNets::TplNets(BookshelfNets &&bnets)
    {
        _num_nets = bnets.num_nets;
        _num_pins = bnets.num_pins;

        _netlist = std::move(bnets.data);
    }

    TplNets::TplNets(BOOST_RV_REF(TplNets) temp) :
            _num_nets(temp._num_nets),
            _num_pins(temp._num_pins),
            _netlist(std::move(temp._netlist))
    {
    }

    TplNets& TplNets::operator=(BOOST_RV_REF(TplNets) temp)
    {
        _num_nets = temp._num_nets;
        _num_pins = temp._num_pins;
        _netlist  = std::move(temp._netlist);

        return *this;
    }

    void TplNets::clear()
    {
        _num_nets = 0;
        _num_pins = 0;

        _netlist.clear();
    }


    TplDB &TplDB::db()
    {
        static TplDB _instance;
        return _instance;
    }


    //////////////////////////////////Helper Functions///////////////////////////////////
    bool TplDB::load_circuit(const std::string &path)
    {
        try {
            boost::filesystem::path   benchmark_path(path);
            _benchmark_name = benchmark_path.filename().string();

            //load modules
            boost::filesystem::path node_file_path(benchmark_path);
            node_file_path /= _benchmark_name + string(".nodes");
            boost::filesystem::path pl_file_path(benchmark_path);
            pl_file_path /= _benchmark_name + string(".pl");

            initialize_modules(node_file_path.string(), pl_file_path.string());

            //load nets
            boost::filesystem::path net_file_path(benchmark_path);
            net_file_path /= _benchmark_name + string(".nets");

            initialize_nets(net_file_path.string());

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

        string out_file_name;
        stringstream ss(out_file_name);
        ss << _benchmark_name << "_" << version++ << ".pl";

        ofstream out(out_file_name.c_str(), ios_base::out);
        ostream_iterator<char> ositer(out, "");

        generate_bookshelf_pl(ositer, bpls);
    }

    //private routines

    void TplDB::initialize_modules(const std::string &node_file, const std::string &pl_file)
    {
        modules.clear();

        //process .nodes file
        ifstream node_stream(node_file);
        node_stream.unsetf(ios::skipws);

        boost::spirit::istream_iterator node_begin(node_stream), node_end;

        BookshelfNodes bnodes;
        parse_bookshelf_node(node_begin, node_end, bnodes);

        //process .pl file
        ifstream pl_stream(pl_file);
        pl_stream.unsetf(ios::skipws);

        boost::spirit::istream_iterator pl_begin(pl_stream), pl_end;

        BookshelfPls bpls;
        parse_bookshelf_pl(pl_begin, pl_end, bpls);

        modules = std::move( TplModules(bnodes, bpls) );
    }

    void TplDB::initialize_nets(const std::string &net_file)
    {
        nets.clear();

        //process .nets file
        ifstream net_stream(net_file);
        net_stream.unsetf(ios::skipws);

        boost::spirit::istream_iterator net_begin(net_stream), net_end;

        BookshelfNets bnets;
        parse_bookshelf_net(net_begin, net_end, bnets);

        nets = std::move( TplNets( std::move(bnets) ) );
    }

}//end namespace tpl

