/*!
 * \file tpl_abstract_thermal_force_model.h
 * \author Peng Fei
 * \brief Interface definition for Thermal Force Model.
 */

#ifndef TPL_ABSTRACT_THERMAL_FORCE_MODEL_H
#define TPL_ABSTRACT_THERMAL_FORCE_MODEL_H

#include <memory>

#include "utils.h"
#include "tpl_abstract_thermal_model.h"

namespace tpl {

    //! Interface definition for tpl move force model.
    class TplAbstractThermalForceModel {
    public:
        //! Default constructor.
        explicit  TplAbstractThermalForceModel(std::shared_ptr<TplAbstractThermalModel> tmodel) : _tmodel(tmodel) {}

        //! Virtual destructor.
        virtual ~TplAbstractThermalForceModel() {}

        virtual void compute_heat_flux_vector(const std::vector<std::vector<double>> &tss, VectorXd &HFx, VectorXd &HFy) const = 0;

    protected:
        std::shared_ptr<TplAbstractThermalModel> _tmodel;
    };

}

#endif //TPL_ABSTRACT_THERMAL_FORCE_MODEL_H
