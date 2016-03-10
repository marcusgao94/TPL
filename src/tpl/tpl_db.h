/*!
 * \file tpl_db.h
 * \author Peng Fei
 * \brief Definition for circuit related data structures
 */

#ifndef TPL_DB_H
#define TPL_DB_H

#include <string>
#include <vector>
#include <list>
#include <map>
#include <stdexcept>

#include "../bookshelf/bookshelf_node.h"
#include "../bookshelf/bookshelf_pl.h"
#include "../bookshelf/bookshelf_net.h"

//#ifndef pdb
//#define pdb (*(tpl::TplDB::db()))
//#endif

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
        double power_density; //!< A module's power density
    };

    //! class storing all the modules' positions and sizes information.
    class TplModules {
    public:
        ///////////////////////// Member Type //////////////////////////////////////////
        //! \typedef std::vector<TplModule>::iterator iterator;
        typedef std::vector<TplModule>::iterator iterator;
        //! \typedef std::vector<TplModule>::const_iterator const_iterator;
        typedef std::vector<TplModule>::const_iterator const_iterator;
        ///////////////////////// Member Type //////////////////////////////////////////

        ///////////////////////// Constructors /////////////////////////////////////////
        //! Default constructor defaulted.
        TplModules() = default;
        //! Constructor using bookshelf data structures.
        TplModules(const BookshelfNodes &bnodes, const BookshelfPls &bpls);
        //! Destructor defaulted.
        ~TplModules() = default;
        //! Copy constructor deleted.
        TplModules(const TplModules &) = delete;
        //! Copy assignment operator deleted.
        TplModules& operator=(const TplModules &) = delete;
        //! Move constructor.
        TplModules(TplModules &&temp);
        //! Move assignment opeartor.
        TplModules &operator=(TplModules &&temp);
        ///////////////////////// Constructors /////////////////////////////////////////

        ////////////////////////// Member Access ///////////////////////////////////////
        //! Access TplModule using index, return reference with bound checking.
        TplModule&       at(const size_t &pos)
        {
            try {
                return _modules.at(pos);
            }
            catch(std::out_of_range e) {
                throw e;
            }
        }
        //! Access TplModule using index, return const reference with bound checking.
        const TplModule& at(const size_t &pos) const
        {
            try {
                return _modules.at(pos);
            }
            catch(std::out_of_range e) {
                throw e;
            }
        }
        //! Access TplModule using index, return reference without bound checking.
        TplModule&       operator[](const size_t &pos)
        {
            return _modules[pos];
        }
        //! Access TplModule using index, return const reference without bound checking.
        const TplModule& operator[](const size_t &pos) const
        {
            return _modules[pos];
        }
        ////////////////////////// Member Access ///////////////////////////////////////

        ///////////////////////// Iterators   //////////////////////////////////////////
        //! Iterator indicating the first TplModule.
        iterator begin()
        {
            return _modules.begin();
        }
        //! Iterator indicating the first TplModule, const version.
        const_iterator cbegin() const
        {
            return _modules.begin();
        }
        //! Iterator indicating the past-the-last TplModule.
        iterator end()
        {
            return _modules.end();
        }
        //! Iterator indicating the past-the-last TplModule, const version.
        const_iterator cend() const
        {
            return _modules.end();
        }
        ///////////////////////// Iterators   //////////////////////////////////////////

        ///////////////////////// Capacity    //////////////////////////////////////////
        //! Total number of modules.
        unsigned int size()      const
        {
            return _num_modules;
        }
        //! Number of free modules.
        unsigned int num_free()  const
        {
            return _num_free;
        }
        //! Number of fixed modules.
        unsigned int num_fixed() const
        {
            return _num_modules-_num_free;
        }
        ///////////////////////// Capacity    //////////////////////////////////////////

        ///////////////////////// Modifiers   //////////////////////////////////////////
        //! Clear all the modules' contents.
        void clear();
        //! Set the free modules' coordinates.
        /*!
         * \param xs The free modules' x coordinates.
         * \param ys The free modules' y coordinates.
         */
        void set_free_module_coordinates(const std::vector<double> &xs, const std::vector<double> &ys);
        ///////////////////////// Modifiers   //////////////////////////////////////////

        ///////////////////////// Id based Member Access ///////////////////////////////
        //! Get a TplModule reference with a Id id.
        TplModule&       module(const std::string &id);
        //! Get a const TplModule reference with a Id id.
        const TplModule& module(const std::string &id) const;
        //! Get a module's index, with a Id id.
        size_t module_index(const std::string &id) const;
        //! Query interface checking wether a module is fixed.
        bool is_module_fixed(const std::string &id) const;
        ///////////////////////// Id based Member Access ///////////////////////////////

        ///////////////////////// Chip Size ////////////////////////////////////////////
        //! Query interface for chip width.
        double chip_width()  const
        {
            return _chip_width;
        }
        //! Query interface for chip height.
        double chip_height() const
        {
            return _chip_height;
        }
        ///////////////////////// Chip Size ////////////////////////////////////////////

        ///////////////////////// Bookshelf Conversion /////////////////////////////////
        //! Write the current placement status in BookshelfPls data structure.
        /*!
         * \param bpls The BookshelfPls data structure holding the result.
         */
        void get_bookshelf_pls(BookshelfPls &bpls) const;
        ///////////////////////// Bookshelf Conversion /////////////////////////////////
    private:
        unsigned int _num_modules; //!< Number of modules.
        unsigned int _num_free;    //!< Number of free modules.

        double _chip_width;  //!< Chip width.
        double _chip_height; //!< Chip height.

        std::vector<TplModule> _modules;                //!< vector of TplModule.
        std::map<std::string, size_t> _id_index_map;    //!< A ID index map for all the modules
    };

    //! \typedef BookshelfPin TplPin;
    typedef BookshelfPin TplPin;
    //! \typedef BookshelfNet TplNet;
    typedef BookshelfNet TplNet;

    //! class storing all the nets and pins.
    class TplNets {
    public:
        ///////////////////////// Member Type //////////////////////////////////////////
        //! \typedef std::list<TplNet>::iterator net_iterator;
        typedef std::list<TplNet>::iterator net_iterator;
        //! \typedef std::list<TplNet>::const_iterator const_net_iterator;
        typedef std::list<TplNet>::const_iterator const_net_iterator;
        //! \typedef std::vector<TplPin>::iterator pin_iterator;
        typedef std::vector<TplPin>::iterator pin_iterator;
        //! \typedef std::vector<TplPin>::iterator iterator;
        typedef std::vector<TplPin>::const_iterator const_pin_iterator;
        ///////////////////////// Member Type //////////////////////////////////////////

        ///////////////////////// Constructors /////////////////////////////////////////
        //! Default constructor defaulted.
        TplNets() = default;
        //! Constructor using bookshelf data structures.
        TplNets(const BookshelfNets &bnets);
        //! Destructor defaulted.
        ~TplNets() = default;
        //! Copy constructor deleted.
        TplNets(const TplNets &) = delete;
        //! Copy assignment operator deleted.
        TplNets& operator=(const TplNets &) = delete;
        //! Move constructor.
        TplNets(TplNets &&temp);
        //! Move assignment operator deleted.
        TplNets& operator=(TplNets &&temp);
        ///////////////////////// Constructors /////////////////////////////////////////

        ////////////////////////// Member Access ///////////////////////////////////////
        //! Access first TplNet.
        TplNet &front()
        {
            return _netlist.front();
        }
        //! Access first TplNet, const version.
        const TplNet &front() const
        {
            return _netlist.front();
        }
        //! Access last TplNet.
        TplNet &back()
        {
            return _netlist.back();
        }
        //! Access last TplNet, const version.
        const TplNet &back() const
        {
            return _netlist.back();
        }
        ////////////////////////// Member Access ///////////////////////////////////////

        ///////////////////////// Iterators   //////////////////////////////////////////
        //! Iterator indicating the first TplNet.
        net_iterator net_begin()
        {
            return _netlist.begin();
        }
        //! Iterator indicating the first TplNet, const version.
        const_net_iterator cnet_begin() const
        {
            return _netlist.begin();
        }
        //! Iterator indicating the past-the-last TplNet.
        net_iterator net_end()
        {
            return _netlist.end();
        }
        //! Iterator indicating the past-the-last TplNet, const version.
        const_net_iterator cnet_end() const
        {
            return _netlist.end();
        }
        //! Iterator indicating the first TplPin.
        pin_iterator pin_begin(const net_iterator &nit)
        {
            return nit->pins.begin();
        }
        //! Iterator indicating the first TplPin, const version.
        const_pin_iterator cpin_begin(const net_iterator &nit) const
        {
            return nit->pins.begin();
        }
        //! Iterator indicating the past-the-last TplPin.
        pin_iterator pin_end(const net_iterator &nit)
        {
            return nit->pins.end();
        }
        //! Iterator indicating the past-the-last TplPin, const version.
        const_pin_iterator cpin_end(const net_iterator &nit) const
        {
            return nit->pins.end();
        }
        ///////////////////////// Iterators   //////////////////////////////////////////

        ///////////////////////// Capacity    //////////////////////////////////////////
        //! Number of nets.
        unsigned int num_nets() const
        {
            return _num_nets;
        }
        //! Number of pins.
        unsigned int num_pins() const
        {
            return _num_pins;
        }
        ///////////////////////// Capacity    //////////////////////////////////////////

        ///////////////////////// Modifiers   //////////////////////////////////////////
        //! Clear the netlist.
        void clear();
        ///////////////////////// Modifiers   //////////////////////////////////////////
    private:
        unsigned int _num_nets;           //!< Number of nets.
        unsigned int _num_pins;           //!< Number of pins.

        std::list<TplNet> _netlist; //!< A sequential container representing the netlist.
    };

    //! The main class for data savings and manipulations.
    class TplDB {
    public:
        //! Realize the singleton design pattern.
        static TplDB *db();
        static void destroy_db();

        //! Load benchmark circuit into in-memory data structures.
        /*!
         * \param path The benchmark file's directory path.
         * \return A boolean variable indicating wether the operation is success.
         */
        bool load_circuit(const char *path);

        //! Take a snapshot of the current placement.
        void generate_placement_snapshot() const;

        TplModules modules;     //!< storing the modules
        TplNets    nets;        //!< storing the nets

    private:
        ///////////////////////// Constructors /////////////////////////////////////////
        //! TplDB constructor.
        TplDB() = default;
        //! TplDB destructor.
        ~TplDB() = default;
        //! TplDB copy constructor deleted.
        TplDB(const TplDB&) = delete;
        //! TplDB copy assignment operator deleted.
        TplDB &operator=(const TplDB&) = delete;
        //! TplDB move constructor deleted.
        TplDB(const TplDB&&) = delete;
        //! TplDB move assignment operator deleted.
        TplDB &operator=(const TplDB&&) = delete;
        ///////////////////////// Constructors /////////////////////////////////////////

        //////////////////////////////////Helper Functions///////////////////////////////////
        //! Private helper routine loading a file into string.
        bool read_file(const char *file_name, std::string &storage);
        //! Private helper routine initialize the modules member variable.
        void initialize_modules(const char *node_file, const char *pl_file);
        //! Private helper routine initialize the nets member variable.
        void initialize_nets   (const char *net_file);
        //////////////////////////////////Helper Functions///////////////////////////////////

        static TplDB *_instance; //!< The TplDB singleton

        std::string _benchmark_name; //!< The current loading circuit's name.
    };

    extern TplDB &pdb;

}//end namespace tpl

#endif//TPL_DB_H

