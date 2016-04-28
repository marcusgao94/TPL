/*!
 * \file tpl_standard_detail_placement.h
 * \author Yue Gao
 * \brief Standard implementation for Detail Placement.
 */

#ifndef TPL_STANDARD_DETAIL_PLACEMENT_H
#define TPL_STANDARD_DETAIL_PLACEMENT_H

#include "tpl_abstract_detail_placement.h"
#include "tpl_db.h"
#include "../tools/def/defwWriter.hpp"
#include "../tools/rippledp/rippledp.h"
#include <cstring>
#include <string>

namespace tpl {

    class TplStandardDetailPlacement : public TplAbstractDetailPlacement {
    public:
        //! Default constructor.
        TplStandardDetailPlacement() : TplAbstractDetailPlacement() {}

        //! Virtual destructor.
        virtual ~TplStandardDetailPlacement() {}

        //! standard implementation of legalization in detail placement
        virtual std::vector<std::vector<TplModule*> > legalization();

        //! standard implementation of detail placement
        /*!
		 * \param rows modules placed in each row
		 */
        virtual void detailPlacement(std::vector<std::vector<TplModule*> > rows);

		//! standard implementation of saving global placement to def format
		/*!
		 * \param benchmark benchmark name
		 */
		virtual void saveDEF(std::string benchmark);

		//! implementation of detail placement using reppledp toolkit
		/*!
		 * \param filename name of the global placement result file
		 */
		virtual void detailPlacement(std::string filename);

	protected:
		int _rowHeight;
		int _chipWidth;
		int _chipHeight;
	};

}//namespace tpl

#endif
