#include "tpl_circuit.h"

#include <fstream>

#include <boost/filesystem.hpp>

#include "../bookshelf/bookshelf_node_parser.hpp"
#include "../bookshelf/bookshelf_pl_parser.hpp"
#include "../bookshelf/bookshelf_net_parser.hpp"

namespace tpl {

    using std::vector;
    using std::list;
    using std::deque;
    using std::map;
    using std::pair;
    using std::string;

    void TplModules::clear()
    {
        ids.clear();
        xcs.clear();
        ycs.clear();
        wds.clear();
        hts.clear();
        flgs.clear();
    }

    TplModule TplModules::operator[](const size_t &i) const
    {
        return TplModule({ids[i], xcs[i], ycs[i], wds[i], hts[i], flgs[i]});
    }

    TplNets::iterator TplNets::begin()
    {
        return netlist.begin();
    }

    TplNets::iterator TplNets::end()
    {
        return netlist.end();
    }

    void TplNets::clear()
    {
        pinstore.clear();
        netlist.clear();
    }

    TplDB *TplDB::db()
    {
        if(_instance == nullptr) {
            _instance = new TplDB;
        }
        return _instance;
    }

    bool TplDB::load_circuit(const std::string &_path)
    {
        using namespace boost::filesystem;
        using namespace thueda;

        try {
            path   benchmark_path(_path);
            string benchmark_name = benchmark_path.filename().string();

            string storage;
            string::const_iterator iter, end;

            //parse .nodes file
            path node_file_path(benchmark_path);
            node_file_path /= benchmark_name + ".nodes";
            read_file(node_file_path.c_str(), storage);

            iter = storage.begin();
            end  = storage.end();

            BookshelfNodes nodes;
            parse_bookshelf_node(iter, end, nodes); 

            //parse .pl file
            path pl_file_path(benchmark_path);
            pl_file_path /= benchmark_name + ".pl";
            read_file(pl_file_path.c_str(), storage);

            iter = storage.begin();
            end  = storage.end();

            BookshelfPls pls;
            parse_bookshelf_pl(iter, end, pls); 

            //parse .nets file
            path net_file_path(benchmark_path);
            net_file_path /= benchmark_name + ".nets";
            read_file(net_file_path.c_str(), storage);

            iter = storage.begin();
            end  = storage.end();

            BookshelfNets nets;
            parse_bookshelf_net(iter, end, nets); 

            //clear old circuit data
            _modules.clear();
            _nets.clear();

            NUM_FREE_MODULE = nodes.num_nodes-nodes.num_terminals;

            //scanning nodes
            deque<Id>         tids;
            deque<Coordinate> txcs, tycs;
            deque<Length>     twds, thts;
            deque<bool>      tflgs;
            size_t cur_terminal_idx=NUM_FREE_MODULE;
            for(size_t i=0; i<nodes.num_nodes; ++i) {

                const BookshelfNode &node = nodes.data[i];
                const BookshelfPl   &pl   = pls.data[i];
                assert( node.id == pl.id );

                if( node.fixed ) {
                    MODULE_ID_INDEX_MAP.insert( make_pair(node.id, cur_terminal_idx++) );
                    
                    tids.push_back(node.id);
                    txcs.push_back(pl.x);
                    tycs.push_back(pl.y);
                    twds.push_back(node.width);
                    thts.push_back(node.height);
                    tflgs.push_back(node.fixed);

                } else {
                    MODULE_ID_INDEX_MAP.insert( make_pair(node.id, _modules.ids.size()) );

                    _modules.ids.push_back(node.id);
                    _modules.xcs.push_back(pl.x);
                    _modules.ycs.push_back(pl.y);
                    _modules.wds.push_back(node.width);
                    _modules.hts.push_back(node.height);
                    _modules.flgs.push_back(node.fixed);
                }

                //update chip width and height
                double right_border = pl.x + node.width; 
                if(right_border>_chip_width) _chip_width  = right_border;
                double top_border = pl.y + node.height; 
                if(top_border>_chip_height) _chip_height = top_border;
            }

            _modules.ids.insert( _modules.ids.end(),  tids.begin(),  tids.end() );
            _modules.xcs.insert( _modules.xcs.end(),  txcs.begin(),  txcs.end() );
            _modules.ycs.insert( _modules.ycs.end(),  tycs.begin(),  tycs.end() );
            _modules.wds.insert( _modules.wds.end(),  twds.begin(),  twds.end() );
            _modules.hts.insert( _modules.hts.end(),  thts.begin(),  thts.end() );
            _modules.flgs.insert(_modules.flgs.end(), tflgs.begin(), tflgs.end());
            //end scanning nodes

            //scanning nets and pins
            map<string, TplPin*> pin_add_map;
            _nets.pinstore.reserve(nets.num_pins);
            for(vector<BookshelfNet>::iterator nit=nets.data.begin(); nit!=nets.data.end(); ++nit) {
                TplNet net;
                net.id = nit->id;

                for(vector<BookshelfPin>::iterator pit=nit->pins.begin(); pit!=nit->pins.end(); ++pit) {
                    if( pin_add_map.count(pit->id) == 0 ) {
                        TplPin pin;
                        pin.id = pit->id;
                        pin.dx = pit->dx;
                        pin.dy = pit->dy;

                        pin_add_map.insert( make_pair(pin.id, &_nets.pinstore+_nets.pinstore.size()) );
                        _nets.pinstore.push_back(pin);
                    }
                    net.pins.push_back( pin_add_map[pit->id] );
                }

                _nets.netlist.push_back(net);
            }
            //end scanning nets and pins

            _grid_size = 100;

            return true;
        } catch(...) {
            return false;
        }
    }//end TplDB::load_circuit

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

