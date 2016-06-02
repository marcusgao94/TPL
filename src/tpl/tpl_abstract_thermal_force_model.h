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
        TplAbstractThermalForceModel() = default;

        //! Virtual destructor.
<<<<<<< HEAD
        virtual ~TplAbstractThermalForceModel() {}
=======
        virtual ~TplAbstractThermalForceModel() {};
>>>>>>> b466aba5aae08fb253f4ff4a460bf80d34a2d59b

        //! Interface for model initialization.
        virtual bool initialize_model() = 0;
    };

}

#endif //TPL_ABSTRACT_THERMAL_FORCE_MODEL_H
