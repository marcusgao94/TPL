/*!
 * \file tpl_standard_algorithm.h
 * \author Peng Fei
 * \brief Standard definition for TPL algorithm interface
 */

#ifndef TPL_STANDARD_ALGORITHM_H
#define TPL_STANDARD_ALGORITHM_H

#include "tpl_abstract_algorithm.h"
#include "tpl_standard_thermal_model.h"
#include "tpl_standard_thermal_force_model.h"
#include "tpl_standard_net_model.h"
#include "tpl_standard_net_force_model.h"
#include "def/defwWriter.hpp"
#include "rippledp.h"

#include "utils.h"

namespace tpl {

    //! Standard implementation for tpl algorithm.
    class TplStandardAlgorithm : public TplAbstractAlgorithm {
    public:
        //! Constructor.
        TplStandardAlgorithm() = default;

        //! Virtual destructor.
        virtual ~TplStandardAlgorithm() {}

        //! Standard implementation for interface initialize_models.
        virtual void initialize_models();

		//! Standard implementation for shredding macros into cells
        virtual void shred();

		//! Standard implementation for aggregating cells into macros
		virtual void aggregate();

        //! Standard implementation for interface make_initial_placement.
        virtual void make_initial_placement();

        //! standard implementation for interface make_global_placement.
        virtual void make_global_placement();

        //! standard implementation of saving global placement to def format
		/*!
		 * \param benchmark benchmark name
		 */
		virtual void saveDEF(std::string benchmark);

		//! standard implementation of detail placement using reppledp toolkit
		/*!
		 * \param filename name of the global placement result file
		 */
        virtual void make_detail_placement(std::string benchmark);

    protected:
        void initialize_move_force_matrix();
        void update_move_force_matrix();

        NetWeight NWx, NWy;
        SpMat Cx,  Cy;
        VectorXd HFx, HFy;
        SpMat Cx0, Cy0;
    };

}//end namespace tpl

#endif //TPL_STANDARD_ALGORITHM_H
