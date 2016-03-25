/*!
 * \file tpl_standard_algorithm.h
 * \author Peng Fei
 * \brief Standard definition for TPL algorithm interface
 */

#ifndef TPL_STANDARD_ALGORITHM_H
#define TPL_STANDARD_ALGORITHM_H

#include "tpl_abstract_algorithm.h"

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

        //! Standard implementation for interface make_initial_placement.
        virtual void make_initial_placement();

        //! standard implementation for interface make_global_placement.
        virtual void make_global_placement();

    protected:
        void initialize_move_force_matrix();
        void update_move_force_matrix();

        NetWeight NWx, NWy;
        SpMat Cx,  Cy;
        VectorXd HFx, HFy;
        SpMat Cx0, Cy0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//! standard implementation for detail placement
    class TplStandardDetailPlacement {
	public:
		//! Virtual deconstructor
		virtual ~TplStandardDetailPlacement() {}

		//! standard implementation of legalization in detail placement
		virtual std::vector<std::vector<TplModule*> > legalization();

        //! standard implementation of detail placement
		/*!
		 * \param rows modules placed in each row
		 */
        virtual void detailPlacement(std::vector<std::vector<TplModule*> > rows);
	protected:
		int _rowHeight;
		int _chipWidth;
		int _chipHeight;
	};

}//end namespace tpl

#endif //TPL_STANDARD_ALGORITHM_H
