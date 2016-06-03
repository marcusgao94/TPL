/*!
 * \file tpl_standard_thermal_force_model.h
 * \author Peng Fei
 * \brief Standard implementation for Thermal Force Model.
 */

#ifndef TPL_STANDARD_THERMAL_FORCE_MODEL_H
#define TPL_STANDARD_THERMAL_FORCE_MODEL_H

#include "tpl_abstract_thermal_force_model.h"

#include "utils.h"
#include "tpl_db.h"

#include <map>
#include <utility>
#include <vector>
#include <memory>

#include <stxxl/bits/containers/matrix.h>

namespace tpl {



    /*!
     * \typedef stxxl::matrix<double, 64> TMat;
     * \brief SpMat type
     */
    typedef stxxl::matrix<double, 64> TMat;

    typedef stxxl::block_scheduler<stxxl::matrix_swappable_block<double, 64> > block_schedular_type;

    //! Standard implementation for tpl move force model.
    class TplStandardThermalForceModel : public TplAbstractThermalForceModel {
    public:
        //! Constructor.
        TplStandardThermalForceModel();

        //Destructor.
        virtual ~TplStandardThermalForceModel() {}

        bool initialize_model();

        //! Standard implementation for compute_head_flux_vector.
        void compute_heat_flux_vector(VectorXd &x_heat_flux, VectorXd &y_heat_flux);

        double get_mu() const {
            return MU;
        }

    protected:
        //! Generate the power density.
        void generate_power_density();

        //! Fetch power density for grid (i,j).
        /*!
         * \param i x index for power density.
         * \param j y index for power density.
         */
        double power_density(int i, int j);

        //! Compute green function for head conduction equation.
        /*!
         * \param i  x index for green function's first point.
         * \param j  y index for green function's first point.
         * \param i0  x index for green function's second point.
         * \param j0  y index for green function's second point.
         */
        double green_function(int i, int j, int i0, int j0);

        //! Generate the thermal profile of the chip.
        void generate_thermal_profile();

        //! Generate heat flux grid for both x and y direction.
        void generate_heat_flux_grid();

        int _gw_num;     //!< Number of bin in x direction.
        int _gh_num;     //!< Number of bin in y direction.

        block_schedular_type _bs;
        std::shared_ptr<TMat> _power_density; //!< Power density container.
        std::shared_ptr<TMat> _thermal_signature; //!< Thermal profile for the chip.
        std::shared_ptr<TMat> _xhf_grid; //!< Heat flux grid in x direction.
        std::shared_ptr<TMat> _yhf_grid; //!< Heat flux grid in y direction.

        std::shared_ptr< std::vector< std::vector<double> > > _green_function;

        double BIN_WIDTH;  //!< Algorithm parameter : a grid bin's width.
        double BIN_HEIGHT; //!< Algorithm parameter : a grid bin's height.
        double R1; //!< Algorithm parameter : green function R1.
        double R2; //!< Algorithm parameter : green function R2.
        double MU; //!< Algorithm parameter : qanlity control parameter.
    };
}

#endif //TPL_STANDARD_THERMAL_FORCE_MODEL_H
