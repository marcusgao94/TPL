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

    //! The entry class for tpl algorithm.
    class TplAlgorithm {
        public:

            /*!
             * \typedef std::map<std::pair<std::string, std::string>, double> NetWeight;
             * \brief NetWeight type
             */
            typedef std::map<std::pair<std::string, std::string>, double> NetWeight;

            //! TplAlgorithm constructor.
            TplAlgorithm();

            //! Use bound2bound net model to initialize the module placement.
            /*!
             */
            void make_initial_placement();

            //! Compute all the free modules' net force target position.
            /*!
             * \param x_target vector storing the target x positions
             * \param y_target vector storing the target y positions
             */
            void compute_net_force_target(std::vector<double> &x_target, std::vector<double> &y_target);

            //! Compute a net's net weight.
            /*!
             * \param x_net_weight storing the net weight in x directions.
             * \param y_net_weight storing the net weight in y directions.
             */
            void compute_net_weight(const TplNets::net_iterator &nit, NetWeight &x_net_weight, NetWeight &y_net_weight);

        private:
            //! Private routine for computing all the free module's net force target position.
            /*!
             * \param net_weight a map of Id pair to double representing the net weight.
             * \param target vector for storing the target  positions.
             */
            void compute_net_force_target(const NetWeight &net_weight, std::vector<double> &target);

            //! Private routine for computing a net's net weight.
            /*!
             * \param ids vector storing the pins' Ids.
             * \param coordinates vector storing the pins' coordinates. 
             * \param min storing the minimum pin coordinate. 
             * \param max storing the maximum pin coordinate. 
             * \param min_idx storing the minimum-coordinate pin's index. 
             * \param max_idx storing the maximum-coordinate pin's index. 
             * \param net_weight storing the computed net weight.
             */
            void compute_net_weight(const std::vector<std::string> &ids, const std::vector<double> &coordinates,
                    const double &min, const double &max, const size_t &min_idx, const size_t &max_idx, NetWeight &net_weight);
    };

}//end namespace tpl

#endif
