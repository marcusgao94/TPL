/*!
 * \file tpl_standard_detail_placement.h
 * \author Yue Gao
 * \brief Standard implementation for Detail Placement.
 */

#ifndef TPL_STANDARD_DETAIL_PLACEMENT_H
#define TPL_STANDARD_DETAIL_PLACEMENT_H

#include "tpl_abstract_detail_placement.h"

namespace tpl {

    class TplStandardDetailPlacement : public TplAbstractDetailPlacement {
    public:
        //! Default constructor.
        TplStandardDetailPlacement() : TplAbstractDetailPlacement() {}

        //! Virtual destructor.
        virtual ~TplStandardNetModel() {}

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

}//namespace tpl

#endif
