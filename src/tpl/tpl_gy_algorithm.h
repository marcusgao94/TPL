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

    class TplGYMoveForceModel final : public TplStandardMoveForceModel {
    public:
        TplGYMoveForceModel(double r1, double r2, unsigned int grid_size);
        ~TplGYMoveForceModel();
    };

    class TplGYAlgorithm final : public TplStandardAlgorithm {
    public:
        ~TplGYAlgorithm();
        void initialize_models() override;
    };

}//end namespace tpl

#endif
