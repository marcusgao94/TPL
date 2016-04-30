/*!
 * \file tpl_abstract_algorithm.h
 * \author Peng Fei
 * \brief Interface definition for TPL Algorithm.
 */

#ifndef TPL_ABSTRACT_ALGORITHM_H
#define TPL_ABSTRACT_ALGORITHM_H

#include <memory>

#include "tpl_abstract_net_model.h"
#include "tpl_abstract_net_force_model.h"
#include "tpl_abstract_thermal_model.h"
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
        //! Default constructor.
        TplAbstractAlgorithm() : net_model(nullptr), net_force_model(nullptr),
                thermal_model(nullptr), thermal_force_model(nullptr) {}

        //! Pure virtual destructor.
        virtual ~TplAbstractAlgorithm() = 0;

        //! Interface for initializing the force models.
        virtual void initialize_models() = 0;

        //! Interface for initializing the module placement.
        virtual void make_initial_placement() = 0;

        //! Interface for global placement.
        virtual void make_global_placement() = 0;

        //! Interface for detail placement.
        virtual void make_detail_placement(std::string benchmark) = 0;

        const std::shared_ptr<TplAbstractNetModel> &net_model() const
        {
            return _net_model;
        }

        std::shared_ptr<TplAbstractNetModel> &net_model()
        {
            return _net_model;
        }

        const std::shared_ptr<TplAbstractNetForceModel> &net_force_model() const
        {
            return _net_force_model;
        }

        std::shared_ptr<TplAbstractNetForceModel> &net_force_model()
        {
            return _net_force_model;
        }

        const std::shared_ptr<TplAbstractThermalModel> &thermal_model() const
        {
            return _thermal_model;
        }

        std::shared_ptr<TplAbstractThermalModel> &thermal_model()
        {
            return _thermal_model;
        }

        const std::shared_ptr<TplAbstractThermalForceModel> &thermal_force_model() const
        {
            return _thermal_force_model;
        }

        std::shared_ptr<TplAbstractThermalForceModel> &thermal_force_model()
        {
            return _thermal_force_model;
        }

    protected:
        std::shared_ptr<TplAbstractNetModel>          _net_model;           //!< Pointer to a TplNetModel.
        std::shared_ptr<TplAbstractNetForceModel>     _net_force_model;     //!< Pointer to a TplNetForceModel.
        std::shared_ptr<TplAbstractThermalModel>      _thermal_model;       //!< Pointer to a TplThermalModel.
        std::shared_ptr<TplAbstractThermalForceModel> _thermal_force_model; //!< Pointer to a TplMoveForceModel.
    };

    TplAbstractAlgorithm::~TplAbstractAlgorithm()
    {
    }

}

#endif //TPL_ABSTRACT_ALGORITHM_H
