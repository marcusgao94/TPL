/*!
 * \file tpl_standard_macro_placement.h
 * \author Yue Gao
 * \brief Standard implementation for Macro Placement.
 */

#ifndef TPL_STANDARD_MACRO_PLACEMENT_H
#define TPL_STANDARD_MACRO_PLACEMENT_H

#include "tpl_abstract_macro_placement.h"
#include "tpl_db.h"
#include <cstring>
#include <string>

namespace tpl {

    class TplStandardMacroPlacement : public TplAbstractMacroPlacement {
    public:
        //! Default constructor.
        TplStandardMacroPlacement() : TplAbstractMacroPlacement() { }

        //! Virtual destructor.
        virtual ~TplStandardMacroPlacement() { }

        //! Standard implementation for shredding macros into cells
        virtual void shred() {}

    };
}//namespace tpl

#endif
