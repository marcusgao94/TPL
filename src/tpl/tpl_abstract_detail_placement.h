/*!
 * \file tpl_abstract_detail_placement.h
 * \author Yue Gao
 * \brief Interface definition for Detail Placement.
 */

#ifndef TPL_ABSTRACT_DETAIL_PLACEMENT_H
#define TPL_ABSTRACT_DETAIL_PLACEMENT_H

#include <vector>

namespace tpl {

    class TplAbstractDetailPlacement {
    public:
        //! Default constructor.
        TplAbstractDetailPlacement() = default;

        //!< Pure virtual destructor.
        virtual ~TplAbstractDetailPlacement() {}

        // legalization
		virtual void legalization() = 0;

		// detail placement
		virtual void detailPlacement() = 0;
    };

}

#endif
