/*!
 * \file tpl_algorithm.h
 * \author gy
 * \brief Definition TPL algorithm
 */

#ifndef TPL_ALGORITHM_H
#define TPL_ALGORITHM_H

#include <vector>
#include <map>
#include <string>

#include "tpl_circuit.h"

namespace tpl {

    class TplAlgorithm {
    public:
        typedef std::map<std::pair<size_t, size_t>, double> NetWeight;

		enum Coord {x, y};

        TplAlgorithm();

        /*!
         * \fn bool load_circuit(const std::string &path);
         * \brief Load benchmark circuit into in-memory data structures.
         * \param path the benchmark file's directory path
         */
        bool load_circuit(const std::string &_path);

        /*!
         * \fn void initial_placement();
         * \brief Use bound2bound net model to initialize the module placement.
         */
        void initial_placement();

        /*!
         * \fn void compute_net_force_target(Eigen::Ref<Eigen::VectorXd> &target);
         * \brief Compute each free module's net force target position.
         * \param x_target vector storing the target x positions
         * \param y_target vector storing the target y positions
         */
        void compute_net_force_target(std::vector<double> &x_target, std::vector<double> &y_target);

        void compute_net_weight(const TplNet &net, NetWeight &x_net_weight, NetWeight &y_net_weight);

        /*!
         * \fn void compute_matrix(BookshelfNets nets);
         * \computer matrix C and d
         * \param nets nets
         */
        void computer_matrix(BookshelfNets nets);

        /*!
         * \fn double compute_weight(size_t index_a, size_t index_b);
         * \compute weight of two modules
         * \param pinA pin a
         * \param pinB pin b
         * \param coordinate 0 represents x and 1 represents y
         * \param p degree of the net
         */
        double compute_weight(BookshelfPin pinA, BookshelfPin pinB, Coord coord, unsigned int p);

        /*!
         * \fn bool is_boundary(size_t idx, bool coordinate);
         * \find if a module is boundary module
         * \param idx index of moduls
         * \param coordinate 0 represents x and 1 represents y
         */
        bool is_boundary(size_t idx, Coord coordinate);



    private:
        /*!
         * \fn bool read_file(const char *file_name, std::string &storage);
         * \brief Read file content to a string variable storage;
         * \param file_name file be read
         * \param storage string variable into which read the file content
         */
        bool read_file(const char *file_name, std::string &storage);
        //void compute_net_weight(const std::vector<double> &pos, NetWeight &net_weight);

        double CHIP_WIDTH;
        double CHIP_HEIGHT;
        unsigned int GRID_SIZE;

        TplModules MODULES;
        TplNets NETS;
        size_t NUM_FREE_MODULE;
        // id of pin -> index in MODULES
        std::map<std::string, size_t> MODULE_ID_INDEX_MAP;

        NetWeight MODULE_WEIGHT_X;
        NetWeight MODULE_WEIGHT_Y;


    };


}//end namespace tpl

#endif
