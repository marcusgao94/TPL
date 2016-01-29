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

namespace tpl {

    using namespace thueda;

    //! struct storing a module's position and size information.
    /*!
     * Please note that the module's coordinates is its center point's location.
     */
    struct TplModule {
        Id         id; //!< A module's id
        Coordinate  x; //!< A module's x coordinate
        Coordinate  y; //!< A module's y coordinate
        Length  width; //!< A module's width
        Length height; //!< A module's height
        bool    fixed; //!< A module's fixed flag
    };

    //! class storing all the modules' positions and sizes information.
    class TplModules {
        public:
            //! Clear all the modules' contents.
            void clear();

            //! Provides an index based access interface to TplModules.
            /*!
             * \param i A module's index.
            */
            TplModule operator[](const size_t &i) const;

        private:
            std::deque<Id>         ids; //!< All modules' ids
            std::deque<Coordinate> xcs; //!< All modules' x coordinates
            std::deque<Coordinate> ycs; //!< All modules' y coordinates
            std::deque<Length>     wds; //!< All modules' widths
            std::deque<Length>     hts; //!< All modules' heights 
            std::deque<bool>      flgs; //!< All modules' fixed flags

            unsigned int     num_nodes; //!< Number of modules
            unsigned int      num_free; //!< Number of free modules

            friend class TplDB;
    };

    //! struct storing a pin's related module's id and its offset from the module center.
    struct TplPin {
        Id       id; //!< The pin's corresponding module's id.
        Distance dx; //!< Not used contemporarily.
        Distance dy; //!< Not used contemporarily.
    };

    //! struct storing a net's information.
    /*!
     * The net's pins are stored as a vector of pointer using boost::ptr_vector for the sake of memory usage.
     */
    struct TplNet {
        Id                          id; //!< The net's id.
        boost::ptr_vector<TplPin> pins; //!< vector of pointers to the net related pins.
    };

    //! class storing all the nets and pins.
    class TplNets {
        public:
            /*!
             * \typedef std::list<TplNet>::iterator iterator;
             * \brief TplNets::iterator type
             */
            typedef std::list<TplNet>::iterator iterator;

            //! Iterator access function representing the netlist beginning.
            iterator begin();

            //! Iterator access function representing the netlist ending.
            iterator end();

            //! Clear all the nets' contents.
            void clear();
        private:
            std::vector<TplPin> pinstore; //!< A container storing all the pins uniquely.
            std::list<TplNet>   netlist;  //!< A sequential container representing the netlist.

            unsigned int num_nets;        //!< Number of nets.
            unsigned int num_pins;        //!< Number of pins.

            friend class TplDB;
    };

    //! The main class for data savings and manipulations.
    class TplDB {
        public:
            /*!
             * \typedef TplNets::iterator net_iterator;
             * \brief TplDB::net_iterator type
             */
            typedef TplNets::iterator net_iterator;

            //! Realize the singleton design pattern.
            static TplDB *db();

            //! Load benchmark circuit into in-memory data structures.
            /*!
             * \param path the benchmark file's directory path 
             * \return A boolean variable indicating wether the operation is success.
             */
            bool load_circuit(const std::string &_path);
            

            //! Query interface for chip width.
            const double &get_chip_width()  const;
            //! Query interface for chip height.
            const double &get_chip_height() const;

            //! Query interface for a module using a Id.
            TplModule get_module      (const std::string &id) const;
            //! Query interface for a module's index.
            size_t    get_module_index(const std::string &id) const;
            //! Query interface for number of free modules.
            unsigned int get_number_of_free_modules() const;
            //! Query interface checking wether a module is fixed.
            bool is_module_fixed(const std::string &id) const;

            //! Iterator access function representing the netlist beginning.
            net_iterator net_begin();
            //! Iterator access function representing the netlist ending.
            net_iterator net_end();

        private:
            //! TplDB constructor.
            TplDB();
            //! TplDB copy constructor, which has been claimed deleted.
            TplDB(const TplDB&) = delete;
            //! TplDB assignment operator, which has been claimed deleted.
            TplDB &operator=(const TplDB&) = delete;

            //! Private helper routine loading a file into string.
            bool read_file(const char *file_name, std::string &storage);

            static TplDB *_instance; //!< The TplDB singleton

            double _chip_width;      //!< storing chip width
            double _chip_height;     //!< storing chip height
            unsigned int _grid_size; //!< storing grid size during tpl algorithm computation

            TplModules _modules;     //!< storing the modules
            TplNets    _nets;        //!< storing the nets
            std::map<std::string, size_t> _module_id_index_map; //!< a id index map for all the modules
    };

}//end namespace tpl

#endif

