/*!
 * \file tpl_circuit.h
 * \author Peng Fei
 * \brief Definition for circuit related data structures
 */

#ifndef TPL_CIRCUIT_H 
#define TPL_CIRCUIT_H

#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>

#include <boost/ptr_container/ptr_vector.hpp>

#include "../bookshelf/utils.h"
/*
#include "../bookshelf/bookshelf_node.h"
#include "../bookshelf/bookshelf_pl.h"
#include "../bookshelf/bookshelf_net.h"
*/

namespace tpl {

    using namespace thueda;

    struct TplModule {
        Id         id; //!< A module's id
        Coordinate  x; //!< A module's x coordinate
        Coordinate  y; //!< A module's y coordinate
        Length  width; //!< A module's width
        Length height; //!< A module's height
        bool    fixed; //!< A module's fixed flag
    };

    //! class storing all the modules' positions and sizes information
    /*!
     * Please note that module's coordinates is its center point's location.
     */
    class TplModules {
        public:
            void clear();
            TplModule operator[](const size_t &i) const;

        private:
            std::deque<Id>         ids; //!< All modules' ids
            std::deque<Coordinate> xcs; //!< All modules' x coordinates
            std::deque<Coordinate> ycs; //!< All modules' y coordinates
            std::deque<Length>     wds; //!< All modules' widths
            std::deque<Length>     hts; //!< All modules' heights 
            std::deque<bool>      flgs; //!< All modules' fixed flags

            friend class TplDB;
    };

    //! public class storing a pin's related module's index and its offset from the module center
    /*!
     * Member variable id is the pin's corresponding module's id.
     * Member variables dx and dy is not used temporarily. 
     */
    struct TplPin {
        Id       id;
        Distance dx;
        Distance dy;
    };

    //! public class storing a net
    /*!
     * The net's pins are stored as a vector of pointer using boost::ptr_vector.
     */
    struct TplNet {
        Id                          id;
        boost::ptr_vector<TplPin> pins;
    };

    //! public class storing all the nets
    /*!
     */
    class TplNets {
        public:
            typedef std::list<TplNet>::iterator iterator;
            iterator begin();
            iterator end();

            void clear();
        private:
            std::vector<TplPin> pinstore;
            std::list<TplNet>   netlist;

            friend class TplDB;
    };

    class TplDB {
        public:
            static TplDB *db();

            bool load_circuit(const std::string &_path);
        private:
            TplDB();
            TplDB(const TplDB&) = delete;
            TplDB &operator=(const TplDB&) = delete;

            bool read_file(const char *file_name, std::string &storage);

            static TplDB *_instance;

            double _chip_width;
            double _chip_height;
            unsigned int _grid_size;

            TplModules _modules;
            TplNets    _nets;
            size_t NUM_FREE_MODULE;
            std::map<std::string, size_t> MODULE_ID_INDEX_MAP;
    };

}//end namespace tpl

#endif

