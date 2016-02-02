#include "tpl_circuit.h"

#include <fstream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "../bookshelf/bookshelf_node_parser.hpp"
#include "../bookshelf/bookshelf_pl_parser.hpp"
#include "../bookshelf/bookshelf_net_parser.hpp"
#include "../bookshelf/bookshelf_pl_generator.hpp"

namespace tpl {
    using namespace std;
    using namespace thueda;

    void TplModules::clear()
    {
        ids.clear();
        xcs.clear();
        ycs.clear();
        wds.clear();
        hts.clear();
        flgs.clear();

        num_nodes = 0;
        num_free  = 0;
    }

    TplModule TplModules::operator[](const size_t &i) const
    {
        return TplModule({ids[i], xcs[i], ycs[i], wds[i], hts[i], flgs[i]});
    }

    void TplModules::set_free_module_coordinates(const std::vector<double> &xs, const std::vector<double> &ys)
    {
#ifndef NDEBUG
        assert( xs.size() == num_free );
        assert( ys.size() == num_free );
#endif

        xcs.erase( xcs.begin(), xcs.begin()+num_free);
        ycs.erase( ycs.begin(), ycs.begin()+num_free);

        xcs.insert(xcs.begin(), xs.begin(), xs.end());
        ycs.insert(ycs.begin(), ys.begin(), ys.end());
    }

    void TplModules::get_bookshelf_pls(thueda::BookshelfPls &bpls) const
    {
#ifndef NDEBUG
        assert(bpls.data.size() == 0);
#endif

        for(size_t i=0; i<num_nodes; ++i) {
            bpls.data.push_back( BookshelfPl({ids[i], xcs[i], ycs[i], flgs[i]}) );
        }
    }

    TplPin::TplPin(const Id &_id, const Distance &_dx, const Distance &_dy) : id(_id), dx(_dx), dy(_dy)
    {
    }

    void TplNets::clear()
    {
        netlist.clear();
        num_nets = 0;
        num_pins = 0;
    }

    TplDB* TplDB::_instance = NULL;

    TplDB *TplDB::db()
    {
        if(_instance == NULL) {
            _instance = new TplDB;
        }
        return _instance;
    }

    //////////////////////Query Iterface///////////////////////
    const double &TplDB::get_chip_width()  const
    {
        return _chip_width;
    }

    const double &TplDB::get_chip_height() const
    {
        return _chip_height;
    }

    TplModule TplDB::get_module(const std::string &id) const
    {
        size_t idx = _module_id_index_map.at(id);
        return _modules[idx];
    }

    size_t TplDB::get_module_index(const std::string &id) const
    {
        return _module_id_index_map.at(id);
    }

    unsigned int TplDB::get_number_of_free_modules() const
    {
        return _modules.num_free;
    }

    bool TplDB::is_module_fixed(const std::string &id) const
    {
        return _modules.flgs[_module_id_index_map.at(id)];
    }
    //////////////////////Query Iterface///////////////////////
    
    //////////////////////////////////Modification Iterface///////////////////////////////////
    void TplDB::update_free_module_position(const std::vector<double> &xs, const std::vector<double> &ys)
    {
        _modules.set_free_module_coordinates(xs, ys);
    }
    //////////////////////////////////Modification Iterface///////////////////////////////////
    
