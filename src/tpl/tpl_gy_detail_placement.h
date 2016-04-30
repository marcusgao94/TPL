/*!
 * \file tpl_standard_detail_placement.h
 * \author Yue Gao
 * \brief Standard implementation for Detail Placement.
 */

#ifndef TPL_GY_DETAIL_PLACEMENT_H
#define TPL_GY_DETAIL_PLACEMENT_H

#include "tpl_abstract_detail_placement.h"
#include "tpl_db.h"
#include "../tools/def/defwWriter.hpp"
#include "../tools/rippledp/rippledp.h"
#include <cstring>
#include <string>

namespace tpl {

    class TplGyDetailPlacement {
    public:
        //! Default constructor.
        TplGyDetailPlacement() {}

        //! Virtual destructor.
        virtual ~TplGyDetailPlacement() {}

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
