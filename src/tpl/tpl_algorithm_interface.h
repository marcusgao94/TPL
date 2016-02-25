/*!
 * \file tpl_algorithm.h
 * \author Peng Fei
 * \brief Definition TPL algorithm interface
 */

#ifndef TPL_ALGORITHM_INTERFACE_H
#define TPL_ALGORITHM_INTERFACE_H

#include <string>
#include <vector>
#include <map>
#include <utility>

#include "tpl_circuit.h"

#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>

namespace tpl {

    using Eigen::VectorXd;

    /*!
     * \typedef Eigen::SparseMatrix<double> SpMat;
     * \brief SpMat type
     */
    typedef Eigen::SparseMatrix<double> SpMat;
    /*!
     * \typedef Eigen::Triplet<double> SpElem;
     * \brief SpElem type
     */
    typedef Eigen::Triplet<double>      SpElem;
    /*!
     * \typedef Eigen::SimplicialLLT<SpMat> LLTSolver;
     * \brief LLTSolver type
     */
    typedef Eigen::SimplicialLLT<SpMat> LLTSolver;

    /*!
     * \typedef std::map<std::pair<std::string, std::string>, double> NetWeight;
     * \brief NetWeight type
     */
    typedef std::map<std::pair<std::string, std::string>, double> NetWeight;

    class TplAbstractChipGrid {
        public:
            ////////////////////////// Member Access ///////////////////////////////////////
            //! Interface for accessing green function value in index pair idx and idx0.
            /*!
             * \param idx  ranges from 0 to grid_size in both x and y direction.
             * \param idx0 ranges from -grid_size to 2*grid_size in both x and y direction.
             */
            virtual double green_function(const std::pair<int, int> &idx, const std::pair<int, int> &idx0) const = 0;
            //! Interface for accessing power density value in (i,j) position.
            /*!
             * \param i x grid bin index.
             * \param j y grid bin index.
             */
            virtual double power_density(int i, int j) const = 0;
            ////////////////////////// Member Access ///////////////////////////////////////

            ///////////////////////// Modifiers   //////////////////////////////////////////
            //! Iterface for updating the grid size.
            /*!
             * \param grid_size unsigned int value indicating the new grid size.
             */
            virtual void update_green_function(unsigned int grid_size) = 0;
            //! Interface for updating the power density.
            /*!
             */
            virtual void update_power_density() = 0;
            ///////////////////////// Modifiers   //////////////////////////////////////////
    };


    //! The entry class for tpl algorithm.
    class TplAbstractAlgorithm {
        public:

            //! Interface for initializing the module placement.
            /*!
             */
            virtual void make_initial_placement() = 0;

            //! Interface for computing all the free modules' net force target position.
            /*!
             * \param x_target vector storing the target x positions
             * \param y_target vector storing the target y positions
             */
            virtual void compute_net_force_target(std::vector<double> &x_target, std::vector<double> &y_target) = 0;

            //! Interface for computing a net's net weight.
            /*!
             * \param x_net_weight storing the net weight in x directions.
             * \param y_net_weight storing the net weight in y directions.
             */
            virtual void compute_net_weight(const TplNets::net_iterator &nit, NetWeight &x_net_weight, NetWeight &y_net_weight) = 0;
    };

}//end namespace tpl

#endif
