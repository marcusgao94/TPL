/*!
 * \file tpl_algorithm.h
 * \author Peng Fei
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

        private:
            bool read_file(const char *file_name, std::string &storage);
            //void compute_net_weight(const std::vector<double> &pos, NetWeight &net_weight);

            double CHIP_WIDTH;
            double CHIP_HEIGHT;
            unsigned int GRID_SIZE;

            TplModules MODULES;
            TplNets    NETS;
            size_t NUM_FREE_MODULE;
            std::map<std::string, size_t> MODULE_ID_INDEX_MAP;

    };

}//end namespace tpl

#endif
