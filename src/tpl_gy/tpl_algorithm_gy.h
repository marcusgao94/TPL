/*!
 * \file tpl_algorithm_pf.h
 * \author Peng Fei
 * \brief Definition for TPL algorithm
 */

#ifndef TPL_ALGORITHM_GY_H
#define TPL_ALGORITHM_GY_H

#include <vector>
#include <map>
#include <string>

#include "../tpl/tpl_algorithm_interface.h"

namespace tpl {

    class TplChipGridGY : public TplAbstractChipGrid {
        public:
            ///////////////////////// Constructors /////////////////////////////////////////
            //! Default constructor defaulted.
            TplChipGridGY() = default;
            //! Constructor using grid_size, r1 and r2 radius.
            TplChipGridGY(double r1, double r2, unsigned int grid_size);
            //! Destructor defaulted.
            ~TplChipGridGY() = default;
            //! Copy constructor defaulted.
            TplChipGridGY(const TplChipGridGY &rhs) = default;
            //! Assignment operator defaulted.
            TplChipGridGY & operator=(const TplChipGridGY &rhs) = default;
            ///////////////////////// Constructors /////////////////////////////////////////

            ////////////////////////// Member Access ///////////////////////////////////////
            double green_function(const std::pair<int, int> &idx, const std::pair<int, int> &idx0) const;
            double power_density(int i, int j) const;
            ////////////////////////// Member Access ///////////////////////////////////////

            ///////////////////////// Modifiers   //////////////////////////////////////////
            //! Update the grid size.
            void update_green_function(unsigned int grid_size);
            //! Update the power density.
            void update_power_density();
            ///////////////////////// Modifiers   //////////////////////////////////////////

        private:
            double _r1; //!< Green function R1 radius.
            double _r2; //!< Green function R2 radius.
            unsigned int _grid_size; //!< Storing grid size during tpl algorithm computation.
            double _bin_width;       //!< A grid bin's width.
            double _bin_height;      //!< A grid bin's height.
            std::map<std::pair<std::pair<int, int>, std::pair<int, int> >, double> _green_function; //!< Green function map.
            std::vector<std::vector<double> > _power_density;  //!< Power density map.
    };

    //! The entry class for tpl algorithm.
    class TplAlgorithmGY : public TplAbstractAlgorithm {
        public:


            //! TplAlgorithmGY constructor.
            TplAlgorithmGY();

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
            //! Private helper routine initialize the chip grid.
            void initialize_chip_grid();

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

            TplChipGridGY chip_grid;
    };

}//end namespace tpl

#endif
