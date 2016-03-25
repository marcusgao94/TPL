/*!
 * \file tpl_gy_algorithm.h
 * \author Gao Yue
 * \brief Refined definition for TPL algorithm
 */

#ifndef TPL_GY_ALGORITHM_H
#define TPL_GY_ALGORITHM_H

#include "tpl_standard_algorithm.h"

namespace tpl {

    class TplGYNetForceModel final : public TplStandardNetForceModel {
    public:
        ~TplGYNetForceModel();
    };

    class TplGYThermalForceModel final : public TplStandardThermalForceModel {
    public:
        TplGYThermalForceModel(TplThermalModelInterface *thermal_model);
        ~TplGYThermalForceModel();
    };

    class TplGYAlgorithm final : public TplStandardAlgorithm {
    public:
        ~TplGYAlgorithm();
        void initialize_models() override;
    };

    class TplGYDetailPlacement final : public TplStandardDetailPlacement {
    public:
        std::vector<std::vector<TplModule*> > legalization();
        void detailPlacement(std::vector<std::vector<TplModule*> > rows);
    };

}//end namespace tpl

#endif
