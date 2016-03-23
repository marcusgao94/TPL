/*!
 * \file tpl_abstract_thermal_model.h
 * \author Peng Fei
 * \brief Interface definition for Thermal Model. 
 */

#ifndef TPL_ABSTRACT_THERMAL_MODEL_H
#define TPL_ABSTRACT_THERMAL_MODEL_H

#include <vector>

namespace tpl {

    class TplAbstractThermalModel {
    public:
        //! Default constructor.
        TplAbstractThermalModel() {}

        //!< Pure virtual destructor.
        virtual ~TplAbstractThermalModel() = 0;

        virtual void compute_thermal_distribution(std::vector<std::vector<double>> &distribution) const = 0;
    };

    TplAbstractThermalModel::~TplAbstractThermalModel()
    {
    }
}

#endif //TPL_ABSTRACT_THERMAL_MODEL_H
