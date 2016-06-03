/*!
 * \file tpl_abstract_algorithm.h
 * \author Peng Fei
 * \brief Interface definition for TPL Algorithm.
 */

#ifndef TPL_ABSTRACT_ALGORITHM_H
#define TPL_ABSTRACT_ALGORITHM_H

#include "tpl_abstract_net_model.h"
#include "tpl_abstract_net_force_model.h"
#include "tpl_abstract_thermal_force_model.h"
#include "tpl_abstract_detail_placement.h"

namespace tpl {

    class TplAbstractNetModel;
    class TplAbstractNetForceModel;
    class TplAbstractThermalModel;
    class TplAbstractThermalForceModel;
    class TplAbstractDetailPlacement;

    //! Interface definition for tpl algorithm.
    class TplAbstractAlgorithm {
    public:
        //! Virtual destructor.
        virtual ~TplAbstractAlgorithm() {}

        //! Interface for initializing the force models.
        virtual void initialize_models() = 0;

        //! Interface for initializing the module placement.
        virtual void make_initial_placement() = 0;

        //! Interface for global placement.
        virtual void make_global_placement() = 0;

        //! Interface for detail placement.
        virtual void make_detail_placement(std::string benchmark) = 0;
    };

}

#endif //TPL_ABSTRACT_ALGORITHM_H