    //////////////////////////////////Helper Functions///////////////////////////////////
    bool TplDB::load_circuit(const char *path)
    {
        using namespace thueda;

        try {
            boost::filesystem::path   benchmark_path(path);
            _benchmark_name = benchmark_path.filename().string();

            string storage;
            string::const_iterator iter, end;

            //parse .nodes file
            boost::filesystem::path node_file_path(benchmark_path);
            node_file_path /= _benchmark_name + ".nodes";
            read_file(node_file_path.c_str(), storage);

            iter = storage.begin();
            end  = storage.end();

            BookshelfNodes bnodes;
            parse_bookshelf_node(iter, end, bnodes); 

            //parse .pl file
            boost::filesystem::path pl_file_path(benchmark_path);
            pl_file_path /= _benchmark_name + ".pl";
            read_file(pl_file_path.c_str(), storage);

            iter = storage.begin();
            end  = storage.end();

            BookshelfPls bpls;
            parse_bookshelf_pl(iter, end, bpls); 

            //parse .nets file
            boost::filesystem::path net_file_path(benchmark_path);
            net_file_path /= _benchmark_name + ".nets";
            read_file(net_file_path.c_str(), storage);

            iter = storage.begin();
            end  = storage.end();

            BookshelfNets bnets;
            parse_bookshelf_net(iter, end, bnets); 

            //clear old circuit data
            _modules.clear();
            _nets.clear();


            //filling modules
            _modules.num_nodes = bnodes.num_nodes;
            _modules.num_free  = bnodes.num_nodes - bnodes.num_terminals;

            deque<Id>         tids;
            deque<Coordinate> txcs, tycs;
            deque<Length>     twds, thts;
            deque<bool>      tflgs;
            size_t cur_terminal_idx = _modules.num_free;
            for(size_t i=0; i<bnodes.num_nodes; ++i) {

                const BookshelfNode &bnode = bnodes.data[i];
                const BookshelfPl   &bpl   = bpls.data[i];

#ifndef NDEBUG
                assert( bnode.id == bpl.id );
#endif

                if( bnode.fixed ) {
                    _module_id_index_map.insert( make_pair(bnode.id, cur_terminal_idx++) );
                    
                    tids.push_back(bnode.id);
                    txcs.push_back(bpl.x);
                    tycs.push_back(bpl.y);
                    twds.push_back(bnode.width);
                    thts.push_back(bnode.height);
                    tflgs.push_back(bnode.fixed);

                } else {
                    _module_id_index_map.insert( make_pair(bnode.id, _modules.ids.size()) );

                    _modules.ids.push_back(bnode.id);
                    _modules.xcs.push_back(bpl.x);
                    _modules.ycs.push_back(bpl.y);
                    _modules.wds.push_back(bnode.width);
                    _modules.hts.push_back(bnode.height);
                    _modules.flgs.push_back(bnode.fixed);
                }

                //update chip width and height
                double right_border = bpl.x + bnode.width; 
                if(right_border>_chip_width) _chip_width  = right_border;
                double top_border = bpl.y + bnode.height; 
                if(top_border>_chip_height) _chip_height = top_border;
            }

            _modules.ids.insert( _modules.ids.end(),  tids.begin(),  tids.end() );
            _modules.xcs.insert( _modules.xcs.end(),  txcs.begin(),  txcs.end() );
            _modules.ycs.insert( _modules.ycs.end(),  tycs.begin(),  tycs.end() );
            _modules.wds.insert( _modules.wds.end(),  twds.begin(),  twds.end() );
            _modules.hts.insert( _modules.hts.end(),  thts.begin(),  thts.end() );
            _modules.flgs.insert(_modules.flgs.end(), tflgs.begin(), tflgs.end());
            //end filling modules

            //filling nets and pins
            _nets.num_nets = bnets.num_nets;
            _nets.num_pins = bnets.num_pins;

            for(vector<BookshelfNet>::iterator nit=bnets.data.begin(); nit!=bnets.data.end(); ++nit) {
                TplNet net;
                net.id     = nit->id;
                net.degree = nit->degree;
                for(vector<BookshelfPin>::iterator pit=nit->pins.begin(); pit!=nit->pins.end(); ++pit) {
                    net.pins.push_back(TplPin(pit->id, pit->dx, pit->dy));
                }
                _nets.netlist.push_back(net);
            }
            //end filling nets and pins

            _grid_size = 100;

            return true;
        } catch(...) {
            return false;
        }
    }//end TplDB::load_circuit
    
    void TplDB::generate_placement_snapshot() const
    {
        static int version = 0;

        BookshelfPls bpls;
        _modules.get_bookshelf_pls(bpls);

        string out_file_name = _benchmark_name + string("_") + boost::lexical_cast<string>(version++) + ".pl";

        ofstream out(out_file_name.c_str(), ios_base::out);
        ostream_iterator<char> ositer(out, "");

        generate_bookshelf_pl(ositer, bpls); 
    }
    //////////////////////////////////Helper Functions///////////////////////////////////
    
    //private routines
    TplDB::TplDB()
    {
    }

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


}//end namespace tpl

