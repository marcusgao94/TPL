/*!
 * \file tpl_standard_thermal_force_model.h
 * \author Peng Fei
 * \brief Standard implementation for Thermal Force Model.
 */

#ifndef TPL_STANDARD_THERMAL_FORCE_MODEL_H
#define TPL_STANDARD_THERMAL_FORCE_MODEL_H

#include "tpl_abstract_thermal_force_model.h"

namespace tpl {

    //! Standard implementation for tpl move force model.
    class TplStandardThermalForceModel : public TplAbstractThermalForceModel {
    public:
        //! Constructor.
        explicit  TplStandardThermalForceModel(std::shared_ptr<TplAbstractThermalModel> tmodel) : TplAbstractThermalForceModel(tmodel) {}

        //! Virtual destructor.
        virtual ~TplStandardThermalForceModel() {}

        //! Standard implementation for compute_head_flux_vector.
        virtual void compute_heat_flux_vector(std::vector<std::vector<double>> &tss, VectorXd &x_heat_flux, VectorXd &y_heat_flux) const;

    private:
        static const int TIMES;
    };

}

#endif //TPL_STANDARD_THERMAL_FORCE_MODEL_H
