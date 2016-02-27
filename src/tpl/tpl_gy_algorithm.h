/*!
 * \file tpl_gy_algorithm.h
 * \author Gao Yue
 * \brief Refined definition for TPL algorithm
 */

#ifndef TPL_GY_ALGORITHM_H
#define TPL_GY_ALGORITHM_H

#include "tpl_standard_algorithm.h"

namespace tpl {
    class TplGYNetForceModel : public TplStandardNetForceModel {
        public:
            virtual ~TplGYNetForceModel() {};
    };

    class TplGYMoveForceModel : public TplStandardMoveForceModel {
        public:
            TplGYMoveForceModel(double r1, double r2, unsigned int grid_size);
            virtual ~TplGYMoveForceModel() {};
    };

    class TplGYAlgorithm : public TplStandardAlgorithm {
        public:
            virtual ~TplGYAlgorithm() {};
            virtual void initialize_models();

    };

}//end namespace tpl

#endif
