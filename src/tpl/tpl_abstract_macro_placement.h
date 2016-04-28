/*!
 * \file tpl_abstract_macro_placement.h
 * \author Yue Gao
 * \brief Interface definition for Macro Placement.
 */

#ifndef TPL_ABSTRACT_MACRO_PLACEMENT_H
#define TPL_ABSTRACT_MACRO_PLACEMENT_H

#include "tpl_db.h"
#include <vector>

namespace tpl {

    class TplAbstractMacroPlacement {
    public:
        //! Default constructor.
        TplAbstractMacroPlacement() = default;

        //!< Pure virtual destructor.
        virtual ~TplAbstractMacroPlacement() {}

    };

}

#endif
