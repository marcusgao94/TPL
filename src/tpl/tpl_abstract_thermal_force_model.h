/*!
 * \file tpl_abstract_thermal_force_model.h
 * \author Peng Fei
 * \brief Interface definition for Thermal Force Model.
 */

#ifndef TPL_ABSTRACT_THERMAL_FORCE_MODEL_H
#define TPL_ABSTRACT_THERMAL_FORCE_MODEL_H

#include "utils.h"

namespace tpl {

    //! Interface definition for tpl move force model.
    class TplAbstractThermalForceModel {
    public:
        //! Default constructor.
        TplAbstractThermalForceModel() = default;

        //! Virtual destructor.
        virtual ~TplAbstractThermalForceModel() {}

        //! Interface for model initialization.
        virtual bool initialize_model() = 0;

        //! Interface for computing heat flux vector in both x and y direction.
        virtual void compute_heat_flux_vector(VectorXd &x_heat_flux, VectorXd &y_heat_flux) = 0;
    };

}

#endif //TPL_ABSTRACT_THERMAL_FORCE_MODEL_H
