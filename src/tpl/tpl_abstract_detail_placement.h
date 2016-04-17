/*!
 * \file tpl_abstract_detail_placement.h
 * \author Yue Gao
 * \brief Interface definition for Detail Placement.
 */

#ifndef TPL_ABSTRACT_DETAIL_PLACEMENT_H
#define TPL_ABSTRACT_DETAIL_PLACEMENT_H

#include "tpl_db.h"
#include <vector>

namespace tpl {

    class TplAbstractDetailPlacement {
    public:
        //! Default constructor.
        TplAbstractDetailPlacement() = default;

        //!< Pure virtual destructor.
        virtual ~TplAbstractDetailPlacement() {}

        // legalization
		virtual std::vector<std::vector<TplModule*> > legalization() = 0;

		// detail placement
		virtual void detailPlacement(std::vector<std::vector<TplModule*> > rows) = 0;

    };

}

#endif
